#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <linux/limits.h>
#include <stdlib.h>
#include "LineParser.h"
#include <string.h>
#define MAX_USER_LINE 2048

void execute(cmdLine* cmd_line_ptr) {
    if(execv(cmd_line_ptr->arguments[0], cmd_line_ptr->arguments) < 0) {
        perror("There was an error executing \n");
        free(cmd_line_ptr);
        exit(1);
    };
}

int main(int argc, char *argv[])
{
    char buffer[PATH_MAX];
    char userLine[MAX_USER_LINE];
    cmdLine *cmd_line;
    while (true)
    {
        getcwd(buffer, PATH_MAX);
        printf("The current working direcrtory : %s \n", buffer);
        fgets(userLine,MAX_USER_LINE,stdin);
        if(strncmp(userLine,"quit",4)==0) {
            free(cmd_line);
            break;
        }
        cmd_line = parseCmdLines(userLine);
        execute(cmd_line);
    }
    return 0;
}