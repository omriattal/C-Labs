all: digitcount
digitcount: digitcount.o
	gcc -g -fno-pie -m32 -Wall -o  digitcount digitcount.o 

digitcount.o: digitcount.c
	gcc -g -fno-pie -m32 -Wall -c -o digitcount.o digitcount.c 

.PHONY: clean

clean: 
	rm -f *.o digitcount
