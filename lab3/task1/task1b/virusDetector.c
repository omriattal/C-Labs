#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define VIRUS_NAME_LENGTH 16
#define ZERO_PADDING_NUMBER_HEX 256
#define INPUT_MAX_SIZE 50
#define MAX_BUFFER_SIZE 10000

char* fileName = "\0";
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
    link *(*func)(link *);
} MENU;

void destroyVirus(virus *virus);
link *createLink(link *nextlink, virus *data);
void list_print(link *virus_list);
link *list_append(link *virus_list, virus *data);
void list_free(link *virus_list);
link *printSignatures(link *list);
link *loadSignatures(link *list);
link *quit(link *list);
void printHex(FILE *file, char *buffer, int length);
virus *readVirus(FILE *file);
void printVirus(virus *virus, FILE *output);
void detect_virus(char *buffer, unsigned int size, link *virus_list);
link* detect_virus_wrapper(link *list);

void destroyVirus(virus *virus)
{
    free(virus->sig);
    free(virus);
}

link *destroyLink(link *list)
{
    destroyVirus(list->vir);
    link *tmp = list;
    list = list->nextVirus;
    free(tmp);
    return list;
}

link *createLink(link *nextlink, virus *data)
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
    link *newlink = createLink(virus_list, data);
    virus_list = newlink;
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

link *printSignatures(link *list)
{
    list_print(list);
    return list;
}

link *quit(link *list)
{
    list_free(list);
    exit(0);
}

void printHex(FILE *file, char *buffer, int length)
{
    for (int i = 0; i < length; i++)
    {
        printf("%02X \t", buffer[i] & 0x0ff);
    }
}
virus *readVirus(FILE *file)
{
    virus *nextVirus = (virus *)malloc(sizeof(virus));
    char N[2];
    fread(N, 1, 2, file);
    int n = N[1] * ZERO_PADDING_NUMBER_HEX + N[0];

    char name[VIRUS_NAME_LENGTH];
    fread(name, 1, VIRUS_NAME_LENGTH, file);

    char *sig = (char *)malloc(sizeof(char) * n);
    fread(sig, 1, n, file);

    strncpy(nextVirus->virusName, name, VIRUS_NAME_LENGTH);
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
link *loadSignatures(link *list)
{
    char input[INPUT_MAX_SIZE];
    scanf("%s", input);
    printf("%s", "\n");
    fgetc(stdin);
    FILE *file = fopen(input, "r");
    while (!feof(file))
    {
        list = list_append(list, readVirus(file));
    }

    list = destroyLink(list);
    fclose(file);
    return list;
}
void detect_virus(char *buffer, unsigned int size, link *virus_list)
{
    link* list = virus_list;
    while (list != NULL)
    {
        int signatureSize = list->vir->SigSize;
        char* signature = list->vir->sig;
        for (int i = 0; i <= size-signatureSize; i++) {
            if(memcmp((buffer+i),signature,signatureSize)==0) {
                printf("The starting byte location is %d \n",i);
                printf("The virus name:%s \n ",list->vir->virusName);
                printf("The size of the virus signature: %d \n",signatureSize);
            }
        }
        list = list->nextVirus;
    }


}
link* detect_virus_wrapper(link* list) {
    char buffer[MAX_BUFFER_SIZE] ="\0";
    unsigned int size = 0;
    FILE *fileToCheck = fopen(fileName, "r");
    fseek(fileToCheck, 0, SEEK_END);
    size = ftell(fileToCheck);
    fseek(fileToCheck, 0, SEEK_SET);
    if (size > MAX_BUFFER_SIZE)
    {
        size = MAX_BUFFER_SIZE;
    }
    fread(buffer, 1, size, fileToCheck);
    detect_virus(buffer,size,list);
    return list;
}

int main(int argc, char *argv[])
{
    fileName = argv[1];

    MENU menu[] = {
        {"Load Signatures", loadSignatures},
         {"Print Signatures", printSignatures},
         {"Detect Virus", detect_virus_wrapper},
          {"quit", quit}, {NULL, NULL}};
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
    
        input = fgetc(stdin) - '0'; fgetc(stdin);
        if (0 <= input && input <= 3)
        {
            printf("the input is %d\n",input);
            list = menu[input].func(list);
        }
       
    }
}
#pragma clang diagnostic pop