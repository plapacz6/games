#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H
#include <stdlib.h>
#include <sys/time.h>
#include <stdbool.h>
#define MAX_NUMBER_APPLES (5)
#define MAX_LEN_SNAKE (1000)

typedef enum game_state_tt{
  gs_beginning,
  gs_start,
  gs_cfg,
  gs_end,
  gs_go,
  gs_pausa,
  gs_over
} game_state_t;

typedef enum type_of_collision_tt{
  NO_COLLISION = 0,
  COLLISION_WITH_BORDER = 1,
  COLLISION_WITH_ONESELF,
  COLLISION_WITH_WALL,
  COLLISION_WITH_OTHER_SNAKE,
  COLLISION_WITH_APPLE,        //snake growth +1
} type_of_collision_t;

typedef struct game_time_control_tt{
  struct timeval time_tv;
  int prev_time_s;
  int prev_time_us;
  unsigned long time_unit;
} game_time_control_t;

typedef struct game_cfg_tt{
  size_t speed;
  size_t number_of_snake;
  size_t snake_max_length[1];
} game_cfg_t;

void game_start();

#endif //GAME_LOBIC_H
