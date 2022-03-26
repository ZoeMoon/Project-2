CC=gcc
CFLAGS=-Wall -pthread -g

all: project2

project2: project2.c
	$(CC)$(CFLAGS) project2.c -o project2

clean:
	rm -f project2
	rm -f *.o

run: 
	./project2 < test.txt
