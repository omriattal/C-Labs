all: encoder

encoder: encoder.o util.o start.o
	ld -m elf_i386  encoder.o util.o start.o -o encoder 

encoder.o: encoder.c
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector encoder.c -o encoder.o 

util.o: util.c
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector util.c -o util.o 	

start.o: start.s
	nasm -f elf start.s -o start.o

.PHONY: clean

clean: 
	rm -f *.o patch

