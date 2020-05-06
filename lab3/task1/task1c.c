#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define VIRUS_NAME_LENGTH 16
#define ZERO_PADDING_NUMBER_HEX 256
#define INPUT_MAX_SIZE 50
#define MAX_BUFFER_SIZE 10000


typedef struct virus
{
    unsigned short SigSize;
    char virusName[VIRUS_NAME_LENGTH];
    char *sig;
} virus;

typedef struct link link;

typedef struct link
{
    link *nextVirus;
    virus *vir;
} link;


typedef struct menu
{
    char *name;
    link *(*func)(char*, link *);
} MENU;



link *list_append(link *virus_list, virus *data);

void list_free(link *virus_list);

void list_print(link *virus_list);

void destroyVirus(virus *virus);

link *constructLink(link *nextlink, virus *data);

link *destroyLink(link *list);

void printHex(FILE *file, char *buffer, int length);

virus *readVirus(FILE *file);

void printVirus(virus *virus, FILE *output);

link *loadSignatures(char *fileName, link *list);

link *printSignatures(char *fileName, link *list);

link *detect_virus_wrapper(char *fileName, link *list);

void detect_virus(char *buffer, unsigned int size, link *virus_list);

link *quit(char* fileName, link *list);


//MENU FUNCTIONS

link *quit(char* fileName, link *list)
{
    list_free(list);
    exit(0);
    return list;
}

link *detect_virus_wrapper(char *fileName, link *list)
{
    char buffer[MAX_BUFFER_SIZE] = "\0";
    unsigned int size = 0;
    FILE* suspected = fopen(fileName,"r");
    fseek(suspected, 0, SEEK_END);
    size = ftell(suspected);
    fseek(suspected, 0, SEEK_SET);
    if (size > MAX_BUFFER_SIZE)
    {
        size = MAX_BUFFER_SIZE;
    }
    fread(buffer, 1, size, suspected);
    detect_virus(buffer, size, list);
    fclose(suspected);
    return list;
}
//LINK FUNCTIONS

link *destroyLink(link *list)
{
    destroyVirus(list->vir);
    link *tmp = list;
    list = list->nextVirus;
    free(tmp);
    return list;
}

link *constructLink(link *nextlink, virus *data)
{
    link *newlink = (link *)malloc(sizeof(link));
    newlink->nextVirus = nextlink;
    newlink->vir = data;
    return newlink;
}

void list_print(link *virus_list)
{
    while (virus_list != NULL)
    {
        printVirus(virus_list->vir, stdout);
        virus_list = virus_list->nextVirus;
    }
}

link *list_append(link *virus_list, virus *data)
{
    link *newlink = constructLink(virus_list, data);
    return newlink;
}

void list_free(link *virus_list)
{
    while (virus_list != NULL)
    {
        link *tmp = virus_list;
        destroyVirus(virus_list->vir);
        virus_list = virus_list->nextVirus;
        free(tmp);
    }
}

link *loadSignatures(char *fileName, link *list)
{
    char inputTmp[INPUT_MAX_SIZE];
    char input[INPUT_MAX_SIZE];
    fgets(inputTmp,INPUT_MAX_SIZE,stdin);
    sscanf(inputTmp,"%s",input);
    printf("%s", "\n");
    FILE *file = fopen(input, "r");
    while (!feof(file))
    {
        virus* vir = readVirus(file);
        list = list_append(list, vir);
    }

    list = destroyLink(list); //junk
    fclose(file);
    return list;
}

link *printSignatures(char *fileName, link *list)
{
    list_print(list);
    return list;
}



void detect_virus(char *buffer, unsigned int size, link *virus_list)
{
    link *list = virus_list;
    while (list != NULL)
    {
        int signatureSize = list->vir->SigSize;
        char *signature = list->vir->sig;
        unsigned int limit = size - signatureSize;
        for (int i = 0; i <= limit; i++)
        { if (memcmp((buffer + i), signature, signatureSize) == 0)
            {
                printf("\n---------- VIRUS DETECTED -----------n");
                printf("The starting byte location is %d \n", i);
                printf("The virus name:%s \n", list->vir->virusName);
                printf("The size of the virus signature: %d", signatureSize);
                printf("\n ----------------------------------------\n\n");
            }
        }
        list = list->nextVirus;
    }
}
//VIRUS FUNCTIONS

void printHex(FILE *file, char *buffer, int length)
{
    for (int i = 0; i < length; i++)
    {
        printf("%02X \t", buffer[i] & 0x0ff);
    }
}

void destroyVirus(virus *virus)
{
    free(virus->sig);
    free(virus);
}

virus *readVirus(FILE *file)
{
    virus *nextVirus = (virus *)malloc(sizeof(virus));
    char nInputRaw[2] = "\0";
    fread(nInputRaw, 1, 2, file);
    unsigned short n = *(unsigned short *) nInputRaw;
    char *sig = (char *)malloc(sizeof(char) * n);
    fread(nextVirus->virusName, 1, VIRUS_NAME_LENGTH, file);

    fread(sig, 1, n, file);

    nextVirus->SigSize = n;
    nextVirus->sig = sig;

    return nextVirus;
}
void printVirus(virus *virus, FILE *output)
{
    fprintf(output, "The name of the virus is: %s \n", virus->virusName);
    fprintf(output, "The virus length is: %d \n", virus->SigSize);
    fprintf(output, "%s", "The virus signature: \n");
    printHex(output, virus->sig, virus->SigSize);
    fprintf(output, "%s", "\n");
}
int main(int argc, char *argv[])
{
    char *fileName = argv[1];
    char inputBuffer[INPUT_MAX_SIZE];
    MENU menu[] = {
        {"Load Signatures", &loadSignatures},
        {"Print Signatures", &printSignatures},
        {"Detect Virus", &detect_virus_wrapper},
        {"quit", &quit},
        {NULL, NULL}};
    int upperBound = 3;     
    int input = 0;
    link *list = NULL;
     while (true)
    {
        printf("%s", "Please choose a function \n");
        for (int i = 0; menu[i].name != NULL; i++)
        {
            printf("%d) %s \n", i, menu[i].name);
        }
        printf("Option: ");
        fgets(inputBuffer, INPUT_MAX_SIZE, stdin);
        sscanf(inputBuffer, "%d", &input);
        if (0 <= input && input <= upperBound)
        {
            list = menu[input].func(fileName, list);
        }
    }
}