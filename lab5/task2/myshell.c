#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <linux/limits.h>
#include <stdlib.h>
#include "LineParser.h"
#include <string.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_USER_LINE 2048
#define TERMINATED -1
#define RUNNING 1
#define SUSPENDED 0

typedef struct process
{
    cmdLine *cmd;         /* the parsed command line*/
    pid_t pid;            /* the process id that is running the command*/
    int status;           /* status of the process: RUNNING/SUSPENDED/TERMINATED */
    struct process *next; /* next process in chain */
} process;

void destroy_all_processes(process **processes);
void destroy_single_process(process *proc);
process *makeProcess(cmdLine *cmd_line, pid_t pid, int status, process *next_proc);
void freeProcessList(process *process_list);
void updateProcessList(process **process_list);
void updateProcessStatus(process *process_list, int pid, int status);

process *makeProcess(cmdLine *cmd_line, pid_t pid, int status, process *next_proc)
{
    process *proc = (process *)(malloc(sizeof(process)));
    proc->cmd = cmd_line;
    proc->pid = pid;
    proc->status = status;
    proc->next = next_proc;
    return proc;
}

void destroy_single_process(process *proc)
{
    freeCmdLines(proc->cmd);
    free(proc);
}
void freeProcessList(process *process_list)
{
    while (process_list != NULL)
    {
        process *tmp = process_list;
        freeCmdLines(tmp->cmd);
        process_list = process_list->next;
        free(tmp);
    }
}

void destroy_all_processes(process **processes)
{
    freeProcessList(*processes);
    free(processes);
}

void addProcess(process **process_list, cmdLine *cmd, pid_t pid)

{
    if (*process_list == NULL)
    {
        *process_list = makeProcess(cmd, pid, RUNNING, NULL);
    }
    else
    {
        addProcess(&(*process_list)->next, cmd, pid);
    }
}

void printProcessList(process **process_list)
{
    printf("%s", "---------- PRINTING PROCESSES ---------- \n\n");
    updateProcessList(process_list);
    int i = 0;
    process *curr = *process_list;
    process *prev = NULL;
    while (curr != NULL)
    {
        printf("index : %d id: %d ", i, curr->pid);
        char *str_status;
        switch (curr->status)
        {
        case 1:
            str_status = "RUNNING";
            break;
        case 0:
            str_status = "SUSPENDED";
            break;
        default: //case -1
            str_status = "TERMINATED";
            break;
        }
        printf("status: %s ", str_status);
        printf("command : %s ", curr->cmd->arguments[0]);
        for (int j = 0; curr->cmd->arguments[j] != NULL; j++)
        {
            printf("arg %d : %s ", j, curr->cmd->arguments[j]);
        }
        if (curr->status == TERMINATED)
        {
            if (prev == NULL)
            {
                *process_list = curr->next;
                process *tmp = curr;
                curr = curr->next;
                destroy_single_process(tmp);
            }
            else
            {
                prev->next = curr->next;
                process *tmp = curr;
                curr = curr->next;
                destroy_single_process(tmp);
            }
        }
        else
        {
            prev = curr;
            curr = curr->next;
        }
        printf("\n");
        i++;
    }
    printf("\n");
    printf("----------------------------------------");
    printf("%s", "\n\n");
}

void updateProcessList(process **process_list)
{
    process *proc = *process_list;
    int status;
    while (proc != NULL)
    {
        if (waitpid(proc->pid, &status, WNOHANG | WUNTRACED | WCONTINUED) > 0)
        {
            if (WIFSTOPPED(status))
            {
                updateProcessStatus(proc, proc->pid, SUSPENDED);
            }
            else if (WIFCONTINUED(status))
            {
                updateProcessStatus(proc, proc->pid, RUNNING);
            }
            else if (WIFEXITED(status) || WIFSIGNALED(status))
            {
                updateProcessStatus(proc, proc->pid, TERMINATED);
            }
        }
        else if (waitpid(proc->pid, &status, WNOHANG) == -1)
        {
            perror("update process list error \n");
        }

        proc = proc->next;
    }
}
void updateProcessStatus(process *process_list, int pid, int status)
{
    while (process_list != NULL)
    {
        if (process_list->pid == pid)
        {
            process_list->status = status;
        }
        process_list = process_list->next;
    }
}

void cd(cmdLine *cmd_line_ptr)
{
    if (cmd_line_ptr->arguments[1] != NULL)
    {
        if (chdir(cmd_line_ptr->arguments[1]) != 0)
        {
            perror("can not change directory \n");
        }
    }
    else if (chdir("..") != 0)
    {
        perror("can not change directory \n");
    }
}
void my_wait_pid(int pid)
{
    waitpid(pid, NULL, 0);
}

void execute(cmdLine *cmd_line_ptr, bool debug_mode)
{
    if (debug_mode)
    {
        fprintf(stderr, "%s", "child executing command! \n");
    }
    execvp(cmd_line_ptr->arguments[0], cmd_line_ptr->arguments);
    perror("There was an error executing command: ");
    perror(cmd_line_ptr->arguments[0]);
    perror("\n");
    freeCmdLines(cmd_line_ptr);
    exit(1);
}


int main(int argc, char *argv[])
{

    bool debug_mode = false;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-d") == 0)
        {
            debug_mode = true;
        }
    }
    char buffer[PATH_MAX];
    char userLine[MAX_USER_LINE];
    cmdLine *cmd_line;
    pid_t pid;
    process **processes = (process **)(malloc(sizeof(process *)));
    *processes = NULL;
    while (true)
    {
        getcwd(buffer, PATH_MAX);
        printf("%s \n", buffer);
        fgets(userLine, MAX_USER_LINE, stdin);
        cmd_line = parseCmdLines(userLine);
        if (strcmp(cmd_line->arguments[0], "quit") == 0)
        {
            freeCmdLines(cmd_line);
            break;
        }
        else if (strcmp(cmd_line->arguments[0], "cd") == 0)
        {
            cd(cmd_line);
            freeCmdLines(cmd_line);
        }
        else if (strcmp(cmd_line->arguments[0], "procs") == 0)
        {
            printProcessList(processes);
            freeCmdLines(cmd_line);
        }
        else if (strcmp(cmd_line->arguments[0], "suspend") == 0)
        {
            int proc_to_suspend = atoi(cmd_line->arguments[1]);
            if(kill(proc_to_suspend, SIGTSTP) < 0) {
                perror("couldn't suspend process number: ");
                perror(cmd_line->arguments[1]); perror("\n");
            };
            freeCmdLines(cmd_line);
        }
        else if (strcmp(cmd_line->arguments[0], "kill") == 0)
        {
            int proc_to_kill = atoi(cmd_line->arguments[1]);
            if(kill(proc_to_kill, SIGINT) < 0) {
                perror("couldn't kill process number: ");
                perror(cmd_line->arguments[1]); perror("\n");
            };
            freeCmdLines(cmd_line);
        }
        else if (strcmp(cmd_line->arguments[0], "wake") == 0)
        {
            int proc_to_wake = atoi(cmd_line->arguments[1]);
             if(kill(proc_to_wake, SIGCONT) < 0) {
                perror("couldn't wake process number: ");
                perror(cmd_line->arguments[1]); perror("\n");
            };
            freeCmdLines(cmd_line);
        }
        else
        {
            if (!(pid = fork()))
            {
                execute(cmd_line, debug_mode);
            }

            if (!(cmd_line->blocking))
            {
                addProcess(processes, cmd_line, pid);
            }
            else
            {
                my_wait_pid(pid);
                freeCmdLines(cmd_line);
            }
            if (debug_mode)
            {
                fprintf(stderr, "%s %d %s", "The parent pid is: ", pid, "\n");
            }
        }
    }
    destroy_all_processes(processes);
    return 0;
}