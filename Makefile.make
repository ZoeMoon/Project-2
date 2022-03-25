PGMS=project2

CC=gcc
CFLAGS=-Wall -pthread 

all: $(PGMS)

clean:
	rm -f $(PGMS)
	rm -f *.o
