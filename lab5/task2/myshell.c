#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <linux/limits.h>
#include <stdlib.h>
#include "LineParser.h"
#include <string.h>
#include <sys/wait.h>

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
void destroy_processes(process *proc);
process *makeProcess(cmdLine *cmd_line, pid_t pid, int status, process *next_proc);

process *makeProcess(cmdLine *cmd_line, pid_t pid, int status, process *next_proc)
{
    process *proc = (process *)(malloc(sizeof(process)));
    proc->cmd = cmd_line;
    proc->pid = pid;
    proc->status = status;
    proc->next = next_proc;
    return proc;
}

void destroy_processes(process *proc)
{
    while (proc != NULL)
    {
        process *tmp = proc;
        freeCmdLines(tmp->cmd);
        proc = proc->next;
        free(tmp);
    }
}

void destroy_all_processes(process **processes)
{
    destroy_processes(*processes);
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
    printf("%s", "----printing processes ----- \n");
    int i = 0;
    process *proc = *process_list;
    while (proc != NULL)
    {
        printf("index : %d id: %d ", i, proc->pid);
        char *str_status;
        switch (proc->status)
        {
        case 1:
            str_status = "RUNNING";
            break;
        case 0:
            str_status = "SUSPENDED";
            break;
        default:
            str_status = "TERMINATED";
            break;
        }
        printf("status: %s ", str_status);
        printf("command : %s ", proc->cmd->arguments[0]);
        for (int j = 0; proc->cmd->arguments[j] != NULL; j++)
        {
            printf("arg %d : %s ", j, proc->cmd->arguments[j]);
        }
        printf("\n");
        i++;
        proc = proc->next;
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
void wait_pid(int pid, int blocking)
{
    if (blocking)
    {
        waitpid(pid, NULL, 0);
    }
}
void execute(cmdLine *cmd_line_ptr, bool debug_mode)
{
    if (debug_mode)
    {
        fprintf(stderr, "%s", "child executing command! \n");
    }
    execvp(cmd_line_ptr->arguments[0], cmd_line_ptr->arguments);
    perror("There was an error executing \n");
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
        if (strncmp(userLine, "quit", 4) == 0)
        {
            break;
        }
        cmd_line = parseCmdLines(userLine);
        if (strcmp(cmd_line->arguments[0], "cd") == 0)
        {
            cd(cmd_line);
        }
        else if (strcmp(cmd_line->arguments[0], "procs") == 0)
        {
            printProcessList(processes);
        }
        else if (!(pid = fork()))
        {
            execute(cmd_line, debug_mode);
        }
        addProcess(processes, cmd_line, pid);

        if (debug_mode)
        {
            fprintf(stderr, "%s %d %s", "The parent pid is: ", pid, "\n");
        }
        wait_pid(pid, cmd_line->blocking);
    }
    destroy_all_processes(processes);
    return 0;
}