
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

/**
 * @brief initialization of snake of id id
 * 
 * @param psn pointe to the snake
 * @param id an integer identifier
 * @param length non-negative starting snake length
 */
void init_snake(snake_t *psn, int id, size_t length);

/**
 * @brief checking if in current direction growth of snake is possible 
 * 
 * @param psn a pointer to the snake
 * @param n an integer parameter specifying how much the snake will grow
 * @return int an integer amount of impossible step of growth in current direction
 */
int snake_growth_N_and_move_forward_N(snake_t *psn, int n);

/**
 * @brief 
 * 
 * the snake grows from the tail, the head does not move
 * (finds random places for growth that do not collide with other board objects)
 * 
 * @param psn pointer to the sneak
 * @param n non-negatve amount of growth sneak's tail
 * @return int 
 */
int snake_growth_tail_N(snake_t *psn, size_t n);

/**
 * @brief preparing snake to move
 * 
 * @param psn a pointer to snake
 * @param pc a pointer to struct of coordynates
 * @param growth a flag if snake is going to growth
 */
void snake_move_forward(snake_t *ps, coords_t *pc, bool growth);

#endif //SNAKE_LOGIC_H
