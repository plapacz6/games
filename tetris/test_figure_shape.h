/* author: plapacz6@gmail.com, date: 2022-02-24 */
#include <stdio.h>
#include <ncurses.h>
#include "tetris.h"
#include "control_version.h"

/* DEBUG */
void test_shape_definition(){
  for(int i = 0; i < FIGURE_NUMBER; i++){
    for(int j = 0; j < 4; j++){
      printf("[%2d, %2d]", shape[i][j].x, shape[i][j].y);
    }
    printf("%s", "\n");
  }
}

/* DEBUG */
void test_print_figures(){
  int shift_x = 2;
  int shift_y = 10;
  for(int i = 0; i < FIGURE_NUMBER; i++){
    if(!(i % 4)) {
      shift_y += 4;
      shift_x = 2;
    }
    for(int j = 0; j < 4; j++){      
      mvprintw(shift_y + shape[i][j].y, shift_x + shape[i][j].x, "%c", 'x'); //'\u2592'); //0x2588); // 219); // 178); //, "%d", i%10);      
    }
    shift_x += 6;
    refresh();
  }  
}