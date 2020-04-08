all: lab1

lab1: main.o numbers.o add.o
	gcc -m32 -g -Wall -o lab1 main.o numbers.o add.o

main.o: main.c
	gcc -g -Wall -m32 -c -o main.o main.c 

numbers.o: numbers.c
	gcc -g -Wall -m32 -c -o numbers.o numbers.c 	

add.o: add.s
	nasm -g -f elf -w+all -o add.o add.s

.PHONY: clean

clean: 
	rm -f *.o lab1
