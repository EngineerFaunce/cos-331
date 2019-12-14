# Usage:
# make			# compiles source files
# make clean	# removes compiled files

CFLAGS = -std=c99 -g

all: source

source: main.o Opcodes.o New.o
	$(CC) $(CFLAGS) -o MyProj Opcodes.o main.o New.o -lm

main.o: main.c Vars.h
	$(CC) $(CFLAGS) -c main.c -o main.o

Opcodes.o: Opcodes.c Vars.h
	$(CC) $(CFLAGS) -c Opcodes.c -o Opcodes.o

New.o: New.c Vars.h
	$(CC) $(CFLAGS) -c New.c -o New.o

clean:
	@echo "Cleaning up..."
	rm -f *.o MyProj