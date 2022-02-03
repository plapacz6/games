
/*
 * snake's logic
 */
#include <stdbool.h>
#include "init_UI.h"
#include "game_logic.h"
#include "snake_logic.h"
#include "apples.h"
extern screen_t screen;
extern apples_t apples;
extern game_cfg_t game_cfg;

//preparing snake to move
void snake_move_forward(snake_t *psn, coords_t *pc, bool growth){
  int i;
  /*
     Moving snake forward:
     Changing the coordinates of each part of the snake
     to the coordinates of the part following it,
     and moving head of the snake to the coordinases == pc.
  */
  if(growth == true){
    if(psn->length < game_cfg.snake_max_length[psn->id] - 1){
      psn->length++;
    }
  }
  for(i = psn->length; i > 0; i--){
    psn->x[i] = psn->x[i - 1];
    psn->y[i] = psn->y[i - 1];
  }
  psn->x[0] = pc->x;
  psn->y[0] = pc->y;
}

int snake_growth_N_and_move_forward_N(snake_t *psn, int n){
  coords_t coords;
  coords.x = psn->x[0];
  coords.y = psn->y[0];
  int i;
  int growth_impossible = 0;
  for(i = 0; i < n; i++){
    if(psn->length < game_cfg.snake_max_length[psn->id] - 1){
      coords.x += psn->dx;
      coords.y += psn->dy;
    }
    else{
      growth_impossible++;
    }
    snake_move_forward(psn, &coords, true);
  }
  return growth_impossible;
}

/** the snake grows from the tail, the head does not move
 * (finds random places for growth that do not collide with other board objects)
 */
int snake_growth_tail_N(snake_t *psn, size_t n){
  screen_t sc = screen;
  apples_t as = apples;
  int x;
  int y;
  int dx; //direction and growth step
  int dy;
  int i;
  int j;
  bool gotowe = false;
  bool kolizja = false;
  do{
    x = psn->x[0];
    y = psn->y[0];
    gotowe = true;
    for(i = 0; i < n; i++){
      dx = rand()%2?-1:1;  //-1 or 1
      dy = rand()%2?-1:1;  //-1 or 1
      //Check for collisions with board objects, apples, borders, snaps, and yourself.
      if(kolizja == true){
        gotowe = false;
        break;
      }
    }
  } while(gotowe != true);
}

void init_snake(snake_t *psn, int id, size_t length){
  psn->id = id;
  psn->length = length;
  psn->dx = 0;
  psn->dy = -1;
  psn->x[0] = screen.xmid;
  psn->y[0] = screen.ymid;
  int i;
  for(i = 0; i < game_cfg.snake_max_length[psn->id]; i++){
    psn->x[i] = screen.xmid;
    psn->y[i] = screen.ymid;
  }
  snake_growth_N_and_move_forward_N(psn, 0);
}

