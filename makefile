CC=gcc

SRCDIR= ./src/
OBJDIR= ./src/
BINDIR= ./

all: main

clean:
	rm -f  ./bonus.exe

main:   
	$(CC) -o $(BINDIR)P5 $(OBJDIR)P5.c -lgsl -lgslcblas -lm