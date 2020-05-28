#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define INPUT_MAX_SIZE 1024
#define FILE_NAME_INPUT_MAX_SIZE 100

typedef struct
{
    char debug_mode;
    char file_name[128];
    int unit_size;
    unsigned char mem_buf[10000];
    size_t mem_count;

} STATE;

typedef struct func_desc
{
    char *name;
    void (*func)(STATE *);
} FUNC_DESC;

bool withinBounds(int bounds, int c);
void print_menu(FUNC_DESC menu[]);
bool valid_unit_size(int size);
int calculate_menu_bounds(FUNC_DESC menu[]);

// MENU FUNCTIONS !


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
    //PRINT THE STATE
    printf("%s","\nTHE STATE: \n");
    if(state->debug_mode) {
        printf("%s","The debug mode is: true \n");
    } else {
        printf("%s","The debug mode is: false \n");
    }
    printf("The file name is: %s \n",state->file_name);
    printf("The unit size is: %d \n",state->unit_size);
}
void set_file_name(STATE *state)
{
    printf("\nenter file name: ");
    char file_name_tmp[FILE_NAME_INPUT_MAX_SIZE];
    char file_name[FILE_NAME_INPUT_MAX_SIZE];
    fgets(file_name_tmp, FILE_NAME_INPUT_MAX_SIZE, stdin);
    sscanf(file_name_tmp, "%s", file_name);
    strcpy(state->file_name, file_name);
    if (state->debug_mode)
    {
        fprintf(stderr, "%s %s", "Debug: file name set to:", file_name);
    }
    printf("%s","\n");
}
void set_unit_size(STATE *state)
{
    printf("\nenter number: ");
    char size_tmp[INPUT_MAX_SIZE];
    int size = 0;
    fgets(size_tmp, INPUT_MAX_SIZE, stdin);
    sscanf(size_tmp, "%d", &size);
    if (valid_unit_size(size))
    {
        state->unit_size = size;
        if (state->debug_mode)
        {
            fprintf(stderr, "%s %d", "Debug: set size to: ", size);
        }
    }
    else
    {
        fprintf(stdout, "%s", "Invalid unit size!");
    }
    printf("%s","\n");
}

void quit(STATE *state)
{
    free(state);
    exit(0);
}

int main(int argc, char **argv)
{
    STATE *state = (STATE *)(malloc (sizeof(STATE)));
    state->debug_mode = false;

    int parsed_input = 0;
    char strInput[INPUT_MAX_SIZE];
    char strinputTmp[INPUT_MAX_SIZE];
    FUNC_DESC menu[] = {{"toggle debug mode", &toggle_debug_mode},
                        {"set file name", &set_file_name},
                        {"set unit size", &set_unit_size},
                        {"quit", &quit},
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
        printf("%s","\n");
    }
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
    for (int i = 0; menu[i].name != NULL; i++){bound++;}
    return bound-1;
}