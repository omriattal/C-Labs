all: flame2

flame2: flame2.o util.o start.o
	ld -m elf_i386  flame2.o util.o start.o -o flame2 

flame2.o: flame2.c
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector flame2.c -o flame2.o 

util.o: util.c
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector util.c -o util.o 	

start.o: start.s
	nasm -f elf start.s -o start.o
	nasm -f elf start.s -l sample.lst

.PHONY: clean

clean: 
	rm -f *.o flame2

