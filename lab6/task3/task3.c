#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <linux/limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

void debug_print(bool debug_mode,char *str) {
    if(debug_mode) {
        fprintf(stderr,"%s",str);
    }
}
void debug_print_with_int(bool debug_mode, char *str,pid_t pid) {
    if(debug_mode) {
        fprintf(stderr,"%s %d \n",str,pid);
    }
}

int main(int argc, char* argv[]) {

    bool debug_mode = false;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-d") == 0)
        {
            debug_mode = true;
        }
    }

    int pipefd[2]; // 0 read-end, 1 - write end
    if(pipe(pipefd) == -1 ) {
        perror("pipe error\n");
        exit(1);
    }
    pid_t cpid;
    char *ls_cmd[3];ls_cmd[0] = "ls";ls_cmd[1] = "-l";ls_cmd[2] = NULL;
    char *tail_cmd[4];tail_cmd[0]="tail";tail_cmd[1]="-n";tail_cmd[2]="2";tail_cmd[3] = NULL;
    pid_t child1,child2;


    debug_print(debug_mode,"parent process > forking\n");
    if(!(cpid = fork())) { // child 1 
        fclose(stdout); // closing stdout
        dup(pipefd[1]);
        if(debug_mode) {}
        debug_print(debug_mode,"child 1 > redirecting stdout to the write end of the pipe\n");
        close(pipefd[1]);
        execvp(ls_cmd[0],ls_cmd);
        debug_print(debug_mode,"child 1 > going to executre cmd: ls -l\n");
        perror("child 1 : error exectuing command");
        exit(1);
    }
    child1 = cpid;
    debug_print_with_int(debug_mode,"parent process > created process with id: ",child1);
    debug_print(debug_mode,"parent process > closing the write end of the pipe\n");
    close(pipefd[1]);



    debug_print(debug_mode,"parent process > forking\n");
    if(!(cpid = fork())) { // child 2
        fclose(stdin);
        dup(pipefd[0]);
        debug_print(debug_mode,"child 2 > redirecting stdout to the write end of the pipe\n");
        close(pipefd[0]);
        debug_print(debug_mode,"child 2 > going to executre cmd: tail -n 2\n");
        execvp(tail_cmd[0],tail_cmd);
        perror("child 2 : error exectuing command");
        exit(1);
    }
    child2 = cpid;
    debug_print_with_int(debug_mode,"parent process > created process with id: ",child2);
    

    debug_print(debug_mode,"parent process > closing the read end of the pipe\n");
    close(pipefd[0]);
    

    debug_print(debug_mode,"parent process > waiting for child processes to terminate\n");
    waitpid(child1,NULL,0);
    waitpid(child2,NULL,0);

    debug_print(debug_mode,"parent process > exiting\n");
    return 0;

}

