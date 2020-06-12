#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

bool isElf(void *map_start)
{
    char *map_start_char = (char *)(map_start);
    return (*map_start_char) == 0x7F && strncmp(map_start_char + 1, "ELF", 3) == 0;
}

void print_program_headers(void *map_start){
    Elf32_Ehdr *elf_header = (Elf32_Ehdr *)map_start;
    int phoff = elf_header->e_phoff;
    int phnum = elf_header->e_phnum;
    Elf32_Phdr *ph_entry = (Elf32_Phdr *)(map_start+phoff);
    printf("%s","TYPE\tOFFSET\t\tVIRTUAL\t\tPHYSICAL\tFILESIZE\tMEMSIZE\tFLAG\tALIGN\n");
    for(int i = 0; i<phnum;i++){
        printf("%d\t%08X\t%08X\t%08X\t%05X\t\t%05X\t%d\t%X\n",ph_entry->p_type,ph_entry->p_offset,ph_entry->p_vaddr,ph_entry->p_paddr,ph_entry->p_filesz,ph_entry->p_memsz,ph_entry->p_flags,ph_entry->p_align);
        phoff+=elf_header->e_phentsize;
        ph_entry = (Elf32_Phdr *)(map_start+phoff); 
    }
}

int main(int argc, char *argv[])
{
    char *filename = argv[1];
    int fd = open(filename, O_RDWR);
    struct stat curr_stat;
    fstat(fd, &curr_stat);
    void *map_start = mmap(0, curr_stat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(isElf(map_start)){
        print_program_headers(map_start);
    } else{
        fprintf(stderr,"%s","NOT AN ELF FILE! \n");
    }
    return 0;
}