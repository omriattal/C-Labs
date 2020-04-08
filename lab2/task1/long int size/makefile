all: check

check: check.o
	gcc -g -m32 -Wall -o  check check.o 

check.o: check.c
	gcc -g -m32 -Wall -c -o check.o check.c 

.PHONY: clean

clean: 
	rm -f *.o check
