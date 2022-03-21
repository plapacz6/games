/* authour: plapacz6@gmail.com, date: 2022-02-24 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <locale.h>
#include <stdbool.h>

#include <ncurses.h>
#include "tetris.h"

board_t board;
coord_t p0;

/**
 * @brief 
 * first coordynats are always 0,0  and relatively to them are sepecified others coordynates
 * so, maybe first coordinates are unnececery.
 */
#define FIGURE_NUMBER (20) //(19)
coord_t shape[FIGURE_NUMBER][4] = {

  //first dreft
  /*
  each figure is diffrent, 
  diffrent point of rotation
  diffrent transformation coordinates druing sequential rotation

  0. square - not change at all   
  1. line:
  2. line_vertical:
  */
  /* beginning block is first bottom left, others are relative to it */
  /* -1 */ {{0,0},{0,0},{0,0},{0,0}},  // for debug purpose only
  /* 0 */ {{0,0},{+1,0},{0,-1},{+1,-1}}, // []
  
  /* 1 */ {{0,0},{0,-1},{0,-2},{0,-3}}, //  |
  /* 2 */ {{0,0},{-1,0},{-2,0},{-3,0}}, // ____   //rotation acdording to right block
  /*
    3. ___I
    4.  L
    5. |---
    6.  uppderdown L -|
    
    7.  I___    
    8. uppdrdan reverse L |-
    9. ---|
    10. reverse L _|
  */
  /* 3 */ {{0,0},{+1,0},{+2,0},{+2,-1}}, // ___I
  /* 4 */ {{0,0},{+1,0},{0,-1},{0,-2}}, //   |_
  /* 5 */ {{0,0},{0,-1},{+1,-1},{+2,-1}}, //  I---
  /* 6 */ {{0,0},{0,-1},{0,-2},{-1,-2}}, //   -|  
  
  /*  7*/ {{0,0},{+1,0},{+2,0},{0,-1}}, //  I___
  /*  8*/ {{0,0},{0,-1},{0,-2},{+1,-2}}, //   |-
  /*  9*/ {{0,0},{-2,-1},{-1,-1},{0,-1}}, //  ---I
  /* 10*/ {{0,0},{+1,0},{+1,-1},{+1,-2}}, //  _|
  
  /*
  11. //turned_left_z
  13. -|_  z
  14. _|-  (reverse z - rz)
  12. //turned_left_rz 
  */
  /* 11*/ {{0,0},{-1,0},{0,-1},{+1,-1}},  //turned_left_z
  /* 12*/ {{0,0},{+1,0},{-1,-1},{0,-1}},
  /* 13*/ {{0,0},{-1,-1},{0,-1},{-1,-2}}, //13. -|_  z
  /* 14*/ {{0,0},{0,-1},{+1,-1},{+1,-2}}, //14. _|-  (reverse z)

  /*  
  15. _A_
  16. vertical_left_A_
  17. -v-
  18. vertical_left_v_ 
  */
  /* 15*/ {{0,0},{0,+1},{-1,+1},{+1,+1}},  //hat _A_
  /* 16*/ {{0,0},{-1,0},{-1,-1},{-2,0}},  //vertical_left_A_
  /* 17*/ {{0,0},{-1,-1},{-1,0},{-1,+1}}, //reversed hat -v-
  /* 18*/ {{0,0},{-1,0},{-1,+1},{-2,0}},  //vertical_left_v_

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

typedef enum  {
  FS_DEBUG,
  FS_BOX,

  FS_LINE_V,
  FS_LINE_H,

  FS_L_LEFT,
  FS_L_,
  FS_L_RIGHT,
  FS_L_BOTTOMUP,

  FS_RL_LEFT,
  FS_RL_,
  FS_RL_RIGHT,
  FS_RL_BOTTOMUP,

  FS_RZ_,
  FS_Z_,
  FS_Z_LEFT,
  FS_RZ_LEFT,
  
  FS_HAT,
  FS_HAT_VL,
  FS_RHAT,
  FS_RHAT_VL,
} figure_shape_name_t;

figure_t *create_figure(figure_shape_name_t fs, coord_t p0){
  size_t i = 0;
  size_t j = 0;
  figure_t *fg = calloc(1, sizeof(figure_t));
  if(fg == NULL){
    perror("create figure: cant allocate memory");
    return NULL;
    //exit(-1);
  }
  fg->xl = p0.x;
  fg->xr = p0.x;
    
  for(i = 0; i < 4; i++){
    fg->c[i].x = shape[fs][i].x + p0.x;
    fg->c[i].y = shape[fs][i].y + p0.y;
    //figure out left and right edge of figure
    if(fg->xl > fg->c[i].x) fg->xl = fg->c[i].x;
    if(fg->xr < fg->c[i].x) fg->xr = fg->c[i].x;
    fg->b[i] = &(fg->c[i]);
  }
  //figure out  //
  for(i = 0; i < 4; i++){
    for(j = i; j < 4; j++){
      if( fg->b[i]->x == fg->b[j]->x ){
        if( fg->b[i]->y < fg->b[j]->y ){
          fg->b[i] = fg->b[j];
  }}}}  
  
  return fg;
}


/* ____   //rotation acdording to right block
  {{0,0},{+1,0},{+2,0},{+2,-1}}, // ___I
  {{0,0},{+1,0},{0,-1},{0,-2}}, //   |_
  {{0,0},{0,-1},{+1,-1},{+2,-1}}, //  I---
  {{0,0},{0,-1},{0,-2},{-1,-2}}, //   -|  
*/
typedef struct rotate_table_tt {
  size_t n;
  figure_shape_name_t *form;
} rotate_table_t;
/**
 * @brief Create and fills a rotate table object
 * which is used to figure out next form of figure during rotation
 * @param n 
 * @param ... 
 * @return rotate_table_t* 
 */
rotate_table_t *create_rotate_table(size_t n, ...){    
  rotate_table_t *rt = NULL;
  int i = 0;  
  va_list args;  
  va_start(args, n);

  rt = calloc(1, sizeof(rotate_table_t));  
  if(rt == NULL){
    perror("rotate_table_t, can't allocate memory");
    return NULL;
    //exit(-1);
  }      
  rt->form = calloc(n, sizeof(figure_shape_name_t));
  if(rt->form == NULL){
    perror("rotate_table_t->form, can't allocate memory");
    free(rt);
    return NULL;
    //exit(-1);    
  }
  rt->n = n;
  for(i = 0; i < n; i++){
    rt->form[i]  = va_arg(args, figure_shape_name_t);
  }
  va_end(args);
  return rt;
}

int craete_tables(){
  rotate_table_t *rt_box = create_rotate_table(1, FS_BOX);
  rotate_table_t *rt_line = create_rotate_table(2, FS_LINE_V, FS_LINE_H);
  rotate_table_t *rt_L = create_rotate_table(4, FS_L_, FS_L_LEFT, FS_L_BOTTOMUP, FS_L_RIGHT);
  rotate_table_t *rt_RL = create_rotate_table(4, FS_RL_, FS_RL_LEFT, FS_RL_BOTTOMUP, FS_RL_RIGHT);
  rotate_table_t *rt_Z = create_rotate_table(2, FS_Z_, FS_Z_LEFT);
  rotate_table_t *rt_RZ = create_rotate_table(2, FS_RZ_, FS_RZ_LEFT);
  if(
    rt_box == NULL || rt_line == NULL || rt_L == NULL || rt_RL == NULL ||
    rt_Z == NULL || rt_RZ == NULL) 
    return -1;
  printf("box: %d: %d\n", (int)rt_box->n, (int)rt_box->form[0]);
  printf("rt_L: %d: %d, %d, %d, %d\n", 
    (int)rt_L->n, 
    (int)rt_L->form[0], (int)rt_L->form[1], 
    (int)rt_L->form[2], (int)rt_L->form[3]);

  return 0;
}
void rotate_left(figure_t *f){

}
void rotate_right(figure_t *f){

}



void move_right(figure_t *f, board_t *b){
  size_t i = 0;
  //size_t j = 0;
  mvprintw(5,0, "%s", "moving RIGHT");
  if(f->xr < (b->right_x - 4) ){
    for(i = 0; i < 4; i++){    
      f->c[i].x++;
    }
    f->xr++;
    f->xl++;
  }
}
void move_left(figure_t *f, board_t *b){
  size_t i = 0;
  //size_t j = 0;
  mvprintw(5,0, "%s", "moving LEFT");
  if(f->xl > (b->left_x + 4)) {
    for(i = 0; i < 4; i++){    
      f->c[i].x--;
    }
    f->xr--;
    f->xl--;
  }
}
void move_down(figure_t *f);

void print_figure(figure_t *f, board_t* pb){
  size_t i = 0;
  mvprintw(3, 0, "%s", "print figure");
  for(i = 0; i < 4; i++){
    move(f->c[i].y, f->c[i].x);
    f->background[i] = inch();
    mvprintw(f->c[i].y + pb->top_y, f->c[i].x + pb->left_x, "%c", '@');    
  }
  refresh();
}
void unprint_figure(figure_t *f, board_t* pb){
  size_t i = 0;
  mvprintw(3, 0, "%s", "un_print figure");
  for(i = 0; i < 4; i++){    
    mvprintw(f->c[i].y + pb->top_y, f->c[i].x + pb->left_x, "%c", f->background[i]);
  }
  refresh();
}

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

  int16_t key;
  //uint8_t keyH;
  //uint8_t keyL;
  bool finishDrawinFigures = false;

  figure_t *f = NULL;
  
  if(craete_tables() == -1) exit(-1);
  if(! (f = create_figure(FS_Z_, p0))) exit(-1);


  setlocale(LC_ALL, "");
  
  initscr();
  timeout(-1); //3); //-1);
  raw();
  keypad(stdscr, TRUE);
  noecho();
  
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
  
  while(!finishDrawinFigures){
    mvprintw(0,0, "[ESC] or [q] => exit, [c] => clear");    
    key = 0;
    key = getch();
    //keyH = key >> 8;
    //keyL = (uint8_t) key;
    mvprintw(1,0,"%c", key);
    switch(key){
      case 27: //ESC
      case 'q':
        finishDrawinFigures = true;
        break;
      case 'c':
        clear();
        print_figure(f, &board);
        break;
      case KEY_LEFT:
      case 'l':
        unprint_figure(f, &board);
        move_left(f, &board);
        print_figure(f, &board);
        break;
      case KEY_RIGHT:
      case 'r':
        unprint_figure(f, &board);
        move_right(f, &board);
        print_figure(f, &board);
        break;
      case KEY_UP:
        break;
      case KEY_DOWN:
        break; 
      //default:         
       // break;      
    }
  }
  mvprintw(0,0,"%d", key);
  endwin();
}
int main(int argc, char **argv){
  board.top_y = 10;
  board.left_x = 10;
  board.right_x = 60;
  board.bottom_y = 25;
  
  p0.x = (board.left_x + board.right_x) / 2;
  p0.y = board.top_y;
  
  test_shape_definition();
  test_draw_shape();
  return 0;
}

void flip_horizontally(figure_t *f) {
  figure_shape_name_t fs = FS_BOX;

  switch(fs) {
    case FS_BOX: {
      break;
    }
    case FS_LINE_V: {
      break;
    }
    case FS_LINE_H: {
      break;
    }

    case FS_L_LEFT: {
      break;
    }
    case FS_L_: {
      break;
    }
    case FS_L_RIGHT: {
      break;
    }
    case FS_L_BOTTOMUP: {
      break;
    }

    case FS_RL_LEFT: {
      break;
    }
    case FS_RL_: {
      break;
    }
    case FS_RL_RIGHT: {
      break;
    }
    case FS_RL_BOTTOMUP: {
      break;
    }

    case FS_RZ_: {
      break;
    }
    case FS_Z_: {
      break;
    }
    case FS_Z_LEFT: {
      break;
    }
    case FS_RZ_LEFT: {
      break;
    }
    default: {

    }
  }//switch
}
void flip_vertically(figure_t *f);