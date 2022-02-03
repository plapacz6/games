#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include "init_UI.h"
#include "game_logic.h"
#include "game_UI.h"
#include "snake_logic.h"
#include "apples.h"

//game global objects
extern screen_t screen;
graphic_symbol_t graphic_symbol;

//game interface
int nr_ruchu = 0;

//funkction definitions
void init_globals_2(){
  graphic_symbol.board_border = "##==##]["; //LU,LD.L,D,RU,RD,L,R
  graphic_symbol.collision = "O*@!?%A";
  graphic_symbol.snake_body = "Oo ";
  graphic_symbol.apple = "aAX ";
  graphic_symbol.ufo = "=";
}

void UI_refresh(){
  #ifdef DEBUG
  extern game_time_control_t g_time;
  extern game_cfg_t game_cfg;
  mvprintw(screen.ymax, 2, "speed = [%i], time_unit = [%i], snake_max_len = [%i] ",
    game_cfg.speed, g_time.time_unit ,game_cfg.snake_max_length[0]);
  #endif //DEBUG
  //move the cursor to a hardly visible place
  move(0,0);
  refresh();
}

void draw_board(){
  int i;
  clear();
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
  //pionowe granice
  for(i = screen.ymin + 1; i < screen.ymax; i++){
    mvaddch(i, screen.xmin,
        graphic_symbol.board_border[IDX_BOARD_L]);
    mvaddch(i, screen.xmax,
        graphic_symbol.board_border[IDX_BOARD_R]);
  }
  //refresh();  //ncurses print to the real screen
}

extern apples_t apples;
void draw_apples(){
  size_t i;
  for(i = 0; i < apples.len; i++){
    mvaddch(
      apples.a[i].y,
      apples.a[i].x,
      graphic_symbol.apple[IDX_APPLE_NO1]
      );
  }
  //refresh();
}

void draw_apples_info(){
  int i;
  for(i = 0; i < apples.len; i++){
    //display order: x, y
    mvprintw(screen.ymin, screen.xmin + 5, " apple = [%i, %i] ", apples.a[i].x, apples.a[i].y);
  }
  //refresh();
}
void draw_snake_info(snake_t *psn){
    char *format_tekst = " snake length = [%i] ";
    mvprintw(screen.ymin, screen.xmax - (strlen(format_tekst) + 4), format_tekst, psn->length);
}

void clean_apples(){
  size_t i;
  for(i = 0; i < apples.len; i++){
    mvaddch(
      apples.a[i].y,
      apples.a[i].x,
      graphic_symbol.apple[IDX_APPLE_CLEAN]
      );
  }
  //refresh();
}

void draw_snake(snake_t* ps, unsigned snake_head_symbol){
  /* snake_head_symbol is place of possible collision. */
  int i;
  mvaddch(ps->y[0], ps->x[0], graphic_symbol.collision
              [snake_head_symbol]);
  for(i = 1; i < ps->length; i++){
      mvaddch(ps->y[i], ps->x[i], graphic_symbol.snake_body
          [IDX_SNAKE_BODY]);
  }
  mvaddch(ps->y[i], ps->x[i], graphic_symbol.snake_body
          [IDX_SNAKE_ERASE]);
  //refresh();
}


void draw_cfg(size_t *pspeed, size_t *psnake_max_length){
  size_t value;
  char choice;
  clear();
  draw_board();
  refresh();
  mvprintw(2, 4, "enter snake's speed (from 1 to 5)");
  do {
    choice = getch();
  } while(choice != '1' && choice != '2' && choice != '3' && choice != '4'&& choice != '5');
  switch(choice){
    case '1':
      value = 1;
      break;
    case '2':
      value = 2;
      break;
    case '3':
      value = 3;
      break;
    case '4':
      value = 4;
      break;
    case '5':
      value = 5;
      break;
    default:
      value = 1;
      break;
  };
  if(value < 0 || value > 5) value = 1;
  *pspeed = value;


  mvprintw(2, 4, "enter snake's max length (1=100, 2 = 200, 5 = 500, 9 = 999)");
  refresh();
  do {
    choice = getch();
  } while(choice != '1' && choice != '2' && choice != '5' && choice != '9');

  switch(choice){
    case '1':
      value = 100;
      break;
    case '2':
      value = 200;
      break;
    case '5':
      value = 500;
      break;
    case '9':
      value = 999;
      break;
    default:
      value = 1;
      break;
  };
  if(value < 1 || value > 999) value = 1;
  *psnake_max_length = value;

  clear();
  refresh();
}

//===============================================================
#ifdef DEBUG
void PRINT_DEBUG(snake_t *psn, int key){

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
}
#endif
