/*
 * author:plapacz6@gmail.com
 * licence:GPLv3
 * data:2020-03-30
 * ver:0.0.1
 *
 * game snake from scratch for linux terminal
 * c89
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "snake_types.h"
#include <stdbool.h>
#include <ncurses.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>

//typedef enum {ms_menu,ms_game, ms_cfg, ms_end} menu_state_t;
//typedef enum {gs_beginning,gs_pausa, gs_go, gs_end} game_state_t;
//typedef struct {
//  int board;  //board: empty or various obstacles
//  int speed;
//  //int incresing_speed; //speed increases automaticaly during game
//  //during level or after level's finishing
//  //int snake_growth; //snake is growing after eatign someting
//  //int number_of_snakes;
//} state_of_game_t;
//
///* board definition */
//typedef struct {
//} board_t;
///* state of sanke */
//typedef struct {
//} snake_t;
//state_of_game_t state_of_game;

#define MAX_NUMBER_APPLES (10)
#define MAX_LEN_SNAKE (10)

typedef struct {
  int dx;  //step in x asix
  int dy;
  int x[MAX_LEN_SNAKE];
 int y[MAX_LEN_SNAKE];
  int length;
} snake_t;
typedef struct {
  int xmin;  //left upper x
  int xmax;
  int ymin;
  int ymax;
  int xmid; //center of screen x
  int ymid;
} screen_t;
typedef struct {
  int x;
  int y;
} coords_t;
typedef enum {
  NO_COLLISION = 0,
  COLLISION_WITH_BORDER = 1,
  COLLISION_WITH_ONESELF, 
  COLLISION_WITH_WALL, 
  COLLISION_WITH_OTHER_SNAKE,
  COLLISION_WITH_APPLE,        //snake growth +1
} type_of_collision_t;
typedef enum {
  IDX_BOARD_LU = 0,
  IDX_BOARD_LD = 1,
  IDX_BOARD_U = 2,
  IDX_BOARD_D = 3,
  IDX_BOARD_RU = 4,
  IDX_BOARD_RD = 5,
  IDX_BOARD_L = 6,
  IDX_BOARD_R = 7,
  
  IDX_COLLISION_ONESLEF = 0,
  IDX_COLLISION_BORDER = 1,
  IDX_COLLISION_WALL = 2,
  IDX_COLLISION_OTHER_SNAKE = 3,

  IDX_SNAKE_HEAD = 0,
  IDX_SNAKE_BODY = 1,
  IDX_SNAKE_ERASE = 2,
} graphic_symbol_index_t;
typedef struct {
  char* board_border;
  char* collision;
  char* snake_body;
} graphic_symbol_t;
typedef struct {
  int x;
  int y;
} apple_t;
typedef struct {
  apple_t a[MAX_NUMBER_APPLES];
  size_t len;
} apples_t;

//game global objects
screen_t screen;  //game has one screen only
coords_t coords; //uniwersal variable for coordynates of screen
graphic_symbol_t graphic_symbol;
apples_t apples;
apples_t *papples = &apples;


//game time control
//time_t previous_time; //to alter i control speed of snake
struct timeval time_tv;
int prev_time_s;
int prev_time_us;
unsigned long time_unit;

//game interface
int nr_ruchu = 0;
char *animation_frames_STAR = "/-\\|";
size_t animation_size;
//static size_t animation_frame_counter = 0;
coords_t animation_bottom_coords;
coords_t info_bottom_coords;
coords_t info_center_coords;
char *info_empty_space = 
"                                                                            ";

//funkction declarations
menu_state_t show_help(int argc, char** argv);
void init_game(void);
menu_state_t menu1(void);
void cfg_edit(void);
void game_start(state_of_game_t *state_of_game);
bool its_time_to_move();
//void game_move(snake_t *s);
void game_move(snake_t *psn,
    screen_t *psc, 
    coords_t *pc, 
    bool predictive_mode,
    game_state_t *pgame_state);
type_of_collision_t collision_detect(snake_t *psn, screen_t *psc, coords_t *pc);
void snake_move_forward(snake_t *s, coords_t *pc, bool growth);
void draw_board(state_of_game_t *state_of_game);
void draw_snake(snake_t* ps, char snake_head_symbol);
void print_bottom_info(char *str, bool erase);
void print_center_info(char *str, bool erase);
void print_bottom_animation(char *frames);
int TESTUJ_KODY_KLAWISZY();

void init_ncurses();
void init_globals();
void init_snake(snake_t *psn);
int snake_growth_N_and_move_forward_N(snake_t *psn,int n); 
void PRINT_DEBUG(snake_t *psn, int key);


//funkction definitions
void init_ncurses(){
  initscr();
  timeout(3); //-1 - blokowanie getchar(), >0 - bez blokowania
  raw();               //dispable buffering
  keypad(stdscr,TRUE); //enable FunctionKey, Arrows
  noecho();            //echo off
  mvprintw(0, 2, "ncurses gotowe ... COLS == %d, LINES == %d", COLS, LINES);
}

void init_globals(){
  screen.xmin = 0;
  screen.ymin = 0;
  screen.xmax = (COLS - 1); 
  screen.ymax = (LINES - 1);  
  screen.xmid = (COLS - 1) / 2;
  screen.ymid = (LINES - 1) / 2;
  time_unit = 300000; //in micro seconds
  graphic_symbol.board_border = "Xx-_#+][";
  graphic_symbol.collision = "*@!?%";
  graphic_symbol.snake_body = "Oo ";
  info_bottom_coords.x = screen.xmin + 6;
  info_bottom_coords.y = screen.ymax;
  info_center_coords.x = screen.xmid / 2;
  info_center_coords.y = screen.ymid;
  animation_bottom_coords.x = screen.xmin + 2;
  animation_bottom_coords.y = screen.ymax;

  animation_size = strlen(animation_frames_STAR);
  papples->len = 0;
}

void init_snake(snake_t *psn){
  psn->length = 1;
  psn->dx = 0;
  psn->dy = -1;
  psn->x[0] = screen.xmid;
  psn->y[0] = screen.ymid;
  int i;
  for(i = 0; i < MAX_LEN_SNAKE; i++){
    psn->x[i] = screen.xmid;
    psn->y[i] = screen.ymid;
  }
  snake_growth_N_and_move_forward_N(psn, 4);
}

int snake_growth_N_and_move_forward_N(snake_t *psn, int n){ 
  coords.x = psn->x[0];
  coords.y = psn->y[0];  
  int i;
  int growth_impossible = 0;
  for(i = 0; i < n; i++){
    if(psn->length < MAX_LEN_SNAKE){
      coords.x += psn->dx;
      coords.y += psn->dy;
    }
    else{
      growth_impossible++;
    }
    snake_move_forward(psn, &coords, TRUE);
  }
  return growth_impossible;
}

void PRINT_DEBUG(snake_t *psn, int key){
#ifdef DEBUG
    int i;
    for(i = 0; i < MAX_LEN_SNAKE; i++){
      mvprintw(2 + i, 4, 
          "sn.x[%d]: %d, sn.y[%d]: %d",
          i, psn->x[i],
          i, psn->y[i]);
    }
    mvprintw(2 + i++, 4, "key: %c: %d", key, key);
    mvprintw(2 + i, 4,   "sn.dx: %d: sn.dy: %d  sn.lenth: %d", 
        psn->dx, psn->dy, psn->length);
#endif
}

void game_start(state_of_game_t *state_of_game){

  //bool end_game = false;  //assurance of the end of the game
  int key; //key pressed on keyboard
  snake_t snake;
  game_state_t game_state = gs_beginning;
  init_ncurses();
  init_globals();
  init_snake(&snake);
  draw_board(state_of_game);
  draw_snake(&snake, graphic_symbol.snake_body
      [IDX_SNAKE_HEAD]);
  refresh();
  //end of game inictialization

  //previous_time = time(NULL); //start conting time
  gettimeofday(&time_tv, NULL); //start conting time
  prev_time_s = time_tv.tv_sec;
  prev_time_us = time_tv.tv_usec;

  while(gs_end != game_state){
    PRINT_DEBUG(&snake, key);
    switch(game_state){
      case gs_beginning:
        print_center_info(
            "poczatek gry: [SPACJA] = rozpocznij", FALSE); 
        while((key = getch()) != ' '){
        }
        print_center_info("",TRUE);
        game_state = gs_go;
        break;
      case gs_go:
        print_bottom_animation(animation_frames_STAR);
        print_bottom_info(
            "gra ...   - [SPACJA] -> pauza, [STRZAŁKI] - sterowanie", FALSE);
          //reset state variables
        key = 0;

        while(game_state == gs_go){
          key = getch();
          switch(key){           
            case 27: //ESC -> pausa             
              game_state = gs_pausa;
              break;
            case ' ':
              game_state = gs_pausa;
              break;
            case KEY_UP: //up
                snake.dy = -1;          
                snake.dx = 0;
              break;
            case KEY_DOWN: //down 
                snake.dy = +1;             
                snake.dx = 0;
              break;
            case KEY_LEFT: //left
                snake.dy = 0;
                snake.dx = -1;              
              break;
            case KEY_RIGHT: //right
                snake.dy = 0;
                snake.dx = +1;
              break;
            #ifdef TESTY              
            case '*':
              key = TESTUJ_KODY_KLAWISZY();
              break;
            #endif
          }
          game_move(&snake, &screen, &coords, 0, &game_state);

          PRINT_DEBUG(&snake, key);
          
          refresh();
        };
        
        break;
      case gs_pausa:
        print_center_info(
            "[ESC] = koniec gry, [SPACJA] = kontynuuj.", FALSE);
        while((key = getch())){
          if(key == 27){
            game_state = gs_end;
            break;
          }
          if(key == ' '){
            game_state = gs_go;
            break;
          }
        }
        print_center_info("", TRUE);
        break;
      case gs_over:
        print_center_info(
            "GMAE OVER! [ESC] = zakończ, [Y] = spróbuj ponownie.", FALSE);
        while((key = getch())){        
          if(key == 27){
            print_center_info("", TRUE);
            game_state = gs_end;
            break;
          }
          if(key == 'y' || key == 'Y'){
            print_center_info("", TRUE);
            init_snake(&snake);
            game_state = gs_beginning;
            break;
          }
        }
        break;
      case gs_end:
          //end_game = TRUE;
        break;
    }
  }
  clear();
  endwin();            //ncurses end
  return;
}

bool its_time_to_move(){
  /* from <sys/time.h> get time in microseconds */
  gettimeofday(&time_tv, NULL); //start counting time
  int curr_time_s = time_tv.tv_sec;
  int curr_time_us = time_tv.tv_usec;
 
  unsigned long diff_time_us =  curr_time_us - prev_time_us;
  #ifdef DEBUG
  mvprintw(2, 30, "ruch nr: %d upl.czasu.us: %10ld", diff_time_us);
  #endif    
  if(curr_time_s == prev_time_s && diff_time_us < time_unit){
    return false;    
  }
  prev_time_s = curr_time_s;
  prev_time_us = curr_time_us;
  return true;
}

//game make one move of game
/* eksperyment z wydajnoscia gdy niepotrzebnie zmienne 
   sie przekazuje */
//int game_move(snake_t *psn){
void game_move(snake_t *psn,
    screen_t *psc, 
    coords_t *pc, 
    bool predictive_mode,
    game_state_t *pgame_state){
  type_of_collision_t collision_type;
  //checking time period
  if(its_time_to_move()){    
    pc->x = psn->x[0] + psn->dx;
    pc->y = psn->y[0] + psn->dy;    
    collision_type = collision_detect(psn, psc, pc);
    if(predictive_mode == FALSE){
      switch(collision_type){
        case NO_COLLISION:
          snake_move_forward(psn, pc, FALSE);
          draw_snake(psn, graphic_symbol.snake_body
              [IDX_SNAKE_HEAD]);
          break;
        case COLLISION_WITH_BORDER:
          snake_move_forward(psn, pc, FALSE);
          draw_snake(psn, graphic_symbol.collision
              [IDX_COLLISION_BORDER]);
          *pgame_state = gs_over; 
          break;
        case COLLISION_WITH_ONESELF:
          snake_move_forward(psn, pc, FALSE);
          draw_snake(psn, graphic_symbol.collision
              [COLLISION_WITH_ONESELF]);
          *pgame_state = gs_over; 
          break;
        case COLLISION_WITH_WALL:
          snake_move_forward(psn, pc, FALSE);
          draw_snake(psn, graphic_symbol.collision
              [IDX_COLLISION_WALL]);
          *pgame_state = gs_over; 
          break;
        case COLLISION_WITH_OTHER_SNAKE:
          snake_move_forward(psn, pc, FALSE);
          draw_snake(psn, graphic_symbol.collision
              [IDX_COLLISION_OTHER_SNAKE]);
          *pgame_state = gs_over; 
          break;
        case COLLISION_WITH_APPLE:
          snake_growth_N_and_move_forward_N(psn,1); 
          break;
      }
    }
    else {
      //warning: choose other direction
    }
  }  
}

//checking if collision occured and type of collision
type_of_collision_t  collision_detect(
    snake_t *psn, 
    screen_t *psc, 
    coords_t *pc
    ){
  int i = 0;
  /* Checking for collision with itself:
     
     'i; is snake's body index. Head is on i == 0, and head will be 
     moves to coordinates == pc. 

     The head cannot take the place of the tail during the same movement.

     In particular head cannot move to itself coordinates (in case of 
     teleportation for example).
  */
  for(i = 0; i < psn->length; i++){
    if(pc->x == psn->x[i] && pc->y == psn->y[i]){
      return COLLISION_WITH_ONESELF;
    }
  }
  /* Checking for collision with walls:
  */   
  if((pc->x == psc->xmin ||
      pc->y == psc->ymin || 
      pc->x == psc->xmax ||
      pc->y == psc->ymax)){
    return COLLISION_WITH_BORDER;
  }    
  
  //Checking for collision with obstackles:
  //TODO: przeszkody alokowane dynamicznie na stercie
  //i kazda ma wlasne wspolrzedne i dlugosc i ksztalt
  //  return COLLISION_WITH_WALL;

  //Checking for collisions with other snake:
  // return COLLISION_WITH_OTHER_SNAKE;

  /* Checking for collision with apple:
     snake will grow: +1 segment.

     For each apple check if head of snake will hit it.
  */  
  for(i = 0; i < papples->len; i++){
    if(pc->x == papples->a[i].x && pc->y == papples->a[i].y){
      return COLLISION_WITH_APPLE;
    }
  }
  
  return NO_COLLISION;
}

//preparing snake to move
void snake_move_forward(snake_t *ps, coords_t *pc, bool growth){
  int i;
  /*
     Moving snake forward:     
     Changing the coordinates of each part of the snake
     to the coordinates of the part following it,
     and moving head of the snake to the coordinases == pc. 
  */
  if(growth == TRUE){
    if(ps->length < MAX_LEN_SNAKE){
      ps->length++;
    }
  }
  for(i = ps->length; i > 0; i--){
    ps->x[i] = ps->x[i - 1]; 
    ps->y[i] = ps->y[i - 1]; 
  }
  ps->x[0] = pc->x;
  ps->y[0] = pc->y;
}

void draw_board(state_of_game_t *state_of_game){  
  int i;
  //mvprintw(LINES - 1, COLS/2, "rysowanie planszy");
  
  mvaddch(screen.ymin, screen.xmin,
      graphic_symbol.board_border[IDX_BOARD_LU]);
  
  for(i = screen.xmin + 1; i < screen.xmax - 1; i++){
    mvaddch(screen.ymin, i,
        graphic_symbol.board_border[IDX_BOARD_U]);
  }
  mvaddch(screen.ymin, screen.xmax,
      graphic_symbol.board_border[IDX_BOARD_RU]);
  
  mvaddch(screen.ymax, screen.xmin,
      graphic_symbol.board_border[IDX_BOARD_LD]);

  for(i = screen.xmin + 1; i < screen.xmax - 1; i++){
    mvaddch(screen.ymax, i,
        graphic_symbol.board_border[IDX_BOARD_D]);
  }
  
  mvaddch(screen.ymax, screen.xmax,
      graphic_symbol.board_border[IDX_BOARD_RD]);
 
  //TODO: pionowe granice
  for(i = screen.ymin + 1; i < screen.ymax - 1; i++){
    mvaddch(i, screen.xmin,
        graphic_symbol.board_border[IDX_BOARD_L]);
    mvaddch(i, screen.xmax,
        graphic_symbol.board_border[IDX_BOARD_R]);
  }

  refresh();  //ncurses print to the real screen
}

void draw_snake(snake_t* ps, char snake_head_symbol){
  //mvprintw(LINES - 1, 0, "rysowanie snakea");
  /* snake_head_symbol is place of possible collision. */
  int i;
  mvaddch(ps->y[0], ps->x[0], snake_head_symbol);
  for(i = 1; i < ps->length; i++){
      mvaddch(ps->y[i], ps->x[i], graphic_symbol.snake_body
          [IDX_SNAKE_BODY]);
  }
  mvaddch(ps->y[i], ps->x[i], graphic_symbol.snake_body
          [IDX_SNAKE_ERASE]);
  refresh();
}

void print_bottom_animation(char *frames){
    //TODO: nie dziala to
  static size_t animation_frame_counter = 0;
  char frame[5];
  frame[0] = frames[animation_frame_counter];
  frame[1] = '\0';
  mvprintw(animation_bottom_coords.y, animation_bottom_coords.x, 
      frame);
  if(animation_frame_counter < animation_size){
    animation_frame_counter++;
  }
  else{
    animation_frame_counter = 0;
  }
  refresh();
}

void print_bottom_info(char *str, bool erase){
  if(!erase){
    mvprintw(info_bottom_coords.y, info_bottom_coords.x, 
        "[   %s   ]", str);
  }
  else{
    mvprintw(info_bottom_coords.y, info_bottom_coords.x, 
        info_empty_space);
  }
  refresh();
}

void print_center_info(char *str, bool erase){
  if(!erase){
    mvprintw(info_center_coords.y, info_bottom_coords.x, 
      ">>> %s <<<", str);
  }
  else{
    mvprintw(info_center_coords.y, info_bottom_coords.x, 
        info_empty_space);
  }
  refresh();
}
