#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define VIRUS_NAME_LENGTH 16


typedef struct virus
{
    unsigned short SigSize;
    char virusName[16];
    char *sig;
} virus;

void destroyVirus(virus* virus) {
    free(virus->sig);
    free(virus);
}


void printHex(char *buffer, int length)
{
    for (int i = 0; i < length; i++)
    {
        printf("%02X \t", buffer[i] & 0x0ff);
    }
}
virus* readVirus(FILE *file)
{
    virus *nextVirus = (virus *)malloc(sizeof(virus));
    char N[2];
    fread(N, 1, 2, file);
    int n = N[1] * 256 + N[0];

    char name[VIRUS_NAME_LENGTH];
    fread(name,1,VIRUS_NAME_LENGTH,file);



    char* sig = (char *) malloc (sizeof(char)*n);
    fread(sig,1,n,file);
    
    strncpy(nextVirus->virusName,name,VIRUS_NAME_LENGTH);
    nextVirus->SigSize = n;
    nextVirus->sig = sig;

    return nextVirus;
}
void printVirus(virus *virus, FILE *output)
{
    fprintf(output,"The name of the virus is: %s \n",virus->virusName);
    fprintf(output,"The virus length is: %d \n",virus->SigSize);
    fprintf(output,"%s","The virus signature: \n");
    printHex(virus->sig,virus->SigSize); fprintf(output,"%s","\n");


}

int main(int argc, char *argv[])
{
    FILE *file = fopen(argv[1], "r");
    virus* vir = readVirus(file);
    printVirus(vir,stdout);
    destroyVirus(vir);
    return 0;
}