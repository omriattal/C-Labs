all: patch

patch: patch.o util.o start.o
	ld -m elf_i386 start.o patch.o util.o -o patch 

patch.o: patch.c
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector patch.c -o patch.o 

util.o: util.c
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector util.c -o util.o 	

start.o: start.s
	nasm -f elf start.s -o start.o

.PHONY: clean

clean: 
	rm -f *.o patch

