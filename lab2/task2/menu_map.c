#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define LOWEST_CHAR 0x20
#define HIGHEST_CHAR 0x7E


char censor(char c);
char* map(char *array, int array_length, char (*f) (char));
bool rangeCheck (char c);
char encrypt(char c);
char decrypt(char c);
char dprt(char c);
char cprt(char c);
char my_get(char c);
char quit(char c);


int main(int argc, char **argv){


}

char censor(char c) {
    if(c == '!')
        return '.';
    else
        return c;
}

char* map(char *array, int array_length, char (*f) (char)){
    char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
    for(int i = 0 ; i< array_length; i++) {
        mapped_array[i] = (*f)(array[i]);
    }
    return mapped_array;
}

char encrypt(char c) {
    if(rangeCheck(c)) {
        c=c+3;
    }
    return c;
}
char decrypt(char c) {
    if(rangeCheck(c)) {
        c=c-3;
    }
    return c;
}
char dprt(char c) {
    printf("%d \n",c);
    return c;
}
char cprt(char c) {
    if(rangeCheck(c)) {
        printf("%c \n",c);
    } else {
        printf(". \n");
    }
    return c;
}

char my_get(char c) {
    return fgetc(stdin);
}

bool rangeCheck(char c) {
    return LOWEST_CHAR<=c && c<=HIGHEST_CHAR;
}

char quit(char c) {
    if(c == 'q') {
        exit(0);
    }
    return 0;
}

