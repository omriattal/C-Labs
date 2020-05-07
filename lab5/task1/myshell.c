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

void execute(cmdLine *cmd_line_ptr, bool debug_mode)
{
    if (debug_mode)
    {
        fprintf(stderr, "%s", "child executing command! \n");
    }

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
    char buffer[PATH_MAX];
    char userLine[MAX_USER_LINE];
    cmdLine *cmd_line;
    pid_t pid;
    while (true)
    {
        getcwd(buffer, PATH_MAX);
        printf("%s \n", buffer);
        fgets(userLine, MAX_USER_LINE, stdin);
        if (strncmp(userLine, "quit", 4) == 0)
        {
            free(cmd_line);
            break;
        }
        cmd_line = parseCmdLines(userLine);
        if (strcmp(cmd_line->arguments[0], "cd") == 0)
        {
            if(cmd_line->arguments[1] != NULL) {
                if(chdir(cmd_line->arguments[1])!=0) {
                     perror("can not change directory \n");
                };
            }
            else if(chdir("..")!=0) {
                perror("can not change directory \n");
            }
        } else if (!(pid = fork()))
        {
            execute(cmd_line, debug_mode);
        }
        
        if (debug_mode)
        {
            fprintf(stderr, "%s %d %s", "The parent pid is: ", pid, "\n");
        }
        
        waitpid(pid, NULL, 0);
    }
    return 0;
}