CC=gcc
FLAGS= -g -Wall -std=c99
LINKER_FLAGS= -g -Wall 
LIBS=  $(ncursesw5-config --cflags --libs) -lncurses

OBJECTS= tetris.o

tetris: tetris.o
	$(CC) $(LINKER_FLAGS) $^  $(LIBS) -o $@

tetris.o: tetris.c tetris.h
	$(CC) $(FLAGS) tetris.c -c

.PHONY: clean

clean:
	-rm $(OBJECTS) tetris

#gcc -o out $(ncursesw5-config --cflags --libs) input.c	
