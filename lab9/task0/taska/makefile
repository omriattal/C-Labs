all: myelf
myelf: myelf.o
	gcc -g -m32 -Wall -o  myelf myelf.o 

myelf.o: myelf.c
	gcc -g -m32 -Wall -c -o myelf.o myelf.c 

.PHONY: clean

clean: 
	rm -f *.o myelf
