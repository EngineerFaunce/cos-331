# Usage:
# make			# compiles source files
# make clean	# removes compiled files

CFLAGS = -std=c99 -g
  
all: source

source: main.o Opcodes.o Functions.o
	$(CC) $(CFLAGS) -o Project2 Opcodes.o Functions.o main.o

main.o: main.c Vars.h
	$(CC) $(CFLAGS) -c main.c -o main.o

Opcodes.o: Opcodes.c Vars.h
	$(CC) $(CFLAGS) -c Opcodes.c -o Opcodes.o

Functions.o: Functions.c Vars.h
	$(CC) $(CFLAGS) -c Functions.c -o Functions.o

clean:
	@echo "Cleaning up..."
	rm -f *.o Project2