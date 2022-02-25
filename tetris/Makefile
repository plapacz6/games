CC=gcc
FLAGS= -g -Wall 
LINKER_FLAGS= -g -Wall 
LIBS= -lncurses 

OBJECTS= tetris.o

tetris: tetris.o
	$(CC) $(LINKER_FLAGS) $^ $(LIBS) -o $@

tetris.o: tetris.c tetris.h
	$(CC) $(FLAGS) tetris.c -c

.PHONY: clean

clean:
	-rm $(OBJECTS) tetris
