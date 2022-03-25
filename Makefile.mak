PGMS=project2

CC=gcc
CFLAGS=-Wall -pthread -g

all: $(PGMS)

clean:
	rm -f $(PGMS)
	rm -f *.o

tarball: 
	tar -czvf $(PGMS).tar.gz project2.c
