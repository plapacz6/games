/* authour: plapacz6@gmail.com, date: 2022-02-24 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <locale.h>
#include <stdbool.h>
#include <time.h>

#include <ncurses.h>
#include "tetris.h"


/**
 * @brief printf figure at the board
 * Before printing make copy of background
 * That copy of background is undependent on future 
 * flip or turn figure, because it remember coordinates
 * from it was take. 
 * But this function must be followerd by unprint_figure soon.
 * @param f 
 * @param pb 
 */
void print_figure(figure_t *f, board_t* pb){
  size_t i = 0;  
  for(i = 0; i < 4; i++){
    if(f->box[i]->bg_filled){
      perror("background not returned");            
    }
    assert(!f->box[i]->bg_filled);
    mvprintw(3, 0, "%s", "print box");
    move(f->box[i]->c.y, f->box[i]->c.x);
    f->box[i]->bg = inch();
    mvprintw(f->box[i]->c.y + pb->top_y, f->box[i]->c.x + pb->left_x, "%c", '@');    
    f->box[i]->bg_filled = true;
  }  
  refresh();
}

/**
 * @brief put back badground after print_figure rewrite it
 * require previous call print_figure, to copy bacdround content to reconstruct
 * @param f 
 * @param pb 
 */
void unprint_figure(figure_t *f, board_t* pb){
  size_t i = 0;
  for(i = 0; i < 4; i++){    
    if(f->box[i]->bg_filled){
      mvprintw(3, 0, "%s", "un_print box");
    
      mvprintw(
        f->box[i]->c.y + pb->top_y, 
        f->box[i]->c.x + pb->left_x, 
        "%c", f->box[i]->bg);
      f->box[i]->bg = 0;
      f->box[i]->bg_filled = false;        
    }    
    refresh();
  }
}


/**
 * @brief 
 * first coordynats are always 0,0  and relatively to them are sepecified others coordynates
 * so, maybe first coordinates are unnececery.
 */
#define FIGURE_NUMBER (20) //(19)
coord_t shape[FIGURE_NUMBER][4] = {
   
  /* -1 */{{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0}},  // for debug purpose only
  /* 0 */ {{ 0, 0},{+1, 0},{ 0,-1},{+1,-1}}, // []
  
  /* 1 */ {{ 0, 0},{ 0,-1},{ 0,-2},{ 0,-3}}, //  |
  /* 2 */ {{ 0, 0},{-1, 0},{-2, 0},{+1, 0}}, // ____   //rotation acdording to right block
  

  /* 3 */ {{ 0, 0},{+1, 0},{-1, 0},{+1,-1}}, // ___I
  /* 4 */ {{ 0, 0},{+1, 0},{ 0,-1},{ 0,-2}}, //   |_
  /* 5 */ {{-1, 0},{-1,-1},{ 0,-1},{+1,-1}}, //  I---
  /* 6 */ {{ 0, 0},{ 0,-1},{ 0,-2},{-1,-2}}, //   -|  
  
  /*  7*/ {{ 0, 0},{+1, 0},{-1, 0},{-1,-1}}, //  I___
  /*  8*/ {{ 0, 0},{ 0,-1},{ 0,-2},{+1,-2}}, //   |-
  /*  9*/ {{+1, 0},{-1,-1},{ 0,-1},{+1,-1}}, //  ---I
  /* 10*/ {{ 0, 0},{-1, 0},{ 0,-1},{ 0,-2}}, //  _|
  
  
  /* 11*/ {{ 0, 0},{-1, 0},{ 0,-1},{+1,-1}},  //turned_left_z
  /* 12*/ {{ 0, 0},{ 0,-1},{-1,-1},{-1,-2}},

  /* 13*/ {{ 0, 0},{+1, 0},{-1,-1},{ 0,-1}}, //13. -|_  z
  /* 14*/ {{ 0, 0},{ 0,-1},{+1,-1},{+1,-2}}, //14. _|-  (reverse z)

  
  /* 15*/ {{ 0, 0},{-1, 0},{+1, 0},{ 0,-1}},  //hat _A_
  /* 16*/ {{ 0, 0},{ 0,-1},{+1,-1},{ 0,-2}},  //vertical_left_A_  
  /* 17*/ {{ 0, 0},{-1,-1},{ 0,-1},{+1,-1}}, //reversed hat -v-
  /* 18*/ {{ 0, 0},{-1,-1},{ 0,-1},{ 0,-2}}  //vertical_left_v_

};


/**
 * @brief checking if figure is in direct neibourshood with actual ground level 
 * 
 * @param f - figure
 * @param b - board
 * @return true 
 * @return false 
 */
extern bool is_bottom_contact(figure_t *f, board_t *b){
  for(int i = 0; i < 4 && f->bt[i]; ++i){
    for(int j = 0; j < BOARD_WIDTH; ++j){
      if(((f->bt[i]->y) - 1) == b->ground_level[j]){
        return true;
      }
    }
  }
  return false;
}



/**
 * @brief set shape of figure
 * 
 * @param fg 
 * @return int 1 == no error
 */
int define_shape_figure(figure_t *fg, figure_shape_name_t fs) {
  int i = 0;
  fg->fs = fs;
  for(i = 0; i < 4; i++){
    fg->box[i]->c.x = shape[fs][i].x + fg->p0.x;
    fg->box[i]->c.y = shape[fs][i].y + fg->p0.y;
    assert(!fg->box[i]->bg_filled);
    fg->box[i]->bg = 0;
    fg->box[i]->bg_filled = false;
  }
  return 1;    
} /***************  complete ********************/

/**
 * @brief figure out left and right edge of figure
 * helping function
 * @param fg 
 * @return int 1 == no error
 */
int determine_LR_edge(figure_t *fg) {
    //figure out left and right edge of figure
  int i = 0;
  for(i = 0; i < 4; i++){
    if(fg->xl > fg->box[i]->c.x) fg->xl = fg->box[i]->c.x;
    if(fg->xr < fg->box[i]->c.x) fg->xr = fg->box[i]->c.x;    
  }
  return 1;
} /***************  complete ********************/


/**
 * @brief determine coordinates of bottof of figure
 * helping function
 * @param fg 
 * @return int 1 == no error
 */
int determine_bottom_line(figure_t *fg){
  int i = 0;
  int j = 0;
  for(i = 0; i < 4; i++){
    fg->bt[i] = &(fg->box[i]->c);
  }
  for(i = 0; i < 4; i++){
    for(j = i; j < 4; j++){
      if( fg->bt[i]->x == fg->bt[j]->x ){
        if( fg->bt[i]->y < fg->bt[j]->y ){
          fg->bt[i] = fg->bt[j];
  }}}}  
  return 1;    
} /***************  complete ********************/

/**
 * @brief Create a figure object
 * 
 * @param fs - shape of figure
 * @param p_box - pointer to reference box
 * @param p1 - beggining point on board 
 * @return figure_t* 
 */
extern figure_t *create_figure(figure_shape_name_t fs, box_t *p_bx, coord_t *p_p1){
  int i = 0;
  figure_t *fg = calloc(1, sizeof(figure_t));
  if(fg == NULL){
    perror("create figure: cant allocate memory");
    return NULL;    
  }  
  for(i = 0; i < 4; i++){
    fg->box[i] = NULL;
    fg->box[i] = calloc(1, sizeof(box_t));
    if(fg->box[i] == NULL) break;
  }
  if(i < 4){    
    perror("create figure: cant allocate memory");
    for( ; i >= 0; i--){
      free(fg->box[i]);
      fg->box[i] = NULL; //double free()
    }
    free(fg);
    fg = NULL;    
    return NULL;    
  }
  fg->p0.x = p_p1->x;
  fg->p0.y = p_p1->y;
  
  fg->xl = p_p1->x;
  fg->xr = p_p1->x;
  
  //TODO: unroll in one loop
  define_shape_figure(fg, fs);
  determine_LR_edge(fg);
  determine_bottom_line(fg);
  
  return fg;
} /***************  complete ********************/

/**
 * @brief dissasemble figure on boxes, and make them part of the ground
 * after moving boxes to the ground, free figure_t memory
 * @param f - figure
 * @param b - board
 */
extern void diassemble_figure(figure_t *f, board_t *b){
  int i = 0;
  for(i = 0; i < 4; i++){
    b->b[f->box[i]->c.y][f->box[i]->c.x] = f->box[i]->v;  //struct copy
    free(f->box[i]);
    f->box[i] = NULL; //double free()
  }

  free(f);
  f = NULL;
} /***************  complete ********************/



extern void turn_left(figure_t* f, coord_t *p1, board_t *p_b){
  figure_shape_name_t  ns = FS_DEBUG;
  switch(f->fs){   
  case FS_DEBUG:
    ns = FS_DEBUG;
    break;    
  case FS_B1_BOX: 
    ns = FS_B1_BOX;
    break;     //B1

  case FS_LN1_LINE_V: 
    ns = FS_LN2_LINE_H;
    break;  //LN1
  case FS_LN2_LINE_H: 
    ns = FS_LN1_LINE_V;
    break;  //LN2

  case FS_L1_L_LEFT: 
    ns = FS_L4_L_BOTTOMUP;
    break;   //L1
  case FS_L2_L_: 
    ns = FS_L1_L_LEFT;
    break;       //L2
  case FS_L3_L_RIGHT: 
    ns = FS_L2_L_;
    break;  //L3
  case FS_L4_L_BOTTOMUP: 
    ns = FS_L3_L_RIGHT;
    break;  //L4

  case FS_RL1_RL_LEFT: 
    ns = FS_RL4_RL_BOTTOMUP;
    break;  //RL1
  case FS_RL2_RL_: 
    ns = FS_RL1_RL_LEFT;
    break;      //RL2
  case FS_RL3_RL_RIGHT: 
    ns = FS_RL2_RL_;
    break; //RL3
  case FS_RL4_RL_BOTTOMUP: 
    ns = FS_RL3_RL_RIGHT;
    break; //RL4

  case FS_RZ1_RZ_: 
    ns = FS_RZ2_RZ_LEFT;
    break;      //RZ1
  case FS_RZ2_RZ_LEFT: 
    ns = FS_RZ1_RZ_;
    break;  //RZ2

  case FS_Z1_Z_: 
    ns = FS_Z2_Z_LEFT;
    break;       //Z1
  case FS_Z2_Z_LEFT: 
    ns = FS_Z1_Z_;
    break;   //Z2
  
  
  case FS_H1_HAT: 
    ns = FS_H4_RHAT_VL;
    break;       //H1
  case FS_H2_HAT_VL: 
    ns = FS_H1_HAT;
    break;    //H2
  case FS_H3_RHAT: 
    ns = FS_H2_HAT_VL;
    break;      //H3
  case FS_H4_RHAT_VL: 
    ns = FS_H3_RHAT;
    break;   //H4  
  }
  unprint_figure(f, p_b);
  define_shape_figure(f, ns);
  determine_LR_edge(f);
  determine_bottom_line(f);
}


extern void turn_right(figure_t* f, coord_t *p1, board_t *p_b){
  figure_shape_name_t  ns = FS_DEBUG;
 switch(f->fs){   
  case FS_DEBUG:
    ns = FS_DEBUG;
    break;
  case FS_B1_BOX: 
    ns = FS_B1_BOX;
    break;     //B1

  case FS_LN1_LINE_V: 
    ns = FS_LN2_LINE_H;
    break;  //LN1
  case FS_LN2_LINE_H: 
    ns = FS_LN1_LINE_V;
    break;  //LN2

  case FS_L1_L_LEFT: 
    ns = FS_L2_L_;
    break;   //L1
  case FS_L2_L_: 
    ns = FS_L3_L_RIGHT;
    break;       //L2
  case FS_L3_L_RIGHT: 
    ns = FS_L4_L_BOTTOMUP;
    break;  //L3
  case FS_L4_L_BOTTOMUP: 
    ns = FS_L1_L_LEFT;
    break;  //L4

  case FS_RL1_RL_LEFT: 
    ns = FS_RL2_RL_;
    break;  //RL1
  case FS_RL2_RL_: 
    ns = FS_RL3_RL_RIGHT;
    break;      //RL2
  case FS_RL3_RL_RIGHT: 
    ns = FS_RL4_RL_BOTTOMUP;
    break; //RL3
  case FS_RL4_RL_BOTTOMUP: 
    ns = FS_RL1_RL_LEFT;
    break; //RL4

  case FS_RZ1_RZ_: 
    ns = FS_RZ2_RZ_LEFT;
    break;      //RZ1
  case FS_RZ2_RZ_LEFT: 
    ns = FS_RZ1_RZ_;
    break;  //RZ2

  case FS_Z1_Z_: 
    ns = FS_Z2_Z_LEFT;
    break;       //Z1
  case FS_Z2_Z_LEFT: 
    ns = FS_Z1_Z_;
    break;   //Z2
  
  
  case FS_H1_HAT: 
    ns = FS_H2_HAT_VL;
    break;       //H1
  case FS_H2_HAT_VL: 
    ns = FS_H3_RHAT;
    break;    //H2
  case FS_H3_RHAT: 
    ns = FS_H4_RHAT_VL;
    break;      //H3
  case FS_H4_RHAT_VL: 
    ns = FS_H1_HAT;
    break;   //H4  
  }
  unprint_figure(f, p_b);
  define_shape_figure(f, ns);
  determine_LR_edge(f);  
  determine_bottom_line(f);
}





extern void move_right(figure_t *f, board_t *b){
  size_t i = 0;
  //size_t j = 0;
  mvprintw(5,0, "%s", "moving RIGHT");
  if(f->xr < (b->right_x - 4) ){
    for(i = 0; i < 4; i++){    
      f->box[i]->c.x++;
    }
    f->p0.x++;    
    f->xr++;
    f->xl++;
  }
}

extern void move_left(figure_t *f, board_t *b){
  size_t i = 0;
  //size_t j = 0;
  mvprintw(5,0, "%s", "moving LEFT");
  if(f->xl > (b->left_x + 4)) {
    for(i = 0; i < 4; i++){    
      f->box[i]->c.x--;
    }
    f->p0.x--;
    f->xr--;
    f->xl--;
  }
}
extern void move_down(figure_t *f);




/**
 * @brief 
 * 
 * @param f 
 */
extern void flip_horizontally(figure_t *f) {
  //TODO: wrong
  int i = 0;
  unsigned horiz_revrse_x[4];
  for(i = 0; i < 4; i++){
    mvprintw(6 + i, 2,"c.x[%d] c,y[%d]", f->box[i]->c.x, f->box[i]->c.y);        
    horiz_revrse_x[3 - i] = f->box[i]->c.x;
  }
  for(i = 0; i < 4; i++){
    f->box[i]->c.x = horiz_revrse_x[i];
    mvprintw(6 + i, 15,"c.x[%d] c,y[%d]", f->box[i]->c.x, f->box[i]->c.y);        
  }        
  determine_LR_edge(f);
  determine_bottom_line(f);
}

extern void flip_vertically(figure_t *f){
  //TODO: wrong
  int i = 0;  
  unsigned vertical_revrse_y[4];
  for(i = 0; i < 4; i++){
    mvprintw(6 + i, 2,"c.x[%d] c,y[%d]", f->box[i]->c.x, f->box[i]->c.y);        
    vertical_revrse_y[3 - i] = f->box[i]->c.y;
  }
  for(i = 0; i < 4; i++){
    f->box[i]->c.y = vertical_revrse_y[i];
    mvprintw(6 + i, 15,"c.x[%d] c,y[%d]", f->box[i]->c.x, f->box[i]->c.y);        
  }        
  determine_LR_edge(f);
  determine_bottom_line(f);
}






void test_shape_definition(){
  for(int i = 0; i < FIGURE_NUMBER; i++){
    for(int j = 0; j < 4; j++){
      printf("[%2d, %2d]", shape[i][j].x, shape[i][j].y);
    }
    printf("%s", "\n");
  }
}



void test_draw_shape(board_t *p_board, box_t *p_ref_box, coord_t *p_p1){
  int shift_x = 2;
  int shift_y = 10;

  int16_t key;
  //uint8_t keyH;
  //uint8_t keyL;
  bool finishDrawinFigures = false;

  figure_t *f = NULL;
  
  if(! (f = create_figure(FS_Z1_Z_, p_ref_box, p_p1))) exit(-1);


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
        unprint_figure(f, p_board);        
        print_figure(f, p_board);
        break;
      case 'n':
        unprint_figure(f, p_board);        
        free(f);
        f = NULL;
        if(! (f = create_figure(rand()%19, p_ref_box, p_p1))) exit(-1);
        print_figure(f, p_board);
        break;
      case KEY_LEFT:
      case 'l':
        unprint_figure(f, p_board);
        move_left(f, p_board);
        print_figure(f, p_board);
        break;
      case KEY_RIGHT:
      case 'r':
        unprint_figure(f, p_board);
        move_right(f, p_board);
        print_figure(f, p_board);
        break;
      case KEY_UP:
        unprint_figure(f, p_board);
        turn_left(f, p_p1, p_board);
        //flip_horizontally(f);
        print_figure(f, p_board);
        break;
      case KEY_DOWN:
        unprint_figure(f, p_board);
        turn_right(f, p_p1, p_board);
        //flip_vertically(f);
        print_figure(f, p_board);
        break; 
      //default:         
       // break;      
    }//switch
  }//while
  mvprintw(0,0,"%d", key);
  endwin();
}//test_draw_shape()



int main(int argc, char **argv){
  board_t board;
  coord_t p1;
  box_t ref_box;
  ref_box.c.x = 0;
  ref_box.c.y = 0;
  ref_box.v.c = 1;
  ref_box.v.p = '*';


  board.top_y = 10;
  board.left_x = 10;
  board.right_x = 60;
  board.bottom_y = 25;
  
  p1.x = (board.left_x + board.right_x) / 2;
  p1.y = board.top_y;
  srand(time(NULL));
  
  test_shape_definition();
  test_draw_shape(&board, &ref_box, &p1);
  return 0;
}



// /* ____   //rotation acdording to right block
//   {{0,0},{+1,0},{+2,0},{+2,-1}}, // ___I
//   {{0,0},{+1,0},{0,-1},{0,-2}}, //   |_
//   {{0,0},{0,-1},{+1,-1},{+2,-1}}, //  I---
//   {{0,0},{0,-1},{0,-2},{-1,-2}}, //   -|  
// */
// typedef struct rotate_table_tt {
//   size_t n;
//   figure_shape_name_t *form;
// } rotate_table_t;
// /**
//  * @brief Create and fills a rotate table object
//  * which is used to figure out next form of figure during rotation
//  * @param n 
//  * @param ... 
//  * @return rotate_table_t* 
//  */
// rotate_table_t *create_rotate_table(size_t n, ...){    
//   rotate_table_t *rt = NULL;
//   int i = 0;  
//   va_list args;  
//   va_start(args, n);

//   rt = calloc(1, sizeof(rotate_table_t));  
//   if(rt == NULL){
//     perror("rotate_table_t, can't allocate memory");
//     return NULL;
//     //exit(-1);
//   }      
//   rt->form = calloc(n, sizeof(figure_shape_name_t));
//   if(rt->form == NULL){
//     perror("rotate_table_t->form, can't allocate memory");
//     free(rt);
//     return NULL;
//     //exit(-1);    
//   }
//   rt->n = n;
//   for(i = 0; i < n; i++){
//     rt->form[i]  = va_arg(args, figure_shape_name_t);
//   }
//   va_end(args);
//   return rt;
// }



// int craete_tables(){
//   rotate_table_t *rt_box = create_rotate_table(1, FS_BOX);
//   rotate_table_t *rt_line = create_rotate_table(2, FS_LINE_V, FS_LINE_H);
//   rotate_table_t *rt_L = create_rotate_table(4, FS_L_, FS_L_LEFT, FS_L_BOTTOMUP, FS_L_RIGHT);
//   rotate_table_t *rt_RL = create_rotate_table(4, FS_RL_, FS_RL_LEFT, FS_RL_BOTTOMUP, FS_RL_RIGHT);
//   rotate_table_t *rt_Z = create_rotate_table(2, FS_Z_, FS_Z_LEFT);
//   rotate_table_t *rt_RZ = create_rotate_table(2, FS_RZ_, FS_RZ_LEFT);
//   if(
//     rt_box == NULL || rt_line == NULL || rt_L == NULL || rt_RL == NULL ||
//     rt_Z == NULL || rt_RZ == NULL) 
//     return -1;
//   printf("box: %d: %d\n", (int)rt_box->n, (int)rt_box->form[0]);
//   printf("rt_L: %d: %d, %d, %d, %d\n", 
//     (int)rt_L->n, 
//     (int)rt_L->form[0], (int)rt_L->form[1], 
//     (int)rt_L->form[2], (int)rt_L->form[3]);

//   return 0;
// }
