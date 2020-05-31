#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include <limits.h>

#define INPUT_MAX_SIZE 1024
#define FILE_NAME_INPUT_MAX_SIZE 100
#define MEM_BUF_SIZE 10000

typedef struct
{
    char debug_mode;
    char file_name[128];
    int unit_size;
    unsigned char mem_buf[MEM_BUF_SIZE];
    size_t mem_count;
    bool display_mode; // 0 -> decimal, 1 -> hexadecimal

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
void debug_print(bool debug_mode, char *str1, char *str2)
{
    if (debug_mode)
    {
        fprintf(stderr, "%s %s", str1, str2);
    }
}
void debug_print_int(bool debug_mode, char *str1, int num)
{
    if (debug_mode)
    {
        fprintf(stderr, "%s %d", str1, num);
    }
}
void debug_print_hexa(bool debug_mode, char *str1, int num)
{
    if (debug_mode)
    {
        fprintf(stderr, "%s %X", str1, num);
    }
}
void init_state(STATE *state);
char *format_from_state(STATE *state)
{
    if (state->display_mode)
    {
        return "%X";
    }
    return "%d";
}
int sizeof_file(char *filename)
{
    FILE *file = fopen(filename, "r");
    int size = 0;
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);
    fclose(file);
    return size;
}
// MENU FUNCTIONS
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
void print_state(STATE *state)
{
    printf("%s", "\nTHE STATE: \n");
    if (state->debug_mode)
    {
        printf("%s", "The debug mode is: ON \n");
    }
    else
    {
        printf("%s", "The debug mode is: OFF \n");
    }
    if (state->display_mode)
    {
        printf("%s", "The display mode is: HEXADECIMAL \n");
    }
    else
    {
        printf("%s", "The display mode is: DECIMAL \n");
    }
    printf("The file name is: %s \n", state->file_name);
    printf("The unit size is: %d \n", state->unit_size);
    printf("The mem count is: %d \n", state->mem_count);
    fprintf(stdout, "%s %p", "The membuf location:\n", state->mem_buf);
}
void set_file_name(STATE *state)
{
    printf("\nenter file name: ");
    char file_name_tmp[FILE_NAME_INPUT_MAX_SIZE];
    char file_name[FILE_NAME_INPUT_MAX_SIZE];
    fgets(file_name_tmp, FILE_NAME_INPUT_MAX_SIZE, stdin);
    sscanf(file_name_tmp, "%s", file_name);
    strcpy(state->file_name, file_name);
    debug_print(state->debug_mode, "Debug: file name set to: ", file_name);
}
void set_unit_size(STATE *state)
{
    printf("\nEnter unit size: ");
    char size_tmp[INPUT_MAX_SIZE];
    int size = 0;
    fgets(size_tmp, INPUT_MAX_SIZE, stdin);
    sscanf(size_tmp, "%d", &size);
    if (valid_unit_size(size))
    {
        state->unit_size = size;
        debug_print_int(state->debug_mode, "Debug: set size to: ", size);
    }
    else
    {
        fprintf(stdout, "%s", "Invalid unit size!");
    }
}
void load_into_memory(STATE *state)
{
    if (state->file_name == NULL)
    {
        printf("Error: file name is null\n");
        return;
    }
    FILE *file = fopen(state->file_name, "r");
    if (file == NULL)
    {
        printf("Error opening the file\n");
        return;
    }
    int location;
    int length;
    char input_tmp[INPUT_MAX_SIZE];
    printf("\nEnter location: ");
    fgets(input_tmp, INPUT_MAX_SIZE, stdin);
    sscanf(input_tmp, "%X", &location);
    printf("Enter length: ");
    fgets(input_tmp, INPUT_MAX_SIZE, stdin);
    sscanf(input_tmp, "%d", &length);

    debug_print(state->debug_mode, "\nDebug: The filename:", state->file_name);
    debug_print_int(state->debug_mode, "\nDebug: The length is:", length);
    debug_print_hexa(state->debug_mode, "\nDebug: The location is: ", location);
    fseek(file, location, SEEK_SET);
    fread(state->mem_buf, state->unit_size, length, file);
    printf("\n--------Loaded %d units in to memory--------\n", length);

    state->mem_count = state->unit_size * length;
    fclose(file);
}
void toggle_display_mode(STATE *state)
{
    if (state->display_mode)
    {
        printf("Display flag now off, DECIMAL representation \n");
        state->display_mode = false;
    }
    else
    {
        printf("Display flag now on, HEXADECIMAL representation\n");
        state->display_mode = true;
    }
}
void memory_display(STATE *state)
{
    char input_tmp[INPUT_MAX_SIZE];
    int u;
    int addr;
    unsigned char *buffer = state->mem_buf;
    printf("\nEnter amount of units: ");
    fgets(input_tmp, INPUT_MAX_SIZE, stdin);
    sscanf(input_tmp, "%d", &u);
    printf("Enter address: ");
    fgets(input_tmp, INPUT_MAX_SIZE, stdin);
    sscanf(input_tmp, "%X", &addr);
    if (state->display_mode)
    {
        printf("%s", "------ PRINTING MEMORY WITH HEXADECIMAL REPRESENTATION ------\n");
    }
    else
    {
        printf("%s", "-------- PRINTING MEMORY WITH DECIMAL REPRESENTATION --------\n");
    }
    if (addr != 0)
    {
        buffer = (unsigned char *)addr;
    }
    unsigned char *end = buffer + state->unit_size * u;
    while (buffer < end)
    {
        //TODO: CHECK GENERICS
        if (state->unit_size == 1)
        {
            unsigned char var = *((unsigned char *)(buffer));
            fprintf(stdout, format_from_state(state), var);
            printf("%s", "\n");
            buffer += state->unit_size;
        }
        else if (state->unit_size == 2)
        {
            unsigned short var = *((unsigned short *)(buffer));
            fprintf(stdout, format_from_state(state),var);
            printf("%s", "\n");
            buffer += state->unit_size;
        }
        else
        {
            unsigned int var = *((unsigned int *)(buffer));
            fprintf(stdout, format_from_state(state), var);
            printf("%s", "\n");
            buffer += state->unit_size;
        }
    }
}
void save_into_file(STATE *state)
{
    int source_addr, target_loc, length;
    char input_tmp[INPUT_MAX_SIZE];
    printf("\nEnter source address: ");
    fgets(input_tmp, INPUT_MAX_SIZE, stdin);
    sscanf(input_tmp, "%X", &source_addr);
    printf("Enter target location: ");
    fgets(input_tmp, INPUT_MAX_SIZE, stdin);
    sscanf(input_tmp, "%X", &target_loc);
    printf("Enter length: ");
    fgets(input_tmp, INPUT_MAX_SIZE, stdin);
    sscanf(input_tmp, "%d", &length);

    if (target_loc >= sizeof_file(state->file_name))
    {
        printf("%s", "Error: target location is greater than the file's size\n");
        return;
    }
    unsigned char *buffer = (unsigned char *)state->mem_buf;
    if (source_addr != 0)
    {
        buffer = (unsigned char *)source_addr;
    }
    FILE *file = fopen(state->file_name, "r+");
    fseek(file, target_loc, SEEK_SET);
    fwrite(buffer, state->unit_size, length, file);
    fclose(file);
}
void modify_memory(STATE *state)
{
    int location, value;
    char input_tmp[INPUT_MAX_SIZE];
    printf("Enter location: ");
    fgets(input_tmp, INPUT_MAX_SIZE, stdin);
    sscanf(input_tmp, "%X", &location);
    printf("Enter value: ");
    fgets(input_tmp, INPUT_MAX_SIZE, stdin);
    sscanf(input_tmp, "%X", &value);
    debug_print_hexa(state->debug_mode, "\nDebug: the location is: ", location);
    debug_print_hexa(state->debug_mode, "\nDebug: the value is: ", value);
    if (location > state->mem_count)
    { 
        printf("%s", "Error: not enough space in membuf!\n");
        return;
    }
    else if ((state->unit_size == 1 && value > UCHAR_MAX)
             || (state->unit_size == 2 && value > USHRT_MAX)
             || (state->unit_size == 4 && value > UINT32_MAX))
    {
        printf("%s","Error: incompatible value!\n");
        return;
    }

    memmove(state->mem_buf+location,&value,state->unit_size);
}
void quit(STATE *state)
{
    free(state);
    exit(0);
}

int main(int argc, char **argv)
{
    STATE *state = (STATE *)(malloc(sizeof(STATE)));
    init_state(state);
    int parsed_input = 0;
    char strInput[INPUT_MAX_SIZE];
    char strinputTmp[INPUT_MAX_SIZE];
    FUNC_DESC menu[] = {{"Toggle debug mode", &toggle_debug_mode},
                        {"Print state info", &print_state},
                        {"Set file name", &set_file_name},
                        {"Set unit size", &set_unit_size},
                        {"Load into memory", &load_into_memory},
                        {"Toggle display mode", &toggle_display_mode},
                        {"Memory display", &memory_display},
                        {"Save into file", &save_into_file},
                        {"Modify memory", &modify_memory},
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
    state->unit_size = 1;
    state->mem_count = 0;
    state->display_mode = false;
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