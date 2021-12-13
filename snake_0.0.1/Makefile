CFLAGS=-Wall -g
HEADERS=snake_types.h #game.h snake.h
OBJECTS=main.o game.o #snake.o

all: snake
	
snake: $(OBJECTS) $(HEADERS)
	gcc $(CFLAGS) $(OBJECTS) -lncurses -o snake
	ctags -R
main.o: main.c $(HEADERS)
	gcc $(CFLAGS) main.c -c
game.o: game.c $(HEADERS)
	gcc $(CFLAGS) game.c -c
snake.o: snake.c $(HEADERS)
	gcc $(CFLAGS) snake.c -c
testuj_kody_kbd_ncurser.o: testuj_kody_kbd_ncurser.c $(HEADERS)
	gcc $(CFLAGS) testuj_kody_kbd_ncurser.c -c

testuj_kody_kbd_ncurser: testuj_kody_kbd_ncurser.c
	gcc $(CFLAGS) -DTEST testuj_kody_kbd_ncurser.c -lncurses -o testuj_kody_kbd_ncurser

.PHONY: clean
clean:
	-rm *.o snake testuj_kody_kbd_ncurser

