CFLAGS=-Wall -g3 -fsanitize=address

GAME_HEADERS=\
game_logic.h game_UI.h init_UI.h snake_logic.h apples.h\
menu_logic.h menu_UI.h menu_definitions.h game_menu_definition.h

GAME_OBJECTS=\
game_logic.o game_UI.o init_UI.o snake_logic.o apples.o\
menu_logic.o menu_UI.o game_menu_definition.o\
main.o 

TEST_MENU_HEADERS=init_UI.h menu_logic.h menu_UI.h menu_definitions.h test_menu_definition.h
TEST_OBJECTS=test_menu_ncurses.o menu_logic.o menu_UI.o init_UI.o

targedUI=NCURSES
targetGAME=GAME_MENU
targetTEST=TEST_MENU

snake_UI:  $(GAME_HEADERS) $(GAME_OBJECTS)
	gcc $(CFLAGS) $(GAME_OBJECTS) -lncurses -o snake_UI
	ctags -R

game_logic.o: game_logic.c $(GAME_HEADERS)
	gcc $(CFLAGS) -D$(targedUI) -D$(targetGAME) game_logic.c -c
game_UI.o: game_UI.c $(GAME_HEADERS)
	gcc $(CFLAGS) -D$(targedUI) -D$(targetGAME) game_UI.c -c
init_UI.o: init_UI.c $(GAME_HEADERS)
	gcc $(CFLAGS) -D$(targedUI) -D$(targetGAME) init_UI.c -c
main.o: main.c $(GAME_HEADERS)
	gcc $(CFLAGS) -D$(targedUI) -D$(targetGAME) main.c -c
menu_logic.o: menu_logic.c $(GAME_HEADERS)
	gcc $(CFLAGS) -D$(targedUI) -D$(targetGAME) menu_logic.c -c
menu_UI.o: menu_UI.c $(GAME_HEADERS) $(GAME_HEADERS)
	gcc $(CFLAGS) -D$(targedUI) -D$(targetGAME) menu_UI.c -c
snake_logic.o: snake_logic.c $(GAME_HEADERS)
	gcc $(CFLAGS) -D$(targedUI) -D$(targetGAME) snake_logic.c -c
apples.o: apples.c $(GAME_HEADERS)
	gcc $(CFLAGS) -D$(targedUI) -D$(targetGAME) apples.c -c
game_menu_definition.o: game_menu_definition.c $(GAME_HEADERS)
	gcc $(CFLAGS) -D$(targedUI) -D$(targetGAME) game_menu_definition.c -c


test_menu_ncurses.o: test_menu_ncurses.c test_menu_definition.h $(TEST_MENU_HEADERS)
	gcc $(CFLAGS) -D$(targedUI) -D$(targetTEST) test_menu_ncurses.c -c
test_menu_ncurses: $(TEST_OBJECTS) $(TEST_MENU_HEADERS)
	gcc $(CFLAGS) -D$(targedUI) $(TEST_OBJECTS) -lncurses -o test_menu_ncurses

.PHONY: clean
clean:
	- rm *.o snake_UI test_menu_ncurses


