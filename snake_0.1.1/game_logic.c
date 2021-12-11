#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include "init_UI.h"
#include "game_logic.h"
#include "game_menu_definition.h"
#include "snake_logic.h"
#include "apples.h"
#include "game_UI.h"


//game global objects
extern screen_t screen;  //init_UI.c
extern menu_t menu1;
extern menu_t menu2;
extern menu_t menu3;
//xxx//coords_t coords; //uniwersal variable for coordynates of n
//xxx//graphic_symbol_t graphic_symbol;
apples_t apples;
apples_t *papples = &apples;

//game time control
game_time_control_t g_time;
game_cfg_t game_cfg;



//funkction definitions
void init_globals_1(){
  papples->len = 1;
  srand(time(NULL));
  game_cfg.speed = 1;
  g_time.time_unit = 300000/game_cfg.speed; //in micro seconds
  game_cfg.number_of_snake = 1;
  game_cfg.snake_max_length[0] = 200<MAX_LEN_SNAKE?200:MAX_LEN_SNAKE;
}

bool its_time_to_move(){
  /* from <sys/time.h> get time in microseconds */
  gettimeofday(&g_time.time_tv, NULL); //start counting time
  int curr_time_s = g_time.time_tv.tv_sec;
  int curr_time_us = g_time.time_tv.tv_usec;

  unsigned long diff_time_us =  curr_time_us - g_time.prev_time_us;
  if(curr_time_s == g_time.prev_time_s && diff_time_us < g_time.time_unit){
    return false;
  }
  g_time.prev_time_s = curr_time_s;
  g_time.prev_time_us = curr_time_us;
  return true;
}


//checking if collision occured and type of collision
type_of_collision_t  collision_detect(
    snake_t *psn,
    coords_t *pc){
  screen_t *psc = &screen;
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

//game make one move of game
void game_move(snake_t *psn, game_state_t *pgame_state){
  coords_t coords;
  coords_t *pc = &coords;
  screen_t *psc = &screen;
  type_of_collision_t collision_type;
  //checking time period
  if(its_time_to_move()){
    pc->x = psn->x[0] + psn->dx;
    pc->y = psn->y[0] + psn->dy;
    collision_type = collision_detect(psn, pc);

    switch(collision_type){
      case NO_COLLISION:
        snake_move_forward(psn, pc, false);
        draw_snake(psn, MODE_COLLISION_NOCOLLISION);
        break;
      case COLLISION_WITH_BORDER:
        snake_move_forward(psn, pc, false);
        draw_snake(psn, MODE_COLLISION_BORDER);
        *pgame_state = gs_over;
        break;
      case COLLISION_WITH_ONESELF:
        snake_move_forward(psn, pc, false);
        draw_snake(psn, MODE_COLLISION_ONESLEF);
        *pgame_state = gs_over;
        break;
      case COLLISION_WITH_WALL:
        snake_move_forward(psn, pc, false);
        draw_snake(psn, MODE_COLLISION_WALL);
        *pgame_state = gs_over;
        break;
      case COLLISION_WITH_OTHER_SNAKE:
        snake_move_forward(psn, pc, false);
        draw_snake(psn, MODE_COLLISION_OTHER_SNAKE);
        *pgame_state = gs_over;
        break;
      case COLLISION_WITH_APPLE:
        snake_move_forward(psn, pc, true);
        //snake_growth_N_and_move_forward_N(psn,1);
        clean_apples();
        init_apples();
        draw_apples();
        draw_apples_info();
        draw_snake(psn, MODE_COLLISION_APPLE);
        break;
    }
    draw_snake_info(psn);
    UI_refresh();
  }
}


void game_start(){

  //bool end_game = false;  //assurance of the end of the game
  int key; //key pressed on keyboard
  snake_t snake;
  snake_t *psn = &snake;

  init_globals_1();
  init_globals_2();
  create_menu(&menu1); //menu_UI
  create_menu(&menu2);
  create_menu(&menu3);

  //end of game inictialization
  game_state_t game_state = gs_start;
  user_choice_t user_choice;

  while(gs_end != game_state){

    switch(game_state){

      case gs_start:
        user_choice = call_menu(&menu1);
        switch(user_choice){
          case M1_START:
            game_state = gs_beginning;
            break;
          case M1_CFG:
            game_state = gs_cfg;
            break;
          case M1_END:
            game_state = gs_end;
            break;
        };
        break;

      case gs_beginning:
        draw_board(); //clean()
        init_snake(&snake, 1, 1);
        init_apples();
        draw_apples();
        draw_apples_info();
        draw_snake(psn, MODE_COLLISION_NOCOLLISION);
        draw_snake_info(psn);
        UI_refresh();
        game_state = gs_go;
        break;

      case gs_go:
       //start counting time
        gettimeofday(&g_time.time_tv, NULL); //start counting time
        g_time.prev_time_s = g_time.time_tv.tv_sec;
        g_time.prev_time_us = g_time.time_tv.tv_usec;

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
          }
          game_move(&snake, &game_state);
        };
        break;

      case gs_pausa:
        user_choice = call_menu(&menu2);
        switch(user_choice){
          case M2_CONTINUE:
            game_state = gs_go;
            break;
          case M2_RESTART:
            game_state = gs_start;
            break;
        };
        break;

      case gs_over:
        user_choice = call_menu(&menu3);
        switch(user_choice){
          case M3_RESTART:
            game_state = gs_beginning;
            break;
          case M3_END:
            game_state = gs_start;
            break;
        };
        break;

      case gs_end:
          //end_game = TRUE;
        break;

      case gs_cfg:
        draw_cfg(&game_cfg.speed, &game_cfg.snake_max_length);
        g_time.time_unit = 300000/game_cfg.speed; //in micro seconds
        game_state = gs_start;
        break;
    }
  }
  destroy_menu(&menu3);
  destroy_menu(&menu2);
  destroy_menu(&menu1);

  return;
}
