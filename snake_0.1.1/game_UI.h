#ifndef GAME_NCURSES_H
#define GAME_NCURSES_H
#include "init_UI.h"
#include "snake_logic.h"

typedef enum {
  IDX_BOARD_LU = 0,
  IDX_BOARD_LD = 1,
  IDX_BOARD_U = 2,
  IDX_BOARD_D = 3,
  IDX_BOARD_RU = 4,
  IDX_BOARD_RD = 5,
  IDX_BOARD_L = 6,
  IDX_BOARD_R = 7,

  IDX_SNAKE_HEAD = 0,
  IDX_SNAKE_BODY = 1,
  IDX_SNAKE_ERASE = 2,

  IDX_APPLE_NO1 = 0,
  IDX_APPLE_NO2 = 1,
  IDX_APPLE_NO3 = 2,
  IDX_APPLE_CLEAN = 3,

  IDX_UFO_1 = 0,
} graphic_symbol_index_t;

typedef struct {
  char* board_border;
  char* collision;
  char* snake_body;
  char* apple;
  char* ufo;
} graphic_symbol_t;

void init_globals_2();
void UI_refresh();
void draw_board();
void draw_apples();
void draw_apples_info();
void draw_snake_info(snake_t *psn);
void clean_apples();
void draw_snake(snake_t* ps, unsigned snake_head_symbol);
void draw_cfg(size_t *pspeed, size_t *psnake_max_length);
#ifdef DEBUG
void PRINT_DEBUG(snake_t *psn, int key);
#endif // DEBUG
#endif //GAME_NCURSES_H

