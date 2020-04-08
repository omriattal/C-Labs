#include <stdio.h>


int main() {
    int iarray[3];
    int farray[3];
    double darray[3];
    char carray[3];

    printf("&iarray: %p \n",iarray);
    printf("&(iarray + 1) : %p \n",iarray + 1);

    printf("&farray: %p \n",farray);
    printf("&(farray +1) : %p",farray + 1 );

    printf("&darray: %p \n",darray);
    printf("&(darray + 1): %p \n",darray + 1);

    printf("&carray: %p \n",carray);
    printf("&(carray + 1): %p \n",carray + 1);



}