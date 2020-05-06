all: task2b

task2b: task2b.o
	gcc -g -Wall -o task2b task2b.o 

task2b.o: task2b.c
	gcc -g -Wall -c -o task2b.o task2b.c 

.PHONY: clean

clean: 
	rm -f *.o task2b
