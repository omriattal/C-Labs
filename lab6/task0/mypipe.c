#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_BUFFER_SIZE 1024
int main(int argc, char *argv[])
{
    int pipefd[2];
    pid_t cpid;
    char buffer[MAX_BUFFER_SIZE];
    if (pipe(pipefd) == -1)
    {
        perror("pipe error");
        exit(1);
    }
    cpid = fork();
    if (cpid == 0)
    {
        write(pipefd[1], "hello", 6);
        exit(0);
    }
    else
    {
        read(pipefd[0], buffer, MAX_BUFFER_SIZE);
        printf("%s\n", buffer);
        close(pipefd[0]);
        close(pipefd[1]);
        exit(0);
    }
}
