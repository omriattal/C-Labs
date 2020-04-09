#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct virus
{
    unsigned short SigSize;
    char virusName[16];
    char *sig;
} virus;

void readVirus(FILE *file)
{
    virus *nextVirus = (virus *)malloc(sizeof(virus));
    char *N = (char *)malloc(sizeof(char) * 2);
    fread(N, 1, 2, file);
    printf("%c \n ", N[0]);
    printf("%d \n", atoi(N));
}
virus *printVirus(virus *virus, FILE *output)
{
}

int main(int argc, char *argv[])
{
    FILE *file = fopen(argv[1], "r");
    readVirus(file);

    return 0;
}