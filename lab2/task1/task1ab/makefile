all: addresses

addresses: addresses.o
	gcc -g -m32 -Wall -o  addresses addresses.o 

addresses.o: addresses.c
	gcc -g -m32 -Wall -c -o addresses.o addresses.c 

.PHONY: clean

clean: 
	rm -f *.o addresses
