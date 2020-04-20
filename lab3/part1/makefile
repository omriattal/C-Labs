all: bubblesort

bubblesort: bubblesort.o
	gcc -g -Wall -o bubblesort bubblesort.o 

bubblesort.o: bubblesort.c
	gcc -g -Wall -c -o bubblesort.o bubblesort.c 

.PHONY: clean

clean: 
	rm -f *.o bubblesort
