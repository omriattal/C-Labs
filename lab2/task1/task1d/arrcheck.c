#include <stdio.h>


int main() {
    int iarray[] = {1,2,3};
    char carray[] = {'a','b','c'};
    int *iarrayPtr = iarray;
    char *carrayPtr = carray;
    void *p;
    int a;

    printf("iarray values: \n ");
    for(int i=0; i<3; i++) {
        printf("%d \t",*(iarrayPtr+i));
    }
    printf("\n carray values: \n ");
    for(int i=0; i<3; i++) {
        printf("%c \t",*(carrayPtr+i));
    }
    printf("\n");

    printf("the value of the uninitialized pointer: %p \n",p);
}