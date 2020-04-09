all: hexaPrint

hexaPrint: hexaPrint.o
	gcc -g -Wall -o hexaPrint hexaPrint.o 

hexaPrint.o: hexaPrint.c
	gcc -g -Wall -c -o hexaPrint.o hexaPrint.c 

.PHONY: clean

clean: 
	rm -f *.o hexaPrint
