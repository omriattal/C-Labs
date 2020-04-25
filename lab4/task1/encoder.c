#include <stdbool.h>
#include "util.h"

#define a_ASCII_NUMBER 97
#define z_ASCII_NUMBER 122
#define nullchar 0
#define TO_UPPER_CASE 32
#define newline_ASCII_NUMBER 10
#define OFFSET 2
#define SYS_WRITE 4
#define O_RDONLY 0
#define STDOUT 1
#define SYS_LSEEK 19
#define SYS_READ 3
#define SYS_OPEN 5
#define O_RDRW 2
#define SYS_CLOSE 6
#define SYS_EXIT 1
#define SUCSSEFUL_EXIT_CODE 0
#define UNSUCSSEFUL_EXIT_CODE 0x55
#define STDIN 0
#define STDERR 2
#define WRONLY 1

bool isLowerCaseLetter(char c);

int main(int argc, char *argv[], char *envp[])
{
    bool debugMode = false;
    int output_file_descriptor = STDOUT;
    int input_file_descriptor = STDIN;
    char* id_sys_call;
    char* return_from_sys_call;
    char input[1] = "\0";
    int i;
    for (i = 1; argv[i] != nullchar && i <= argc; i++) {
        if (strcmp(argv[i], "-D") == 0) {
            debugMode = true;;
        }
        if (strncmp(argv[i], "-o", 2) == 0) {
            output_file_descriptor = system_call(SYS_OPEN,argv[i]+ OFFSET,WRONLY);    
            if(output_file_descriptor < 0) {
                system_call(SYS_WRITE,STDOUT,"Invalid output file!\n",21);
                system_call(SYS_EXIT,UNSUCSSEFUL_EXIT_CODE);
            }
        }
        if(strncmp(argv[i],"-i",2) == 0) {
            input_file_descriptor = system_call(SYS_OPEN,argv[i]+OFFSET,O_RDONLY);
            if(input_file_descriptor < 0) {
                system_call(SYS_WRITE,STDOUT,"Invalid input file!\n",20);
                system_call(SYS_EXIT,UNSUCSSEFUL_EXIT_CODE);
            }
        }
    }

    
    while (true)
    {
        int returned_from_read = system_call(SYS_READ, input_file_descriptor, input, 1);
        if(returned_from_read<=0) {
            break;
        }
        if(input[0] != newline_ASCII_NUMBER) {
        return_from_sys_call= itoa(returned_from_read);
        id_sys_call = itoa(SYS_READ);
        if(debugMode) {
            system_call(SYS_WRITE,STDERR,"",1);
            system_call(SYS_WRITE,STDERR,return_from_sys_call,strlen(return_from_sys_call));
            system_call(SYS_WRITE,STDERR,"\t",1);
            system_call(SYS_WRITE,STDERR,id_sys_call,strlen(id_sys_call));
            system_call(SYS_WRITE,STDERR,"\n",1);
        }
        if (isLowerCaseLetter(input[0]))
        {
            input[0] = input[0] - TO_UPPER_CASE;
        }
         return_from_sys_call = itoa(system_call(SYS_WRITE, output_file_descriptor, input, 1));
         id_sys_call = itoa(SYS_WRITE);
        if(debugMode) {
            system_call(SYS_WRITE,STDERR,"",1);
            system_call(SYS_WRITE,STDERR,return_from_sys_call,strlen(return_from_sys_call));;
            system_call(SYS_WRITE,STDERR,"\t",1);
            system_call(SYS_WRITE,STDERR,id_sys_call,strlen(id_sys_call));
            system_call(SYS_WRITE,STDERR,"\n",1);
        }
        
    } else
    {
        system_call(SYS_WRITE,output_file_descriptor,"\n",1);
    }
    
    }
    system_call(SYS_CLOSE,input_file_descriptor);
    system_call(SYS_CLOSE,output_file_descriptor);
    return 0;
}

bool isLowerCaseLetter(char c)
{
    return c >= a_ASCII_NUMBER && c <= z_ASCII_NUMBER;
}