all: virusDetector

virusDetector: virusDetector.o
	gcc -g -Wall -o virusDetector virusDetector.o 

virusDetector.o: virusDetector.c
	gcc -g -Wall -c -o virusDetector.o virusDetector.c 

.PHONY: clean

clean: 
	rm -f *.o virusDetector
