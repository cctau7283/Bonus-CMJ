CC=gcc

SRCDIR= ./src/
OBJDIR= ./src/
BINDIR= ./

all: main

clean:
	rm -f  ./p5

main:   
	$(CC) -o $(BINDIR)p5 $(OBJDIR)p5.c -lgsl -lgslcblas -lm