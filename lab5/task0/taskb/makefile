all: looper

looper: looper.o
	gcc -g -Wall -o looper looper.o 

looper.o: looper.c
	gcc -g -Wall -c -o looper.o looper.c

clean: 
	rm -f *.o looper

