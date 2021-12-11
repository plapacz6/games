CFLAGS=-Wall -g -DTESTY
HEADERS=snake_types.h
all: snake
	
snake: main.o game.o $(HEADERS)
	gcc $(CFLAGS) main.o game.o -lncurses -o snake
	ctags -R
main.o: main.c $(HEADERS)
	gcc $(CFLAGS) main.c -c
game.o: game.c $(HEADERS)
	gcc $(CFLAGS) game.c -c
clean:
	- rm *.o snake

