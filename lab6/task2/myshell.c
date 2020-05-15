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

typedef struct Binding
{
    char *var;
    char *value;
    struct Binding *next;

} BINDING;

BINDING *makeBinding(char *var, char *value, BINDING *next);
void destroy_single_binding(BINDING *binding);
void destroy_binding_list(BINDING *binding);
void add_binding(BINDING **bindings, char *var, char *value);
bool check_binding(BINDING *binding, char *var);
void set_new_binding(BINDING *binding, char *value);
void print_bindings(BINDING **bindings);

//BINDING RELATED FUNCTIONS
BINDING *makeBinding(char *var, char *value, BINDING *next)
{
    BINDING *binding = (BINDING *)(malloc(sizeof(BINDING)));
    binding->var = strcpy(calloc(strlen(var),1),var);
    binding->value = strcpy(calloc(strlen(value),1),value);
    binding->next = next;
    return binding;

}
void destroy_single_binding(BINDING *binding)
{
    free(binding->var);
    free(binding->value);
    free(binding);
}

void destroy_binding_list(BINDING *bindings)
{
    while (bindings != NULL)
    {
        BINDING *tmp = bindings;
        free(tmp->var);
        free(tmp->value);
        bindings = bindings->next;
        free(tmp);
    }
}

void destroy_all_bindings(BINDING **bindings)
{
    destroy_binding_list(*bindings);
    free(bindings);
}

void add_binding(BINDING **bindings, char *var, char *value)
{
    if (*bindings == NULL)
    {
        *bindings = makeBinding(var, value, NULL);
    }
    else
    {
        if (check_binding(*bindings, var))
        {
            set_new_binding(*bindings, value);
        }
        else
        {
            add_binding(&(*bindings)->next, var, value);
        }
    }
}

bool check_binding(BINDING *binding, char *var)
{
    return strcmp(binding->var, var) == 0;
}

void set_new_binding(BINDING *binding, char *value)
{
    free(binding->value);
    binding->value = strcpy(calloc(strlen(value),1),value);
}

void print_bindings(BINDING **bindings)
{
    BINDING *binding = *bindings;
    printf("%s", "\n------------ PRINTING VARIABLES ------------\n\n");
    while (binding != NULL)
    {
        printf("%s", "VAR: ");
        printf("%s\t", binding->var);
        printf("%s", "VALUE: ");
        printf("%s\n", binding->value);
        binding = binding->next;
    }
    printf("%s", "\n--------------------------------------------\n");
}

void redirect(cmdLine *cmd_line_ptr)
{
    if (cmd_line_ptr->inputRedirect != NULL)
    {
        if (fclose(stdin) != -1)
        {
            if (fopen(cmd_line_ptr->inputRedirect, "r") == NULL)
            {
                perror("error redirecting the input");
            }
        }
        else
        {
            perror("error redirecting the input");
        }
    }
    if (cmd_line_ptr->outputRedirect != NULL)
    {
        if (fclose(stdout) != -1)
        {
            if (fopen(cmd_line_ptr->outputRedirect, "w") == NULL)
            {
                perror("error redirecting the output");
            }
        }
        else
        {
            perror("error redirecting the output");
        }
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
void my_wait_pid(int pid, int blocking)
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
    redirect(cmd_line_ptr);
    execvp(cmd_line_ptr->arguments[0], cmd_line_ptr->arguments);
    perror("There was an error executing \n");
    free(cmd_line_ptr);
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
    BINDING **binding_list = (BINDING **)(malloc(sizeof(BINDING *)));
    *binding_list = NULL;
    int pipefd[2];
    char buffer[PATH_MAX];
    char userLine[MAX_USER_LINE];
    cmdLine *cmd_line;
    pid_t pid;
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
        }
        else if (strcmp(cmd_line->arguments[0], "set") == 0)
        {
            if (cmd_line->arguments[1] == NULL || cmd_line->arguments[2] == NULL)
            {
                perror("invalid set operation\n");
            }
            else
            {
                add_binding(binding_list, cmd_line->arguments[1], cmd_line->arguments[2]);
            }

        } 
        else if (strcmp(cmd_line->arguments[0], "vars") == 0)
        {
            print_bindings(binding_list);
        }
        else if (!(pid = fork()))
        {
            execute(cmd_line, debug_mode);
        }

        if (debug_mode)
        {
            fprintf(stderr, "%s %d %s", "The parent pid is: ", pid, "\n");
        }

        my_wait_pid(pid, cmd_line->blocking);
        freeCmdLines(cmd_line);

    }
    destroy_all_bindings(binding_list);
    return 0;
}