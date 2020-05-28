#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

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
void debug_print (bool debug_mode,char* str1, char *str2) {
    if(debug_mode){
        fprintf(stderr,"%s %s",str1,str2);
    }
}
void debug_print_int (bool debug_mode,char* str1, int num) {
    if(debug_mode){
        fprintf(stderr,"%s %d",str1,num);
    }
}
void debug_print_hexa (bool debug_mode,char* str1, int num) {
    if(debug_mode){
        fprintf(stderr,"%s %02X",str1,num);
    }
}
void init_state (STATE* state);


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

    printf("%s","\nTHE STATE: \n");
    if(state->debug_mode) {
        printf("%s","The debug mode is: true \n");
    } else {
        printf("%s","The debug mode is: false \n");
    }
    printf("The file name is: %s \n",state->file_name);
    printf("The unit size is: %d \n",state->unit_size);
    printf("The mem count is: %d \n",state->mem_count);
    if(state->display_mode) {
        printf("%s","The display mode is: on \n");
    } else {
        printf("%s","The display mode is: off \n");
    }
}
void set_file_name(STATE *state)
{
    printf("\nenter file name: ");
    char file_name_tmp[FILE_NAME_INPUT_MAX_SIZE];
    char file_name[FILE_NAME_INPUT_MAX_SIZE];
    fgets(file_name_tmp, FILE_NAME_INPUT_MAX_SIZE, stdin);
    sscanf(file_name_tmp, "%s", file_name);
    strcpy(state->file_name, file_name);
    debug_print(state->debug_mode,"Debug: file name set to: ",file_name);
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
        debug_print_int(state->debug_mode,"Debug: set size to: ",size);
    }
    else
    {
        fprintf(stdout, "%s", "Invalid unit size!");
    }
    printf("%s","\n");
}
void load_into_memory(STATE* state){
    if(state->file_name==NULL){
        printf("Error: file name is null\n");
        return;
    }
    FILE *file = fopen(state->file_name,"r");
    if(file==NULL) {
        printf("Error opening the file\n");
        return;
    }
    int location;
    int length;
    char input_tmp [INPUT_MAX_SIZE];
    printf("Enter location: ");
    fgets(input_tmp,INPUT_MAX_SIZE,stdin);
    sscanf(input_tmp,"%X",&location);
    debug_print_hexa(state->debug_mode,"\nDebug: The location is: ",location);

    printf("\nEnter length: ");
    fgets(input_tmp,INPUT_MAX_SIZE,stdin);
    sscanf(input_tmp,"%d",&length);

    debug_print(state->debug_mode,"\nDebug: The filename:",state->file_name);
    debug_print_int(state->debug_mode,"\nDebug: The length is:",length);
    debug_print_hexa(state->debug_mode,"\nDebug: The location is: ",location);
    fseek(file,location,SEEK_SET);
    fread(state->mem_buf,state->unit_size,length,file);
    printf("\n\nLoaded %d units in to memory\n",length);

    state->mem_count += state->unit_size * length;
    fclose(file);

    
}
void toggle_display_mode(STATE* state) {
    if(state->display_mode) {
        printf("Display flag now off, decimal representation \n");
        state->display_mode=false;
    } else {
        printf("Display flag now on, hexadecimal representatio\n");
        state->display_mode=true;
    }
}
void quit(STATE *state)
{
    free(state);
    exit(0);
}

int main(int argc, char **argv)
{
    STATE *state = (STATE *)(malloc (sizeof(STATE)));
    init_state(state);

    int parsed_input = 0;
    char strInput[INPUT_MAX_SIZE];
    char strinputTmp[INPUT_MAX_SIZE];
    FUNC_DESC menu[] = {{"toggle debug mode", &toggle_debug_mode},
                        {"set file name", &set_file_name},
                        {"set unit size", &set_unit_size},
                        {"load into memory", &load_into_memory},
                        {"toggle display mode",&toggle_display_mode},
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
void init_state (STATE* state) {
    state->debug_mode = false;
    state->unit_size=1;
    state->mem_count=0;
    state->display_mode=false;
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