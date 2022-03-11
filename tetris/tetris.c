/* authour: plapacz6@gmail.com, date: 2022-02-24 */
#include "tetris.h"
#include <stdio.h>
#include <locale.h>
#include <ncurses.h>



/**
 * @brief 
 * first coordynats are always 0,0  and relatively to them are sepecified others coordynates
 * so, maybe first coordynates are unnececery.
 */
#define FIGURE_NUMBER (15)
coord_t shape[FIGURE_NUMBER][4] = {

  //first dreft
  /* beginning block is first bottom left, others are relative to it */
  {{0,0},{+1,0},{0,-1},{+1,-1}}, // []
  
  {{0,0},{0,-1},{0,-2},{0,-3}}, //  |
  {{0,0},{-1,0},{-2,0},{-3,0}}, // ____   //rotation acdording to right block

  {{0,0},{+1,0},{+2,0},{+2,-1}}, // ___I
  {{0,0},{+1,0},{0,-1},{0,-2}}, //   |_
  {{0,0},{0,-1},{+1,-1},{+2,-1}}, //  I---
  {{0,0},{0,-1},{0,-2},{-1,-2}}, //   -|  
  
  {{0,0},{+1,0},{+2,0},{0,-1}}, //  I___
  {{0,0},{0,-1},{0,-2},{+1,-2}}, //   |-
  {{0,0},{-2,-1},{-1,-1},{0,-1}}, //  ---I
  {{0,0},{+1,0},{+1,-1},{+1,-2}}, //  _|

  /*
  each figure is diffrent, 
  diffrent point of rotation
  diffrent transformation coordinates druing sequential rotation

  square - not change at all 
  rotate -
  flit -

  line:
  rotate: alwas horizontal line chcange to vertical according to right vertice
  flitp -

  1.  L
  2.  uppderdown L
  3. ___I
  4.  I___
  5. reverse L
  6. uppdrdan reverse L
  7. ---|
  8. |---
  rotate: point of brak line: 
  left: 1 -> 3, ...
  right: 3 -> 1, ...
  flip:  1 -> 5, 2 -> 6, ...
  */
  {{0,0},{-1,0},{0,-1},{+1,-1}},
  {{0,0},{+1,0},{-1,-1},{0,-1}},
  {{0,0},{-1,-1},{0,-1},{-1,-2}},
  {{0,0},{0,-1},{+1,-1},{+1,-2}},
  /*
  1. _|-  (reverse z)
  2. -|_  z
  3. turned z
  4. turned reverse z
  rotate: point of break line
  left: 1->4, ...
  right:
  flip: reverse z-> z, z->reverse z, turned z -> turned reverse z, ...
  */

  /*
  _|`
  -v-
  */
};

/**
 * @brief checking if figure is in direct neibourshood with actual ground level 
 * 
 * @param f - figure
 * @param b - board
 * @return true 
 * @return false 
 */
bool is_bottom_contact(figure_t *f, board_t *b){
  for(int i = 0; i < 4 && f->b[i]; ++i){
    for(int j = 0; j < BOARD_WIDTH; ++j){
      if(((f->b[i]->y) - 1) == b->ground_level[j]){
        return true;
      }
    }
  }
  return false;
}

void step_down(figure_t *f);

void flip_horizontally(figure_t *f);
void flip_vertically(figure_t *f);
void move_left(figure_t *f);
void move_right(figure_t *f);
void move_down(figure_t *f);

figure_t *new_figure(/*color*/);  //alocate memory
void generate_figure(figure_t *f, coord_t *sh);  //generate coordinates of blocks, and shape

void test_shape_definition(){
  for(int i = 0; i < FIGURE_NUMBER; i++){
    for(int j = 0; j < 4; j++){
      printf("[%2d, %2d]", shape[i][j].x, shape[i][j].y);
    }
    printf("%s", "\n");
  }
}
void test_draw_shape(){
  int shift_x = 2;
  int shift_y = 10;

  uint16_t key;
  uint8_t keyH;
  uint8_t keyL;

  setlocale(LC_ALL, "");
  
  initscr();
  timeout(-1);
  raw();
  keypad(stdscr, TRUE);
  noecho();
  for(int i = 0; i < FIGURE_NUMBER; i++){
    for(int j = 0; j < 4; j++){      
      mvprintw(shift_y + shape[i][j].y, shift_x + shape[i][j].x, "%c", 'x'); //'\u2592'); //0x2588); // 219); // 178); //, "%d", i%10);      
    }
    shift_x += 6;
    refresh();
  }  
  key = getch();
  keyH = key >> 8;
  keyL = (uint8_t) key;
  bool finishDrawinFigures = false;
  while(!finishDrawinFigures){
    switch(key){
      case 27: //ESC
        finishDrawinFigures = true;
        break;
      case KEY_LEFT:
        break;
      case KEY_RIGHT:
        break;
      case KEY_UP:
        break;
      case KEY_DOWN:
        break;        
    }
  }
  mvprintw(0,0,"%d", key);
  endwin();
}
int main(int argc, char **argv){
  test_shape_definition();
  test_draw_shape();
  return 0;
}

