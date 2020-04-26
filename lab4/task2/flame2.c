#include "util.h"
#include <stdbool.h>

#define nullchar 0
#define PROGRAM_ARG_OFFSET 2
#define SYS_WRITE 4
#define O_RDONLY 0
#define STDOUT 1
#define SYS_LSEEK 19
#define SYS_GETDENTS 141
#define SYS_READ 3
#define SYS_OPEN 5
#define O_RDRW 2
#define SYS_CLOSE 6
#define SYS_EXIT 1
#define SUCSSEFUL_EXIT_CODE 0
#define UNSUCSSEFUL_EXIT_CODE 0x55
#define FILE_PERMISSION 0777
#define STDIN 0
#define STDERR 2
#define WRONLY 1
#define MAX_BYTES_IN_DIRECTORY 8192


#define DT_UNKNOWN 0
#define DT_FIFO 1
#define DT_CHR 2
#define DT_DIR 4
#define DT_BLK 7
#define DT_REG 8
#define DT_LNK 10
#define DT_SOCK 12
#define DT_WHT 14

typedef struct ent
{
    int inode;
    int offset;
    short len;
    char buf[1];

} ent;

int quit_or_proceed(unsigned int returned_from_sys_call, char *error_msg);
void print_ent(ent *ent_ptr, bool debug_mode);
int quit_or_proceed(unsigned int returned_from_sys_call, char *error_msg)
{
    if (returned_from_sys_call < 0)
    {
        system_call(SYS_WRITE, STDOUT, error_msg, strlen(error_msg));
        system_call(SYS_EXIT, UNSUCSSEFUL_EXIT_CODE, 0, 0);
    }
    return returned_from_sys_call;
}

void debug_print(unsigned int EAX)
{
    system_call(SYS_WRITE, STDERR, "[Debug param: ", 14);
    system_call(SYS_WRITE, STDERR, itoa(EAX), strlen(itoa(EAX)));
    system_call(SYS_WRITE, STDERR, " ", 1);
}

int call1(unsigned int EAX, char *EBX, unsigned int ECX, unsigned int EDX, bool debug_mode, char *error_msg)
{
    if (debug_mode)
    {
        debug_print(EAX);
    }
    int result = quit_or_proceed(system_call(EAX, EBX, ECX, EDX), error_msg);
    if (debug_mode)
    {
        system_call(SYS_WRITE, STDERR, " return: ", 9);
        system_call(SYS_WRITE, STDERR, itoa(result), itoa(result));
        system_call(SYS_WRITE, STDERR, "]", 1);
        system_call(SYS_WRITE, STDERR, " \n", 2);
    }
    return result;
}
int call2(unsigned int EAX, unsigned int EBX, char *ECX, unsigned int EDX, bool debug_mode, char *error_msg)
{
    if (debug_mode)
    {
        debug_print(EAX);
    }
    int result = quit_or_proceed(system_call(EAX, EBX, ECX, EDX), error_msg);
    if (debug_mode)
    {
        system_call(SYS_WRITE, STDERR, " return: ", 9);
        system_call(SYS_WRITE, STDERR, itoa(result), itoa(result));
        system_call(SYS_WRITE, STDERR, "]", 1);
        system_call(SYS_WRITE, STDERR, " \n", 2);
    }
    return result;
}

void print_ent(ent *ent_ptr, bool debug_mode)
{
    char *size_str = itoa(ent_ptr->len);
    char *name = ent_ptr->buf;
    call2(SYS_WRITE, STDOUT, "size is: ", 9, debug_mode, "Writing to stdout error");
    call2(SYS_WRITE, STDOUT, size_str, strlen(size_str), debug_mode, "Writing to stdout error");
    call2(SYS_WRITE, STDOUT, " name is: ", 10, debug_mode, "Writing to stdout error");
    call2(SYS_WRITE, STDOUT, name, strlen(name), debug_mode, "Writing to stdout error");

}

void print_ent_type (int type_num,bool debug_mode) {
    call2(SYS_WRITE, STDOUT, " type is: ", 10, debug_mode, "Writing to stdout error");
    switch (type_num)
    {
    case DT_FIFO:
        call2(SYS_WRITE, STDOUT, "fifo", 4 , debug_mode, "Writing to stdout error");
        break;
    case DT_BLK:
        call2(SYS_WRITE, STDOUT, "blk",3, debug_mode, "Writing to stdout error");
        break;
    case DT_CHR:
        call2(SYS_WRITE, STDOUT, "chr",3, debug_mode, "Writing to stdout error");
        break;
    case DT_DIR:
        call2(SYS_WRITE, STDOUT, "dir",3, debug_mode, "Writing to stdout error");
        break;
    case DT_LNK:
        call2(SYS_WRITE, STDOUT, "lnk", 3, debug_mode, "Writing to stdout error");
        break;
    case DT_REG:
        call2(SYS_WRITE, STDOUT, "reg", 3, debug_mode, "Writing to stdout error");
        break;
    case DT_SOCK:
        call2(SYS_WRITE, STDOUT, "sock", 4 , debug_mode, "Writing to stdout error");
        break;
    default:
        call2(SYS_WRITE, STDOUT, "unknown", 7, debug_mode, "Writing to stdout error");
        break;
    }
    call2(SYS_WRITE, STDOUT, "\n", 1, debug_mode, "Writing to stdout error");
}

int main(int argc, char *argv[], char *envp[])
{
    system_call(SYS_WRITE,STDOUT,"Flame2 - This Time it's Personal\n",33);
    bool debug_mode = false;
    char* prefix="";
    int i;
    for (i = 0; i < argc; i++)
    {
        if (strcmp("-D", argv[i]) == 0)
        {
            debug_mode = true;
        }
        if(strncmp(argv[i],"-p",2) == 0 ) {
            prefix = argv[i]+PROGRAM_ARG_OFFSET;
        }
    }
    char buffer[MAX_BYTES_IN_DIRECTORY];
    ent *ent_ptr;
    int wd_file_descriptor = call1(SYS_OPEN, ".", O_RDONLY, FILE_PERMISSION, debug_mode, "Open Current Directory Error");
    ent_ptr = (ent*) buffer;
    int amount_of_bytes_read = call2(SYS_GETDENTS, wd_file_descriptor, buffer, MAX_BYTES_IN_DIRECTORY, debug_mode, "Get Dents Error");
    int counter_bytes;
    int type_of_file;
    for(counter_bytes = 0 ; counter_bytes < amount_of_bytes_read; counter_bytes+=ent_ptr->len) {
        type_of_file = (int)(*(buffer+counter_bytes+ent_ptr->len -1));
        ent_ptr = (ent*) (buffer+counter_bytes);  
        if(strncmp(ent_ptr->buf,prefix,strlen(prefix)) == 0) {
            print_ent(ent_ptr, debug_mode);
            print_ent_type(type_of_file,debug_mode);
        }

    }

    return 0;
}