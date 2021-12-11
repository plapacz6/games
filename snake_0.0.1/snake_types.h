/*
 * author:Paweł Łapacz
 * licence:free
 * data:2020-03-30
 * ver:0.0.1
 *
 * game snake from scratch for linux terminal
 * c89
 */
#ifndef SNAKE_TYPES_H
#define SNAKE_TYPES_H

typedef enum {
  ms_menu,
  ms_game, 
  ms_cfg, 
  ms_end
} menu_state_t;
typedef enum {
  gs_beginning,
  gs_pausa, 
  gs_go, 
  gs_over,
  gs_end
} game_state_t;
typedef struct {
  int board;  //board: empty or various obstacles
  int speed;
  //int incresing_speed; //speed increases automaticaly during game
  //during level or after level's finishing
  //int snake_growth; //snake is growing after eatign someting
  //int number_of_snakes;
} state_of_game_t;

/* board definition */
typedef struct {
} board_t;
/* state of sanke */
state_of_game_t state_of_game;


#endif //SNAKE_TYPES_H
