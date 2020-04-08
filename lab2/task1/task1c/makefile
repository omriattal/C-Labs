all: arrcheck

arrcheck: arrcheck.o
	gcc -g -m32 -Wall -o  arrcheck arrcheck.o 

arrcheck.o: arrcheck.c
	gcc -g -m32 -Wall -c -o arrcheck.o arrcheck.c 

.PHONY: clean

clean: 
	rm -f *.o arrcheck
