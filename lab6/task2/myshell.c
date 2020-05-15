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
char *copy_string(char *str);
void destroy_single_binding(BINDING *binding);
void destroy_binding_list(BINDING *binding);
void add_binding(BINDING **bindings, char *var, char *value);
bool check_binding(BINDING *binding, char *var);
void set_new_binding(BINDING *binding, char *value);
void print_bindings(BINDING **bindings);
char *get_value(BINDING **bindings, char *var);

//BINDING RELATED FUNCTIONS
BINDING *makeBinding(char *var, char *value, BINDING *next)
{
    BINDING *binding = (BINDING *)(malloc(sizeof(BINDING)));
    binding->var = copy_string(var);
    binding->value = copy_string(value);
    binding->next = next;
    return binding;
}

char *copy_string(char *str)
{
    return strcpy(calloc(strlen(str) + 1, 1), str);
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
    binding->value = copy_string(value);
}

char *get_value(BINDING **bindings, char *var)
{

    BINDING *binding = *bindings;
    while (binding != NULL)
    {
        if (strcmp(binding->var, var) == 0)
        {
            return binding->value;
        }
        binding = binding->next;
    }
    return NULL;
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

void apply_bindings(BINDING **bindings, cmdLine *cmd_line)
{
    for (int i = 0; i < cmd_line->argCount; i++)
    {
        char *arg = cmd_line->arguments[i];
        if (strncmp(arg, "$", 1) == 0)
        {
            char *arg_value = get_value(bindings, arg + 1);
            if (arg_value != NULL)
            {
                replaceCmdArg(cmd_line, i, arg_value);
            }
            else
            {
                perror("variable not found :( \n");
            }
        }
    }
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

        if (strcmp(cmd_line_ptr->arguments[1], "~") == 0)
        {
            char *path = getenv("HOME");
            if (path != NULL)
            {
                if (chdir(path) != 0)
                {
                    perror("can not change directory \n");
                }
            }
            else
            {
                perror("can not change directory");
            }
        }
        else if (chdir(cmd_line_ptr->arguments[1]) != 0)
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
        apply_bindings(binding_list, cmd_line);
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
            freeCmdLines(cmd_line);
        }
        else if (strcmp(cmd_line->arguments[0], "vars") == 0)
        {
            print_bindings(binding_list);
            freeCmdLines(cmd_line);
        }
        else
        {
            if (!(pid = fork()))
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
    }
    destroy_all_bindings(binding_list);
    return 0;
}