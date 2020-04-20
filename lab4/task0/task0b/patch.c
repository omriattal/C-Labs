#include "util.h"
#include <stdio.h>

#define SYS_WRITE 4
#define STDOUT 1
#define SYS_LSEEK 19
#define SYS_READ 3
#define SYS_OPEN 5
#define O_RDRW 2
#define SYS_CLOSE 6
#define SYS_EXIT 1
#define SUCSSEFUL_EXIT_CODE 0
#define UNSUCSSEFUL_EXIT_CODE 0x55
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

void quit_or_proceed(unsigned int returned_from_sys_call) {
    if(returned_from_sys_call < 0) {
        system_call(SYS_EXIT,UNSUCSSEFUL_EXIT_CODE,0,0);
    }

}

int main (int argc , char* argv[], char* envp[]) {

    char* file_name = argv[1];
    char* x_name = argv[2];
    unsigned int file_descriptor = 0;
    int shira_location = 657;
   
    file_descriptor = system_call(SYS_OPEN,file_name,O_RDRW);
    system_call(SYS_LSEEK,file_descriptor,shira_location,SEEK_SET);
    system_call(SYS_WRITE,file_descriptor,x_name,strlen(x_name));
    system_call(SYS_WRITE,file_descriptor,"\0",1);
    system_call(SYS_CLOSE,file_descriptor);
    return 0;    

    
}