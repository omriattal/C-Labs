#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define a_ASCII_NUMBER 97
#define z_ASCII_NUMBER 122
#define newline_ASCII_NUMBER 10

typedef enum Mode {
    REGULAR, ENC_PLUS, ENC_MINUS
} MODE;

bool isLowerCaseLetter(char c);

bool endOfInstructions(char c);

int incrementInstructionIndex(int index, char *instructions);


int main(int argc, char *argv[]) {
    MODE mode = REGULAR;
    bool debugFlag = false;
    char inputChar;
    char *instructions = NULL;
    int index = 0;
    FILE *outputfile = stdout;

    for (int i = 1; argv[i] != NULL && i <= 4; i++) {
        if (strcmp(argv[i], "-D") == 0) {
            debugFlag = true;;
        }
        if (strncmp(argv[i], "-o", 2) == 0) {
            outputfile = fopen(argv[i] + 2, "w");
        }
        if (strncmp(argv[i], "+e", 2) == 0 && strlen(argv[i]) > 2) {
            mode = ENC_PLUS;
            instructions = argv[i] + 2;
        } else if (strncmp(argv[i], "-e", 2) == 0 && strlen(argv[i]) > 2) {
            mode = ENC_MINUS;
            instructions = argv[i] + 2;
        }
    }
    if (debugFlag) {
        printf("-D \n");
    }

    do {
        inputChar = fgetc(stdin);
        if (feof(stdin)) {
            fclose(outputfile);
            break;
        }
        if (inputChar != newline_ASCII_NUMBER) {
            if (debugFlag) {
                fprintf(stderr, "%d \t", inputChar);
            }
            if (mode == REGULAR && isLowerCaseLetter(inputChar)) {
                inputChar = inputChar - 32;
            } else if (mode == ENC_MINUS) {
                char numToAdd = instructions[index] - '0';
                inputChar = inputChar - numToAdd;
                index = incrementInstructionIndex(index, instructions);
            } else if (mode == ENC_PLUS) {
                char numToAdd = instructions[index] - '0';
                inputChar = inputChar + numToAdd;
                index = incrementInstructionIndex(index, instructions);
            }

            if (debugFlag) {
                fprintf(stderr, "%d \n", inputChar);
            }
            fputc(inputChar, outputfile);
        } else {
            if (debugFlag) {
                fputc('\n', stderr);
            }

            fputc('\n', outputfile);
            index = 0;
        }
    } while (true);
}


int incrementInstructionIndex(int index, char *instructions) {
    index++;
    if (endOfInstructions(instructions[index])) {
        index = 0;
    }
    return index;
}

bool isLowerCaseLetter(char c) {
    return c >= a_ASCII_NUMBER && c <= z_ASCII_NUMBER;
}

bool endOfInstructions(char c) {
    return (c == newline_ASCII_NUMBER) || (c == '\0');
}