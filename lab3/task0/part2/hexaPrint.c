#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void printHex(char *buffer, int length)
{
    for (int i = 0; i < length; i++)
    {
        printf("%02X ", buffer[i] & 0x0ff);
    }
}

int main(int argc, char *argv[])
{
    FILE *inputFile = fopen(argv[1], "r");
    int size = 0;
    char *buffer = (char *)malloc(sizeof(char) * size);

    fseek(inputFile, 0, SEEK_END);
    size = ftell(inputFile);
    fseek(inputFile, 0, SEEK_SET);

    fread(buffer, 1, size, inputFile);
    printHex(buffer, size);

    free(buffer);
    fclose(inputFile);

    return 0;
}