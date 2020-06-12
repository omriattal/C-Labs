all: virus

virus: virus.o
	ld -m elf_i386  virus.o -o virus

virus.o: virus.s
	nasm -f elf virus.s -o virus.o
	nasm -f elf virus.s -l sample.lst

.PHONY: clean

clean: 
	rm -f *.o virus

