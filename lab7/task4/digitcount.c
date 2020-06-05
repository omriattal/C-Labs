#include <string.h>
#include <stdio.h>
#include <stdbool.h>

int digit_cnt(char *input)
{
    int counter = 0;
    int i = 0;
    if (input != NULL)
    {
        while (*(input+i) != 0 && *(input+i) != 10 && *(input+i) != 32)
        {
            if (input[i] >= 48 && input[i] <= 57)
            {
                counter++;
            }
            i++;
        }
        return counter;
    }
    return -1;
}
int main(int argc, char *argv[])
{
    printf("%d\n", digit_cnt(argv[1]));
}