
#ifndef SNAKE_LOGIC_H
#define SNAKE_LOGIC_H

typedef struct snake_tt{
  int dx;  //step in x asix
  int dy;
  int x[MAX_LEN_SNAKE];
  int y[MAX_LEN_SNAKE];
  int length;
  int id; //snake's number
} snake_t;

typedef enum mode_of_snake_tt{
  MODE_COLLISION_NOCOLLISION = 0,
  MODE_COLLISION_ONESLEF = 1,
  MODE_COLLISION_BORDER = 2,
  MODE_COLLISION_WALL = 3,
  MODE_COLLISION_OTHER_SNAKE = 4,
  MODE_COLLISION_APPLE = 5
} mode_of_snake_t;

void init_snake(snake_t *psn, int id, size_t length);
int snake_growth_N_and_move_forward_N(snake_t *psn, int n);
int snake_growth_tail_N(snake_t *psn, size_t n);
void snake_move_forward(snake_t *ps, coords_t *pc, bool growth);
#endif //SNAKE_LOGIC_H
