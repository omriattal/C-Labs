#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define LOWEST_CHAR 0x20
#define HIGHEST_CHAR 0x7E
#define INITIAL_SIZE 5
#define QUIT_FUNC_INDEX 6
#define NEWLINE_MINUS_CHAR_0_VALUE -38
#define INPUT_MAX_SIZE 50
char censor(char c);

char *map(char *array, int array_length, char (*f)(char));

bool rangeCheck(char c);

char encrypt(char c);

char decrypt(char c);

char dprt(char c);

char cprt(char c);

char my_get(char c);

char quit(char c);

bool withinBounds(int bounds, int c);

typedef struct fun_desc {
    char *name;
    char (*fun)(char);
} FUN_DESC;

int main(int argc, char **argv) {
    char *carray = (char *) malloc (INITIAL_SIZE * sizeof(char)); carray[0] = '\0';
    char *tmp;
    int bounds = 0;
    int input = 0;
    char strInput[INPUT_MAX_SIZE]; char strinputTmp[INPUT_MAX_SIZE];

    FUN_DESC menu[] = {{"Censor",       &censor},
                       {"Encrypt",      &encrypt},
                       {"Decrypt",      &decrypt},
                       {"Print dec",    &dprt},
                       {"Print string", &cprt},
                       {"Get string",   &my_get},
                       {"Quit",         &quit},
                       {NULL, NULL}};

    for (int i = 0; menu[i].name != NULL; i++) {bounds++;} bounds = bounds - 1;
    while (true) {
        printf("%s", "Please choose a function \n");
        for (int i = 0; menu[i].name != NULL; i++) {
            printf("%d) %s \n", i, menu[i].name);
        }
        printf("Option: ");
        fgets(strInput,INPUT_MAX_SIZE,stdin);
        sscanf(strInput,"%s",strinputTmp);
        input = atoi(strinputTmp);
        if(input!= NEWLINE_MINUS_CHAR_0_VALUE) { //ELIMINATES THE NEWLINE ADDED AFTER READING FROM INPUT
            if (withinBounds(bounds, input)) {
                tmp = map(carray, INITIAL_SIZE, menu[input].fun);
                free(carray);
                carray = tmp;
            } else {
                free(carray);
                quit('q');
            }
            printf("DONE. \n \n");
        }
    }
}

char censor(char c) {
    if (c == '!')
        return '.';
    else
        return c;
}

char *map(char *array, int array_length, char (*f)(char)) {
    char *mapped_array = (char *) (malloc(array_length * sizeof(char)));
    for (int i = 0; i < array_length; i++) {
        mapped_array[i] = (*f)(array[i]);
    }
    return mapped_array;
}

char encrypt(char c) {
    if (rangeCheck(c)) {
        c = c + 3;
    }
    return c;
}

char decrypt(char c) {
    if (rangeCheck(c)) {
        c = c - 3;
    }
    return c;
}

char dprt(char c) {
    printf("%d \n", c);
    return c;
}

char cprt(char c) {
    if (rangeCheck(c)) {
        printf("%c \n", c);
    } else {
        printf(". \n");
    }
    return c;
}

char my_get(char c) {
    return fgetc(stdin);
}

bool rangeCheck(char c) {
    return LOWEST_CHAR <= c && c <= HIGHEST_CHAR;
}

char quit(char c) {
    if (c == 'q') {
        exit(0);
    }
    return 0;
}

bool withinBounds(int bounds, int c) {
    if (c >= 0 && c <= bounds) {
        printf("Within bounds \n");
    } else {
        printf("Not within bounds \n");
    }
    return c >= 0 && c <= bounds; //THIS IS ON PURPOSE!
}