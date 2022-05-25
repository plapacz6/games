CC=gcc
FLAGS= -g -Wall -std=c99 -pthread
LINKER_FLAGS= -g -Wall 
LIBS=  $(ncursesw5-config --cflags --libs) -lncurses 

OBJECTS= tetris.o


tetris: tetris.o control_loop.o
	$(CC) $(LINKER_FLAGS) $^  $(LIBS) -o $@

tetris.o: tetris.c tetris.h control_version.h
	$(CC) $(FLAGS) -DCONTROL_LOOP tetris.c -c

control_loop.o: control_loop.c tetris.h control_version.h
	$(CC) $(FLAGS) -DCONTROL_LOOP control_loop.c -c



tetris_th: tetris_th.o control_thread.o
	$(CC) $(LINKER_FLAGS) $^  -pthread $(LIBS) -o $@

tetris_th.o: tetris.c tetris.h control_version.h
	$(CC) $(FLAGS) -DCONTROL_THREAD -phtread tetris.c -c -o tetris_th.o

control_thread.o: control_thread.c tetris.h control_version.h
	$(CC) $(FLAGS) -DCONTROL_THREAD -phtread control_thread.c -c



control_signal.o: control_signal.c tetris.h control_version.h
	$(CC) $(FLAGS) -DCONTROL_SIGNAL control_signal.c -c

.PHONY: clean

clean:
	-rm $(OBJECTS) tetris tetris_th

#gcc -o out $(ncursesw5-config --cflags --libs) input.c	
