/* author: plapacz6@gmail.com, date: 2022-02-24 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <locale.h>
#include <stdbool.h>
#include <time.h>

#include <ncurses.h>
#include "tetris.h"

WINDOW *wnd_info = NULL;
next_figure_window_t win_next;
score_window_t win_score;  
hint_window_t win_hint;
figure_t *current_figure = NULL;
figure_t *next_figure = NULL;
coord_t p1;  //point of createing new figure

void print_info(unsigned y, unsigned x,char *fs, char *msg){
  mvwprintw(wnd_info,y,x,fs,msg);   
  wrefresh(wnd_info);
}



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
void print_figure(gm_window_t *p_w, figure_t *f){
  size_t i = 0;  
  for(i = 0; i < 4; i++){
    if(f->box[i]->bg_filled){
      perror("background not returned");            
    }
    assert(!f->box[i]->bg_filled);
    print_info(3, 1, "%s", "print box_t");
    wmove(p_w->w, f->box[i]->c.y, f->box[i]->c.x);
    f->box[i]->bg = inch();
    mvwprintw(p_w->w, 
      f->box[i]->c.y, 
      f->box[i]->c.x, 
      "%c", f->box[i]->v.p);    
    f->box[i]->bg_filled = true;
  }  
  wrefresh(p_w->w);
}

/**
 * @brief put back badground after print_figure rewrite it
 * require previous call print_figure, to copy bacdround content to reconstruct
 * @param f 
 * @param pb 
 */
void unprint_figure(gm_window_t *p_w, figure_t *f){
  size_t i = 0;
  for(i = 0; i < 4; i++){    
    if(f->box[i]->bg_filled){
      print_info(3, 1, "%s", "un_print box");
    
      mvwprintw(
        p_w->w,
        f->box[i]->c.y, 
        f->box[i]->c.x, 
        "%c", f->box[i]->bg);
      f->box[i]->bg = 0;
      f->box[i]->bg_filled = false;        
    }    
    wrefresh(p_w->w);
  }
}

void print_ground(board_t *p_b){
  int i = 0; 
  int j = 0;
  for(i = 1; i < BORAD_HIGHT - 1; i++)
    for(j = 1; j < BOARD_WIDTH -1; j++){
      mvwprintw(p_b->w.w, i, j, "%c", p_b->b[i][j].p);
    }
  wrefresh(p_b->w.w);
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
#define REF_BOX_V_MAX (5)
box_visual_t ref_box_v[REF_BOX_V_MAX] = {
  {'X',1},{'x',1},{'*',1},{'o',1},{'#',1}
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
  mvwprintw(win_score.w.w, 6,1,"%s","is_bottom_contact");
  wrefresh(win_score.w.w);
  int i = 0;  
  for(i = 0; i < 4; ++i) {
    if(b->ground_level[f->box[i]->c.x] == f->box[i]->c.y){
      return true;
    }
  }
  return false;
}
/*TODO

  There is something wrong w deissabemle or is_bottom_contact
  ones figure catched fragment of groudn and fallen down beneth
  ground
*/


/**
 * @brief set shape of figure
 * 
 * @param fg 
 * @return int 1 == no error
 */
int define_shape_figure(figure_t *fg, figure_shape_name_t fs, unsigned bx) {
  int i = 0;
  fg->fs = fs;
  for(i = 0; i < 4; i++){
    fg->box[i]->c.x = shape[fs][i].x + fg->p0.x;
    fg->box[i]->c.y = shape[fs][i].y + fg->p0.y;
    assert(!fg->box[i]->bg_filled);
    fg->box[i]->bg = 0;
    fg->box[i]->bg_filled = false;
    fg->box[i]->v.p = ref_box_v[bx].p;
    fg->box[i]->v.c = ref_box_v[bx].c;
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
 * @brief Create a figure object
 * 
 * @param fs - shape of figure
 * @param p_box - pointer to reference box
 * @param p1 - beggining point on board 
 * @return figure_t* 
 */
extern figure_t *create_figure(figure_shape_name_t fs, unsigned bx, coord_t *p_p1){
  assert(bx < REF_BOX_V_MAX);
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
  fg->bx = bx;
  
  //TODO: unroll in one loop
  define_shape_figure(fg, fs, bx);  
  determine_LR_edge(fg);  
  
  return fg;
} /***************  complete ********************/

/**
 * @brief dissasemble figure on boxes, and make them part of the ground
 * after moving boxes to the ground, free figure_t memory
 * @param f - figure
 * @param b - board
 */
extern void diassemble_figure(figure_t *f, board_t *b){
  mvwprintw(win_score.w.w, 6,1,"%s","disassemble figure");
  wrefresh(win_score.w.w);
  int i = 0;  
  for(i = 0; i < 4; i++){
    if(b->ground_level[ f->box[i]->c.x ] > f->box[i]->c.y) {
      b->ground_level[ f->box[i]->c.x ] = f->box[i]->c.y;
    }
    b->b[f->box[i]->c.y][f->box[i]->c.x].p = f->box[i]->v.p;  
    b->b[f->box[i]->c.y][f->box[i]->c.x].c = f->box[i]->v.c;  
    
    free(f->box[i]);
    f->box[i] = NULL; //double free()
  }

  free(f);
  f = NULL;
} /***************  complete ********************/



extern void turn_left(figure_t* f, board_t *p_b){
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
  unprint_figure(&p_b->w,f);
  define_shape_figure(f, ns, f->bx);
  determine_LR_edge(f);  
}


extern void turn_right(figure_t* f, board_t *p_b){
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
  unprint_figure(&p_b->w,f);
  define_shape_figure(f, ns, f->bx);
  determine_LR_edge(f);    
}


extern void move_right(board_t *p_b, figure_t *f){
  size_t i = 0;
  //size_t j = 0;
  print_info(5,1, "%s", "moving RIGHT");
  if(f->xr < (p_b->right_x - 4) ){
    for(i = 0; i < 4; i++){    
      f->box[i]->c.x++;
    }
    f->p0.x++;    
    f->xr++;
    f->xl++;
  }
}

extern void move_left(board_t *p_b, figure_t *f){
  size_t i = 0;
  //size_t j = 0;
  print_info(5,1, "%s", "moving LEFT");
  if(f->xl > (p_b->left_x + 4)) {
    for(i = 0; i < 4; i++){    
      f->box[i]->c.x--;
    }
    f->p0.x--;
    f->xr--;
    f->xl--;
  }
}

extern void step_down(figure_t *f, board_t *p_b){
  int i = 0;
  f->p0.y++;
  for(i = 0; i < 4; i++) {
    f->box[i]->c.y++;
  }
  if(f->p0.y > BORAD_HIGHT - 1) {
    mvwprintw(p_b->w.w, BORAD_HIGHT, 1, "the bottom has been reached");
    wrefresh(p_b->w.w);
  }
  
  if(is_bottom_contact(f,p_b)){
    diassemble_figure(f, p_b);
    print_ground(p_b);
    //f = NULL;  //in diasseble, and local varialble
    unprint_figure(&win_next.w, next_figure);
    current_figure = next_figure;  
    //f = current_figure;  //local variable
    
    next_figure = NULL;
    if(! (next_figure = create_figure(rand()%19 + 1, rand()%REF_BOX_V_MAX, &p1))) exit(-1);
    print_figure(&win_next.w, next_figure);
    mvwprintw(win_next.w.w,1,1,"%s", "next figure:3");
    wrefresh(win_next.w.w);
  }  
}

extern void move_down(figure_t *f, board_t *p_b){
  print_ground(p_b);
}


void test_shape_definition(){
  for(int i = 0; i < FIGURE_NUMBER; i++){
    for(int j = 0; j < 4; j++){
      printf("[%2d, %2d]", shape[i][j].x, shape[i][j].y);
    }
    printf("%s", "\n");
  }
}


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

bool is_time_to_step(clock_t t1, unsigned intv){
  mvwprintw(win_score.w.w,4,1,"%u  %u",t1, clock());
  wrefresh(win_score.w.w);
  if( (clock() - t1) > (CLOCKS_PER_SEC/1000) * intv ) {
    mvwprintw(win_score.w.w,5,1,"%s","time past");
    wrefresh(win_score.w.w);
    return true;
  }
  return false;
}

void main_loop(board_t *p_board, coord_t *p_p1){

  int16_t key;
  //uint8_t keyH;
  //uint8_t keyL;
  char akey[4] = {0,0,0,0};
  bool finishDrawinFigures = false;

  clock_t time_start;
  unsigned interval = 100; //ms

  figure_t *f = NULL;  
  figure_shape_name_t fs = 0;

  fs = rand()%19 + 1;
  current_figure = NULL;
  if(! (current_figure = create_figure(fs, rand()%REF_BOX_V_MAX, p_p1))) exit(-1);

  fs = rand()%19 + 1;
  next_figure = NULL;
  if(! (next_figure = create_figure(fs, rand()%REF_BOX_V_MAX, p_p1))) exit(-1);
  mvwprintw(win_next.w.w,1,1,"%s", "next figure:1");
  print_figure(&win_next.w, next_figure);
  
  f = current_figure;
  print_figure(&p_board->w, f);
  
  mvwprintw(win_hint.w.w, 1, 1,"%s","[ESC] or [q] => exit,  [c] => clear");
  mvwprintw(win_hint.w.w, 2, 1,"%s","[<-] move left,     [->] move right");
  mvwprintw(win_hint.w.w, 3, 1,"%s","[space]  drop down,  [n] new figure");
  //wrefresh(win_hint.w.w);
  time_start = clock();
  while(!finishDrawinFigures){
    
    if(is_time_to_step(time_start, interval)){
      unprint_figure(&p_board->w, f);
      step_down(f,p_board);
      f = current_figure; //potential NULL in step_down/dissablemble_figure
      print_figure(&p_board->w, f);
      wrefresh(p_board->w.w);
      refresh();
      time_start = clock();
    }
    
    mvwprintw(win_hint.w.w, 4, 1,"%s","catching cursor here");
    wrefresh(win_hint.w.w);
    key = 0;
    key = getch();
    //keyH = key >> 8;
    //keyL = (uint8_t) key;
    akey[0] = key;
    print_info(2,1,"%c", akey);
    switch(key){
      case 27: //ESC
      case 'q':
        finishDrawinFigures = true;
        break;

      case 'c':   /****   debug section *********/
        
        clear();   //clear border definition of windows
        // wrefresh(p_board->w.w);  //not drawing window again
        // wrefresh(win_next.w.w);  //there is necessity call wborder(...) again
        // wrefresh(win_score.w.w);
        // wrefresh(win_hint.w.w);
        // refresh();
        mvwprintw(win_hint.w.w, 1, 1,"%s","[ESC] or [q] => exit,  [c] => clear");
        mvwprintw(win_hint.w.w, 2, 1,"%s","[<-] move left,     [->] move right");
        mvwprintw(win_hint.w.w, 3, 1,"%s","[space]  drop down,  [n] new figure");
        wrefresh(win_hint.w.w);
        unprint_figure(&p_board->w, f);        
        print_figure(&p_board->w, f);
        break;

      case 'n':  /******  debug section **********/
        unprint_figure(&p_board->w, current_figure);        
        unprint_figure(&win_next.w, next_figure);            

        free(current_figure);                
        current_figure = NULL;
        current_figure = next_figure;
        
        next_figure = NULL;                
        fs = rand()%19 + 1;
        if(! (next_figure = create_figure(fs, rand()%REF_BOX_V_MAX, p_p1))) exit(-1);      
        mvwprintw(win_next.w.w,1,1,"%s", "next figure:2");
        print_figure(&win_next.w, next_figure);
        
        f = current_figure;                
        print_figure(&p_board->w, f);

        break;


      case KEY_LEFT:
      case 'l':        
        unprint_figure(&p_board->w, f);
        move_left(p_board, f);
        print_figure(&p_board->w, f);
        break;
      case KEY_RIGHT:
      case 'r':        
        unprint_figure(&p_board->w, f);
        move_right(p_board, f);
        print_figure(&p_board->w, f);
        break;
      case KEY_UP:        
        unprint_figure(&p_board->w, f);      
        turn_left(f, p_board);
        print_figure(&p_board->w, f);
        break;
      case KEY_DOWN:        
        unprint_figure(&p_board->w, f);
        turn_right(f, p_board);
        print_figure(&p_board->w, f);
        break; 
      case ' ':
        move_down(f, p_board);
        
        break;
      //default:         
       // break;      
    }//switch

  }//while
  
  
}//test_draw_shape()

void create_window_frame(gm_window_t *p_gm, unsigned h, unsigned w, unsigned ty, unsigned lx){
  p_gm->left_x = lx;
  p_gm->top_y = ty;
  p_gm->height = h;
  p_gm->width = w;
  p_gm->w = newwin(p_gm->height, p_gm->width, p_gm->top_y, p_gm->left_x);
  wborder(p_gm->w, '|','|','-','-','+','+','+','+');
  //box(board.w, 0, 0);
  wrefresh(p_gm->w);
}



int main(int argc, char **argv){
  int i = 0;
  setlocale(LC_ALL, "");  
  initscr();
  timeout(0); //0 - noblokgin -1 - blocking; >0 waith ms and err
  //nodelay(stdscr, TRUE);
  
  raw();
  keypad(stdscr, TRUE);
  noecho();
  refresh();   //must be before newwin() to order to drwain that newwindow
  srand(time(NULL));
  
  //test_print_figures();
  
  int odp_key = 0;
  
  /********  creating windows  **************/
  board_t board;
  create_window_frame(&board.w, BORAD_HIGHT, BOARD_WIDTH, 0, 0);
  nodelay(board.w.w, TRUE);
  board.left_x = board.w.left_x;
  board.right_x = board.w.left_x + board.w.width;        
  for(i = 1; i < BOARD_WIDTH - 1; i++){
    board.ground_level[i] = BORAD_HIGHT - 2;
  }
  int j = 0;
  for(i = 1; i < BORAD_HIGHT - 1; i++)
    for(j = 1; j < BOARD_WIDTH - 1; j++){
      board.b[i][j].p = ' ';
      board.b[i][j].c = 0;
    }
  
  //global:coord_t p1;  //point of createing new figure
  p1.x = board.w.width / 2;
  p1.y = board.w.top_y + 5;

  /***** next   ******/
  create_window_frame(&win_next.w, 
    BORAD_HIGHT / 3, COLS - BOARD_WIDTH, 0 * (BORAD_HIGHT / 3), BOARD_WIDTH + 1);   

  /***** score  ******/
  create_window_frame(&win_score.w, 
    BORAD_HIGHT / 3, COLS - BOARD_WIDTH, 1 * (BORAD_HIGHT / 3), BOARD_WIDTH + 1);
  
  wnd_info = win_score.w.w;

  /***** hint   ******/  
  create_window_frame(&win_hint.w, 
    BORAD_HIGHT / 3, COLS - BOARD_WIDTH, 2 * (BORAD_HIGHT / 3), BOARD_WIDTH + 1);
  
  refresh();

  /***********  main loop  ***************/
  //test_shape_definition();  
  main_loop(&board, &p1);

  delwin(board.w.w);
  endwin();

  return 0;
}


// /**
//  * @brief 
//  * 
//  * @param f 
//  */
// extern void flip_horizontally(figure_t *f) {
//   //TODO: wrong
//   int i = 0;
//   unsigned horiz_revrse_x[4];
//   for(i = 0; i < 4; i++){
//     mvprintw(6 + i, 2,"c.x[%d] c,y[%d]", f->box[i]->c.x, f->box[i]->c.y);        
//     horiz_revrse_x[3 - i] = f->box[i]->c.x;
//   }
//   for(i = 0; i < 4; i++){
//     f->box[i]->c.x = horiz_revrse_x[i];
//     mvprintw(6 + i, 15,"c.x[%d] c,y[%d]", f->box[i]->c.x, f->box[i]->c.y);        
//   }        
//   determine_LR_edge(f);
// }

// extern void flip_vertically(figure_t *f){
//   //TODO: wrong
//   int i = 0;  
//   unsigned vertical_revrse_y[4];
//   for(i = 0; i < 4; i++){
//     mvprintw(6 + i, 2,"c.x[%d] c,y[%d]", f->box[i]->c.x, f->box[i]->c.y);        
//     vertical_revrse_y[3 - i] = f->box[i]->c.y;
//   }
//   for(i = 0; i < 4; i++){
//     f->box[i]->c.y = vertical_revrse_y[i];
//     mvprintw(6 + i, 15,"c.x[%d] c,y[%d]", f->box[i]->c.x, f->box[i]->c.y);        
//   }        
//   determine_LR_edge(f);
// }

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
