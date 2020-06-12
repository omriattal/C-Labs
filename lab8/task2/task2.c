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

#define INPUT_MAX_SIZE 1024
#define FILE_NAME_INPUT_MAX_SIZE 100

typedef struct
{
    char debug_mode;
    int currentfd;
    Elf32_Ehdr *header;
    struct stat fd_stat;
} STATE;

typedef struct func_desc
{
    char *name;
    void (*func)(STATE *);
} FUNC_DESC;

//AUXILLARY
bool withinBounds(int bounds, int c);
void print_menu(FUNC_DESC menu[]);
bool valid_unit_size(int size);
int calculate_menu_bounds(FUNC_DESC menu[]);
void init_state(STATE *state);
void close_file(STATE *state)
{
    if (state->currentfd != -1)
    {
        close(state->currentfd);
    }
}
void unmap(STATE *state)
{
    if (state->header != NULL)
    {
        munmap(state->header, state->fd_stat.st_size);
    }
}
bool isElf(void *map_start)
{
    char *map_start_char = (char *)(map_start);
    return (*map_start_char) == 0x7F && strncmp(map_start_char + 1, "ELF", 3) == 0;
}
void print_magic(Elf32_Ehdr *elf_header)
{
    printf("%s", "\nMagic: ");
    for (size_t i = 1; i <= 3; i++)
    {
        printf("%0X ", elf_header->e_ident[i]);
    }
}
void print_entry(Elf32_Ehdr *elf_header)
{
    printf("%s %0X", "\nEntry Point: ", elf_header->e_entry);
}
void print_data_scheme(Elf32_Ehdr *elf_header)
{
    printf("%s", "\nData scheme: ");
    if (elf_header->e_ident[EI_DATA] == 1)
    {
        printf("two's complement, little endian");
    }
    else
    {
        printf("two's complement, big endian");
    }
}
void print_shdr_offset(Elf32_Ehdr *elf_header)
{
    printf("%s %d ", "\nSection header table offset: ", elf_header->e_shoff);
}
void print_shdr_num(Elf32_Ehdr *elf_header)
{
    printf("%s %d ", "\nNumber of section headers: ", elf_header->e_shnum);
}
void print_size_of_each_shdr_entry(Elf32_Ehdr *elf_header)
{
    printf("%s %d\n", "\nSection header entry size: ", elf_header->e_shentsize);
}
void print_phdr_offset(Elf32_Ehdr *elf_header)
{
    printf("%s %d ", "\nProgram header table offset: ", elf_header->e_phoff);
}
void print_phdr_num(Elf32_Ehdr *elf_header)
{
    printf("%s %d ", "\nNumber of program headers: ", elf_header->e_phnum);
}
void print_size_of_each_phdr_entry(Elf32_Ehdr *elf_header)
{
    printf("%s %d", "\nProgram header entry sizes: ", elf_header->e_phentsize);
}
void print_elf_info(Elf32_Ehdr *elf_header)
{
    printf("-------------    PRINTING ELF    -------------\n");
    print_magic(elf_header);
    print_data_scheme(elf_header);
    print_entry(elf_header);
    print_shdr_offset(elf_header);
    print_shdr_num(elf_header);
    print_size_of_each_shdr_entry(elf_header);
    print_phdr_offset(elf_header);
    print_phdr_num(elf_header);
    print_size_of_each_phdr_entry(elf_header);
    printf("\n\n---------------------------------------------");
}
char *get_type_name(int type)
{
    switch (type)
    {
    case SHT_PROGBITS:
        return "PROGBITS";
    case SHT_SYMTAB:
        return "SYMTAB";
    case SHT_STRTAB:
        return "STRTAB";
    case SHT_RELA:
        return "RELA";
    case SHT_HASH:
        return "HASH";
    case SHT_DYNAMIC:
        return "DYNAMIC";
    case SHT_NOTE:
        return "NOTE";
    case SHT_NOBITS:
        return "NOBITS";
    case SHT_REL:
        return "REL";
    case SHT_SHLIB:
        return "SHLIB";
    case SHT_INIT_ARRAY:
        return "INIT_ARRAY";
    case SHT_FINI_ARRAY:
        return "FINI_ARRAY";
    case SHT_PREINIT_ARRAY:
        return "PREINIT_ARRAY";
    case SHT_GROUP:
        return "GROUP";
    case SHT_SYMTAB_SHNDX:
        return "SYMTAB_SHNDX";
    case SHT_NUM:
        return "NUM";
    case SHT_LOOS:
        return "LOOS";
    default:
        return "NULL";
    }
}
char *get_special_sh_name(int index){
    switch(index){
        case SHN_LORESERVE:
            return "LORESERVE/LOPROC";
        case SHN_HIPROC:
            return "HIPOC";
        case SHN_ABS:
            return "ABS";
        case SHN_COMMON:
            return "COMMON";
        case SHN_HIRESERVE:
            return "HIRESERVE";
        case SHN_UNDEF:
            return "UNDEF";
        default:
            return NULL;
    }
}
char *get_sh_name_from_offset(Elf32_Ehdr *elf_header, int offset_str)
{
    void *begin_file = (void *)elf_header;
    Elf32_Shdr *strtab_entry = (Elf32_Shdr *)(begin_file + elf_header->e_shoff + elf_header->e_shentsize * elf_header->e_shstrndx);
    int offset_strtab = strtab_entry->sh_offset;
    return (char *)(begin_file + offset_strtab + offset_str);
}
char *get_sh_name_from_index(Elf32_Ehdr *elf_header, int index) {
    char *special_name = get_special_sh_name(index);
    if(special_name!=NULL) {
        return special_name;
    }
    void *begin_file = (void *)elf_header;
    Elf32_Shdr *sh_entry = (Elf32_Shdr *)(begin_file + elf_header->e_shoff + elf_header->e_shentsize * index);
    char *sh_name = get_sh_name_from_offset(elf_header,sh_entry->sh_name);
    return sh_name;
}
int get_sh_entry_index_from_type(Elf32_Ehdr *elf_header,int type){
    void *elf_begin = (void *)elf_header;
    int shoff = elf_header->e_shoff;
    int shdr_num = elf_header->e_shnum;
    int symtab_index = -1;
    Elf32_Shdr *entry = (Elf32_Shdr *)(elf_begin + shoff);
    for (int i = 0; i < shdr_num; i++)
    {
       if(entry->sh_type==type){
           symtab_index=i;
           break;
       }
        shoff += elf_header->e_shentsize;
        entry = (Elf32_Shdr *)(elf_begin + shoff);
    }
    return symtab_index;
}
int get_sh_entry_index_from_name(Elf32_Ehdr *elf_header,char *sh_name){
    int shdr_num = elf_header->e_shnum;
    int symtab_index = -1;
    for (int i = 0; i < shdr_num; i++)
    {
       if(strcmp(get_sh_name_from_index(elf_header,i),sh_name)==0){
           symtab_index=i;
           break;
       }
    }
    return symtab_index;
}
char *get_symbol_name(Elf32_Ehdr* elf_header, int str_offset){
    void *begin_file = (void *)elf_header;
    int strtab_index = get_sh_entry_index_from_name(elf_header,".strtab");
    Elf32_Shdr *strtab_entry = (Elf32_Shdr *)(begin_file + elf_header->e_shoff + elf_header->e_shentsize * strtab_index);
    int offset_strtab = strtab_entry->sh_offset;
    return (char *)(begin_file + offset_strtab + str_offset);
}

//MENU FUNCTIONS
void toggle_debug_mode(STATE *state)
{
    if (state->debug_mode)
    {
        state->debug_mode = false;
        printf("Debug flag now off\n");
    }
    else
    {
        state->debug_mode = true;
        printf("Debug flag mode now on\n");
    }
}
void examine_elf_file(STATE *state)
{
    printf("\nEnter file name: ");
    char file_name_tmp[FILE_NAME_INPUT_MAX_SIZE];
    char file_name[FILE_NAME_INPUT_MAX_SIZE];
    fgets(file_name_tmp, FILE_NAME_INPUT_MAX_SIZE, stdin);
    sscanf(file_name_tmp, "%s", file_name);
    close_file(state);
    int curr_fd;
    if ((curr_fd = open(file_name, O_RDWR)) < 0)
    {
        fprintf(stderr, "%s", "error in opening file \n");
        return;
    }
    struct stat curr_stat;
    if (fstat(curr_fd, &curr_stat) != 0)
    {
        close(curr_fd);
        fprintf(stderr, "%s", "stat failed \n");
        return;
    }
    void *curr_map_start = mmap(0, curr_stat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, curr_fd, 0);
    if (curr_map_start == MAP_FAILED)
    {
        close(curr_fd);
        fprintf(stderr, "%s", "mmap failed \n");
        return;
    }
    if (!isElf(curr_map_start))
    {
        close(curr_fd);
        munmap(curr_map_start, curr_stat.st_size);
        fprintf(stderr, "%s", "not an ELF file\n");
        return;
    }
    close_file(state);
    unmap(state);
    state->header = (Elf32_Ehdr *)curr_map_start;
    state->fd_stat = curr_stat;
    state->currentfd = curr_fd;
    print_elf_info(state->header);
}
void print_section_name(STATE *state)
{
    if (state->currentfd == -1)
    {
        fprintf(stderr, "%s", "\ninvalid file");
        return;
    }
    Elf32_Ehdr *elf_header = state->header;
    printf("%s", "\nSection headers sizes:\n");
    void *elf_begin = (void *)elf_header;
    int shoff = elf_header->e_shoff;
    int shdr_num = elf_header->e_shnum;
    Elf32_Shdr *entry = (Elf32_Shdr *)(elf_begin + shoff);
    printf("%s", "INDEX\tNAME\t\tADDRESS\t\tOFFSET\tSIZE\tTYPE\n");
    for (int i = 0; i < shdr_num; i++)
    {
        printf("[%d]\t%s\t\t%08X\t%06X\t%06X\t%s \n", i, get_sh_name_from_offset(elf_header, entry->sh_name), entry->sh_addr, entry->sh_offset, entry->sh_size, get_type_name(entry->sh_type));
        shoff += elf_header->e_shentsize;
        entry = (Elf32_Shdr *)(elf_begin + shoff);
    }
}
void print_symbols(STATE *state)
{
    if (state->currentfd == -1)
    {
        fprintf(stderr, "%s", "\ninvalid file");
        return;
    }

    Elf32_Ehdr *elf_header = state->header;
    int symtab_index = get_sh_entry_index_from_type(elf_header,SHT_SYMTAB);
    void *begin_file = (void *)elf_header;
    Elf32_Shdr *symtab_shdr_entry = (Elf32_Shdr *)(begin_file + elf_header->e_shoff + elf_header->e_shentsize * symtab_index);
    int symtab_size = symtab_shdr_entry->sh_size;
    int symtab_offset = symtab_shdr_entry->sh_offset;
    Elf32_Sym *symtab_entry = (Elf32_Sym *)(begin_file+symtab_offset);
    int size_acc = 0;
    int index = 0;
    printf("INDEX\tVALUE\tSECTION_INDEX\tSECTION NAME\tSYMBOL NAME\n");
    while(size_acc < symtab_size){
        char *sh_name = get_sh_name_from_index(elf_header,symtab_entry->st_shndx);
        printf("%d\t%08X\t%d\t%s\t\t%s\n",index,symtab_entry->st_value,symtab_entry->st_shndx,sh_name,get_symbol_name(elf_header,symtab_entry->st_name));
        size_acc+=sizeof(Elf32_Sym);
        symtab_entry=(Elf32_Sym *)(begin_file+symtab_offset + size_acc);
        index++;
    }
}
void quit(STATE *state)
{
    close_file(state);
    unmap(state);
    free(state);
    exit(0);
}
int main(int argc, char *argv[])
{
    STATE *state = (STATE *)(malloc(sizeof(STATE)));
    init_state(state);
    int parsed_input = 0;
    char strInput[INPUT_MAX_SIZE];
    char strinputTmp[INPUT_MAX_SIZE];
    FUNC_DESC menu[] = {{"Toggle Debug Mode", &toggle_debug_mode},
                        {"Examine ELF File", &examine_elf_file},
                        {"Print Section Names", &print_section_name},
                        {"Print Symbol Names", &print_symbols},
                        {"Quit", &quit},
                        {NULL, NULL}};

    int bounds = calculate_menu_bounds(menu);
    while (true)
    {
        printf("%s", "Please choose an action \n");
        print_menu(menu);
        printf("Option: ");
        fgets(strInput, INPUT_MAX_SIZE, stdin);
        sscanf(strInput, "%s", strinputTmp);
        parsed_input = atoi(strinputTmp);
        if (withinBounds(bounds, parsed_input))
        {
            (*menu[parsed_input].func)(state);
        }
        printf("%s", "\n");
    }
}

void init_state(STATE *state)
{
    state->debug_mode = false;
    state->currentfd = -1;
    state->header = NULL;
}
bool withinBounds(int bounds, int c)
{
    if (c < 0 && c > bounds)
    {
        printf("Not within bounds \n");
    }
    return c >= 0 && c <= bounds;
}
void print_menu(FUNC_DESC menu[])
{
    for (int i = 0; menu[i].name != NULL; i++)
    {
        printf("%d) %s \n", i, menu[i].name);
    }
}
bool valid_unit_size(int size)
{
    if (size == 1 || size == 2 || size == 4)
    {
        return true;
    }
    return false;
}
int calculate_menu_bounds(FUNC_DESC menu[])
{
    int bound = 0;
    for (int i = 0; menu[i].name != NULL; i++)
    {
        bound++;
    }
    return bound - 1;
}