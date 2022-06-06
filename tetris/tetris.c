/* author: plapacz6@gmail.com, date: 2022-02-24 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <locale.h>
#include <stdbool.h>
#include <time.h>

#include <pthread.h>

#include <ncurses.h>
#include "control_version.h"
#include "tetris.h"

WINDOW *wnd_info = NULL; /*DEBUG*/
next_figure_window_t win_next;
score_window_t win_score;  
hint_window_t win_hint;

board_t board;
figure_t current_figure;
figure_t next_figure;
figure_t symulate_figure;
coord_t p1;  //point of creating new figure
coord_t p2;  //point of creating next figure

uint32_t color_attribute = 0;
uint32_t color_bw = 0;

char *error_msg = {
    "create figure: cant allocate memory for ...",    
};

game_state_t game_state;

game_cfg_t game_cfg;


/*DEBUG*/
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
  int color_nr = 1;
  uint32_t color_attr = 1;
  for(i = 0; i < 4; i++){
    assert(f->box[i].bg_filled == false);
    print_info(3, 1, "%s", "print box_t"); /*DEBUG*/
    
    wmove(p_w->w, f->box[i].c.y, f->box[i].c.x);
    f->box[i].bg = inch();  //archiving background
    //wattr_on(stdscr,((((chtype)((((f->box[i].v.c)))) << ((0) + 8)) & ((chtype)((((1UL) << 8) - 1UL)) << ((0) + 8)))),());
    color_nr = (f->box[i].v.c);
    (color_attr = color_bw ) || (color_attr = COLOR_PAIR( color_nr ) );
    wattron(p_w->w, color_attr);
    //mvwprintw(p_w->w, 5,1,"TEKST KONTROLNY: %10.x %s", color_attr, print_bin(color_attr));
    
    mvwprintw(p_w->w, 
      f->box[i].c.y, 
      f->box[i].c.x, 
      "%c", f->box[i].v.p);    
    wattroff(p_w->w, color_attr);      
    f->box[i].bg_filled = true;
  }    
  
  wrefresh(p_w->w);
}


/**
 * @brief put back badground after print_figure rewrite it
 * Not require previous call print_figure, to copy bacdround content to reconstruct
 * In such case, nothing is done.
 * @param f 
 * @param pb 
 */
void unprint_figure(gm_window_t *p_w, figure_t *f){
  size_t i = 0;
  for(i = 0; i < 4; i++){    
    if(f->box[i].bg_filled){
      print_info(3, 1, "%s", "un_print box");
    
      //restoring backgound
      mvwprintw(
        p_w->w,
        f->box[i].c.y, 
        f->box[i].c.x, 
        "%c", f->box[i].bg);
      f->box[i].bg = 0;
      f->box[i].bg_filled = false;        
    }    
    wrefresh(p_w->w);
  }
}

/**
 * @brief redrawing entire boread (use only before first call print_figure
 * 
 * print_figure() fill a bufor for storing piece of backdound, and its value will be unactual
 * after call print_board()
 * 
 * @param p_b 
 */
void print_board(){  
  int color_nr = 1;
  uint32_t color_attr = 1;  
  int i = 0; 
  int j = 0;
  for(i = 1; i < BORAD_HIGHT - 1; i++)
    for(j = 1; j < BOARD_WIDTH -1; j++){
      color_nr = board.b[i][j].c;
      (color_attr = color_bw ) || (color_attr = COLOR_PAIR( color_nr ) );
      wattron(board.w.w, color_attr);      
      mvwprintw(board.w.w, i, j, "%c", board.b[i][j].p);
      wattroff(board.w.w, color_attr);      
    }
  wrefresh(board.w.w);
}


/**
 * @brief 
 * first coordynats are always 0,0  and relatively to them are sepecified others coordynates
 * so, maybe first coordinates are unnececery.
 */

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

box_appearc_t ref_box_v[REF_BOX_A_MAX] = {
  {'X',2},{'x',3},{'*',4},{'o',5},{'#',6},
  //{ACS_BLOCK,1},{ACS_BOARD,1},{ACS_BOARD,1}
};

box_appearc_t ref_board_empty_field = {
  ' ', 0
};

bool is_board_field_empty(coord_t *p_c) {
  if( board.b[p_c->y][p_c->x].p == ref_board_empty_field.p ){
    return true;
  }
  return false;
}

/**
 * @brief find coord's y-part  1 before current ground level
 * 
 * @param p_c 
 */
unsigned find_ground_y_coord(coord_t *p_c){
  int i = 0;
  for(i = p_c->y; i < BORAD_HIGHT; i++){
    if( board.b[i][p_c->x].p != ref_board_empty_field.p){
      break;
    }
  }  
  return i - 1;
}

/**
 * @brief find coords of figure that are higher (less y part) in all figure's box'es highst
 * for move_down()
 * @param p_f 
 */
void find_ground_figure_coord(figure_t *p_f){
  int i = 0;
  unsigned min_y = 0;
  unsigned prev_min_y = 0;
  for(i = 0; i < 4; i++){
    min_y = find_ground_y_coord(& p_f->box[i].c);
    if(min_y < prev_min_y)
      prev_min_y = min_y;
  }
  /* TODO ..........................  */
  return;
}



/**
 * @brief checking if figure is in direct neibourhood with actual ground level 
 * 
 * @param f - figure
 * @param b - board
 * @return true 
 * @return false 
 */
bool is_bottom_contact(figure_t *f, board_t *b){
  mvwprintw(win_score.w.w, 6,1,"%s","is_bottom_contact");
  wrefresh(win_score.w.w);
  int i = 0;  
  for(i = 0; i < 4; ++i) {
    /* 
      TRUE if ground_level is of 1 below of current figure's y
      so they are stick, but not cover
      -1 insted of +1 because y grow to the bottom
      greater y is lower on the screen
    */   
    if( (b->b[f->box[i].c.y + 1][f->box[i].c.x]).p 
          != ref_board_empty_field.p 
        ||
          //f->box[i].c.y == (BORAD_HIGHT - 2)   //c.y == 0 -> y:1
          /* DEBUG vvv */
          f->box[i].c.y >= (BORAD_HIGHT - 2)
        ){
      return true;
      /*
      board field's content differ from screen content, so border line
      of board '-' isn't substitute of not empty filed
      */
    }
  }
  return false;
}


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
    if(fg->xl > fg->box[i].c.x) fg->xl = fg->box[i].c.x;
    if(fg->xr < fg->box[i].c.x) fg->xr = fg->box[i].c.x;    
  }
  return 1;
} /***************  complete ********************/


/**
 * @brief set shape of figure
 * 
 * @param fg - pointer to figure_t
 * @param fs - index in figure shape array
 * @param bx - index in figure style array
 * @return int 1 == no error
 */
int define_shape_figure(figure_t *fg, figure_shape_name_t fs, unsigned bx) {
  int i = 0;
  fg->fs = fs;
  for(i = 0; i < 4; i++){
    fg->box[i].c.x = shape[fs][i].x + fg->p0.x;
    fg->box[i].c.y = shape[fs][i].y + fg->p0.y;
    assert(!fg->box[i].bg_filled);
    fg->box[i].bg = 0;
    fg->box[i].bg_filled = false;
    fg->box[i].v.p = ref_box_v[bx].p;
    fg->box[i].v.c = ref_box_v[bx].c;
  }
  determine_LR_edge(fg);
  return 1;    
} /***************  complete ********************/



/**
 * @brief Create a figure object (alocate memory)
 * To use at preparation environment of game, memory remain the same, but content of figures is changing 
 * @param fg - pointer to figure_t
 * @param fs - shape of figure (index in figure shape table)
 * @param bx - index in figure style array
 * @param p_p1 - pointer to beggining point on board
 * @return 1 - ok
 */
int create_figure(figure_t *fg, figure_shape_name_t fs, unsigned bx, coord_t *p_p1){
  assert(bx < REF_BOX_A_MAX); 
  
  fg->p0.x = p_p1->x;
  fg->p0.y = p_p1->y;
  
  fg->xl = p_p1->x;
  fg->xr = p_p1->x;
  fg->bx = bx;
    
  define_shape_figure(fg, fs, bx);  
    
  return 1;
}



/**
 * @brief dissasemble figure and make it part of gound
 * 
 * color and pattern of boxes, are copied to the fileds of borad. 
 * 
 * Before call use unprint_figure(&win_next, &next_figure).* 
 * @param f - figure, (remain untached, so figure cover part of ground after this function call)
 * @param b - board
 */
void diassemble_figure(figure_t *f, board_t *b){
  assert(f->box[0].bg_filled == false);

  mvwprintw(win_score.w.w, 6,1,"%s","disassemble figure");
  wrefresh(win_score.w.w);

  int i = 0;  
  for(i = 0; i < 4; i++){
    b->b[f->box[i].c.y][f->box[i].c.x].p = f->box[i].v.p;  
    b->b[f->box[i].c.y][f->box[i].c.x].c = f->box[i].v.c;            
  }  
}



/**
 * @brief generate next figure
 * Before call use unprint_figure(&win_next, &next_figure).
 * @param f 
 * @param p_p1 - coordinates of generated figure
 */
void generate_next_figure(figure_t *f, coord_t *p_p1){
  assert(f->box[0].bg_filled == false);

  figure_shape_name_t fs = (rand()%(FIGURE_NUMBER - 1)) + 1;  
  
  create_figure(&next_figure, fs, rand()%REF_BOX_A_MAX, p_p1);    
}



/**
 * @brief copy content of f1 to f2
 * Both figures must be unprintf before copying.
 * @param f2 destination
 * @param f1 source
 * @param p_p1 pointer to coord_t where f2 will be placed
 */
void copy_figure(figure_t *f2, figure_t *f1, coord_t *p_p1){
  int i = 0;  
  for(i = 0; i < 4; i++){
    f2->box[i].c.x  = f1->box[i].c.x;
    f2->box[i].c.y  = f1->box[i].c.y;
    f2->box[i].v.p  = f1->box[i].v.p;
    f2->box[i].v.c  = f1->box[i].v.c;
    f2->box[i].bg_filled = false;
  }
  f2->fs = f1->fs;
  f2->p0.x = p_p1->x;// f1->p0;
  f2->p0.y = p_p1->y;
  f2->bx = f1->bx;
  f2->xl = f1->xl;
  f2->xr = f1->xr;
}



typedef enum figure_status_position_tt {
  F_STATUS_POSITION_OK = 0b00000000,
  F_STATUS_POSITION_LEFT_B = 0b00000001,
  F_STATUS_POSITION_RIGHT_B = 0b00000010,
  F_STATUS_POSITION_G = 0b00000100,  
} figure_status_position_t;

figure_status_position_t is_cover_LR(figure_t *f){
  figure_status_position_t status = F_STATUS_POSITION_OK;
  int i = 0;
  for(i = 0; i < 4; i++){
    /* cover border of board */
    if(f->box[i].c.x == board.left_x) 
      status += F_STATUS_POSITION_LEFT_B;
    if(f->box[i].c.x == board.right_x) 
      status += F_STATUS_POSITION_RIGHT_B;
    /* cover ground element */
    if( board.b[f->box[i].c.y][f->box[i].c.x].p 
          != ref_board_empty_field.p 
        ||
          f->box[i].c.y >= (BORAD_HIGHT - 1)
        ){
      status += F_STATUS_POSITION_G;
    }
  }  
  return status;  
}



/**
 * @brief cheking if turn on current position is possible
 * If not, trying to correct slitly current position and turn again
 * @param f 
 * @param p_b 
 * @param ns 
 */
void turn_figure(figure_t *f, board_t *p_b, figure_shape_name_t ns) {
  unprint_figure(&p_b->w, f);
  bool turn_ok = false;
  bool turn_not_possible = false;
  figure_status_position_t curr_status = F_STATUS_POSITION_OK;

  copy_figure(&symulate_figure, f, &(f->p0));
  do{    
    define_shape_figure(&symulate_figure, ns, symulate_figure.bx);
    
    curr_status = is_cover_LR(&symulate_figure);
    switch (curr_status) {      
    case F_STATUS_POSITION_G:
    case (F_STATUS_POSITION_LEFT_B | F_STATUS_POSITION_RIGHT_B) :    
    case ( (F_STATUS_POSITION_LEFT_B | F_STATUS_POSITION_RIGHT_B) | F_STATUS_POSITION_G) :
      turn_not_possible = true; 
      break;
    case F_STATUS_POSITION_OK:
      turn_ok = true;
      copy_figure(f, &symulate_figure, &symulate_figure.p0);
      break;
    case F_STATUS_POSITION_LEFT_B:      
      symulate_figure.p0.x++;
      break;
    case F_STATUS_POSITION_RIGHT_B:      
      symulate_figure.p0.x--;
      break;
    }
  } while(! (turn_ok || turn_not_possible));
  print_figure(&p_b->w, f);
}



/**
 * @brief move figure right
 * 
 * @param p_b 
 * @param f 
 */
void move_right(board_t *p_b, figure_t *f){ 
  unprint_figure(&p_b->w, f);
  int i = 0;  
  print_info(5,1, "%s", "moving RIGHT");
  if( f->xr < (p_b->right_x)){// - 1) ){      
    for(i = 0; i < 4; i++){
      /*
      TODO:
      case :
                 **  ***
                 *** *  <- place where it shoud be possible move left
                 *****
      it demand storing not only ground line but 
      whole shape of ground (x and y)
      */
      /* 
      y is near bottom when is greater 
      xr+1 is not > board.right_x, because check in if^^^
      */
      if(board.b[f->box[i].c.y][f->box[i].c.x].p != ref_board_empty_field.p) {
        return;
      }
    }  
    for(i = 0; i < 4; i++){    
      f->box[i].c.x++;
    }
    f->p0.x++;    
    f->xr++;
    f->xl++;
  }
  print_figure(&p_b->w, f);
}



/**
 * @brief move figure left
 * 
 * @param p_b 
 * @param f 
 */
void move_left(board_t *p_b, figure_t *f){ 
  unprint_figure(&p_b->w, f);
  int i = 0;  
  print_info(5,1, "%s", "moving LEFT");
  if(f->xl > (p_b->left_x)){// + 1)) {
    for(i = 0; i < 4; i++){
      /* 
      y is near bottom when is greater 
      xr-1 is not < board.left_x, because check in if^^^
      */
      if(board.b[f->box[i].c.y][f->box[i].c.x].p != ref_board_empty_field.p) {
        return;
      }
    }      
    for(i = 0; i < 4; i++){    
      f->box[i].c.x--;
    }
    f->p0.x--;
    f->xr--;
    f->xl--;
  }
  print_figure(&p_b->w, f);
}



/**
 * @brief create initial state of curren and next figures
 * 
 * @param f 
 * @param p_b 
 */
void prepare_1figure_to_start(figure_t *f, board_t *p_b){     
    unprint_figure(&p_b->w, f);
    unprint_figure(&win_next.w, &next_figure);    

    generate_next_figure(&next_figure, &p1);   
    copy_figure(&current_figure, &next_figure, &p1);

    generate_next_figure(&next_figure, &p2);   
    print_figure(&win_next.w, &next_figure);
    mvwprintw(win_next.w.w,1,1,"%s", "next figure:1");
    wrefresh(win_next.w.w);

    print_figure(&board.w, f);
}



/**
 * @brief make one step down of figure's position
 * This is controled by time passing.
 * @param f 
 * @param p_b 
 */
void step_down(figure_t *f, board_t *p_b){
  unprint_figure(&p_b->w, f);
  /* make one step down */  
  int i = 0;
  f->p0.y++;
  for(i = 0; i < 4; i++) {
    f->box[i].c.y++;
  }
  /* DEBUG */
  if(f->p0.y >= BORAD_HIGHT - 2) {
    mvwprintw(p_b->w.w, BORAD_HIGHT, 1, "bottom reached");
    wrefresh(p_b->w.w);
  }
  assert(f->p0.y < (BORAD_HIGHT) );

  /* check if now is bottom contact with ground 
     (remnant of one of previous figure, or border of board) */
  if(is_bottom_contact(f, p_b)){

    diassemble_figure(f, p_b);   
    /* f still exist, copy figure will destroy it soon */
    unprint_figure(&win_next.w, &next_figure);
    copy_figure(f, &next_figure, &p1);
    generate_next_figure(&next_figure, &p2);       
    print_figure(&win_next.w, &next_figure);    
    mvwprintw(win_next.w.w,1,1,"%s", "next figure:1");
    wrefresh(win_next.w.w);
    
    print_board();              
  } 
  else {
    print_figure(&p_b->w, f);
  }
}



/**
 * @brief moving figure to the ground
 * After pressing 'send figure do the ground' key.
 * @param f 
 * @param p_b 
 */
extern void move_down(figure_t *f, board_t *p_b){
  /*
  for(i = 0; i < 4; i++){
    p_b->ground_level[f->box[i].c.x]
    f->box[i].c.y = ...
  }

  //TODO
  */
  print_board();
}



/**
 * @brief Create a window frame object
 * 
 * @param p_gm 
 * @param h 
 * @param w 
 * @param ty 
 * @param lx 
 */
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



/**
 * @brief restoring beginning content of board's fileds.
 * 
 */
void board_reset(){
  int i = 0;
  int j = 0;  
  for(i = 1; i < BORAD_HIGHT - 1; i++){
    for(j = 1; j < BOARD_WIDTH - 1; j++){
      board.b[i][j].p = ref_board_empty_field.p;
      board.b[i][j].c = ref_board_empty_field.c;
    }  
    /* contact with board boarder is detected in 
    is_bottom_contact function by checking coordinates
    instead of checking filds content 
    */    
  }
}



int main(int argc, char **argv){

  /* init tools */
  setlocale(LC_ALL, ""); //for unicode
  initscr();
  if(has_colors()){
    mvprintw(&win_score.w, 1, 1, "terminal has colors");
    start_color();
    if(can_change_color()){
      mvprintw(&win_score.w, 2, 1, "terminal can change colors");
      init_color(COLOR_BLACK, 10,10,10);
      init_color(COLOR_WHITE, 900,900,900);

      init_color(COLOR_RED, 700,0,0);
      init_color(COLOR_GREEN, 0,1000,400);     
      init_color(COLOR_YELLOW, 500, 500, 0);

      init_color(COLOR_BLUE, 0,0,1000);            
      init_color(COLOR_CYAN, 50,0,1000);      
      init_color(COLOR_MAGENTA, 500,500,1000);      
    }
    init_pair(1,COLOR_WHITE, COLOR_CYAN);
    init_pair(2,COLOR_WHITE, COLOR_BLUE);
    init_pair(3,COLOR_WHITE, COLOR_MAGENTA);
    init_pair(4,COLOR_WHITE, COLOR_GREEN);
    init_pair(5,COLOR_WHITE, COLOR_YELLOW);        
    init_pair(6,COLOR_WHITE, COLOR_RED);    
    init_pair(7,COLOR_WHITE, COLOR_BLACK);
    
    color_attribute = 
      COLOR_PAIR(1) | COLOR_PAIR(2) | COLOR_PAIR(3) | 
      COLOR_PAIR(4) | COLOR_PAIR(5) | COLOR_PAIR(6);    
    color_bw = 0;
  }
  else {
    color_attribute = A_REVERSE;
    color_bw = A_REVERSE;    
  }
  wattrset(win_next.w.w, color_attribute);      
  wattrset(board.w.w, color_attribute);    
  //(NCURSES_OK_ADDR(board.w) ? ((board.w.w)->_attrs = NCURSES_CAST(attr_t, color_attribute), OK) : ERR);    
  
  #ifdef DEBUG
  {    
    int color_nr = 6;
    uint32_t color_attr = 0;
    (color_attr = color_bw ) || (color_attr = COLOR_PAIR( color_nr ) );
    //attron(COLOR_PAIR(color_nr));
    attron(color_attr);
    mvprintw(1,1,"TEKST KONTROLNY: %10.x %s", A_REVERSE, print_bin(A_REVERSE));  
    mvprintw(2,1,"TEKST KONTROLNY: %10.x %s", COLOR_PAIR(2), print_bin(COLOR_PAIR(2)));  
    mvprintw(3,1,"TEKST KONTROLNY: %10.x %s", COLOR_PAIR(4), print_bin(COLOR_PAIR(4)));  
    mvprintw(4,1,"TEKST KONTROLNY: %10.x %s", COLOR_PAIR(2) | A_REVERSE , print_bin(COLOR_PAIR(2) | A_REVERSE ));  
    mvprintw(4,1,"TEKST KONTROLNY: %10.x %s", COLOR_PAIR(2) |COLOR_PAIR(4) | A_REVERSE , print_bin(COLOR_PAIR(2) |COLOR_PAIR(4) | A_REVERSE ));  
    
    mvprintw(5,1,"TEKST KONTROLNY: %10.x %s", color_attr, print_bin(color_attr));
    //attroff(COLOR_PAIR(color_nr));
    attron(color_attr);
    //getch();  
    //endwin();
    //return 0;
  }
  #endif 

  #ifdef CONTROL_LOOP
  timeout(0); //0 - noblokgin -1 - blocking; >0 waith ms and err
  //nodelay(stdscr, TRUE);
  #endif //CONTROL_LOOP
  #ifdef CONTROL_THREAD
  timeout(1); //0 - noblokgin -1 - blocking; >0 waith ms and err
  #endif //CONTROL_THREAD
  #ifdef CONTROL_SIGNAL
  #endif //CONTROL_SIGNAL
  

  //raw();
  cbreak();

  keypad(stdscr, TRUE);
  noecho();
  curs_set(0);  //invisible cursor
  refresh();   //must be before newwin() to order to drwain that newwindow
  
  srand(time(NULL));
  
  //test_print_figures();
  



  /************** inicjalize game components ********** */
  game_state.intv_stepdown = 1;  // 1 sek;
  //game_cfg. ...
  
  
  /************ board initialization  **************/
  create_window_frame(&board.w, BORAD_HIGHT, BOARD_WIDTH, 0, 0);
  nodelay(board.w.w, TRUE);
  board.left_x = board.w.left_x;
  board.right_x = board.w.left_x + board.w.width - 2; //-1 -> cover right border       

  board_reset();

  //global:coord_t p1;  //point of createing new figure
  p1.x = board.w.width / 2;
  p1.y = board.w.top_y + 4;
  p2.x = board.w.width / 2;
  p2.y = 4;



  /********  creating windows  **************/
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
  
  #ifdef CONTROL_LOOP
  user_control();
  time_control();
  #endif //CONTROL_LOOP

  #ifdef CONTROL_THREAD
  pthread_t th_user_control;
  create_thread(&th_user_control, NULL, user_control, NULL);
  pthread_t th_time_control;
  create_thread(&th_time_control, NULL, time_control, NULL);

  pthread_join(th_user_control, NULL);
  pthread_join(th_time_control, NULL);
  #endif // CONTROL_THREAD

  #ifdef CONTROL_SIGNAL
  #endif //CONTROL_SIGNAL

  delwin(board.w.w);
  endwin();

  return 0;
}
/* =================================================================== */


/**
 * @brief turn figure left
 * 
 * @param f 
 * @param p_b 
 */
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
 turn_figure(f, p_b, ns);  
}



/**
 * @brief turn figure right
 * 
 * @param f 
 * @param p_b 
 */
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
  turn_figure(f, p_b, ns);    
}
