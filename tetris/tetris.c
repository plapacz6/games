/* authour: plapacz6@gmail.com, date: 2022-02-24 */
#include "tetris.h"


coord_t shape[11][4] = {
  /* beginning block is first bottom left, others are relative to it */
  {{0,0},{+1,0},{0,-1},{+1,-1}}, // []
  
  {{0,0},{0,-1},{0,-2},{0,-3}}, //  |
  {{0,0},{+1,0},{+2,0},{+3,0}}, // ____

  {{0,0},{+1,0},{+2,0},{+2,-1}}, // ___I
  {{0,0},{+1,0},{+2,0},{0,-1}}, //  I___

  {{0,0},{+1,0},{0,-1},{0,-2}}, //   |_
  {{0,0},{+1,0},{+1,-1},{+1,-2}}, //  _|

  {{0,0},{-2,-1},{-1,-1},{0,-1}}, //  ---I
  {{0,0},{0,-1},{+1,-1},{+2,-1}}, //  I---
  {{0,0},{0,-1},{0,-2},{+1,-2}}, //   |-
  {{0,0},{0,-1},{0,-2},{-1,-2}}, //   -|  
};



bool is_bottom_contact(figure_t *f);
void step_down(figure_t *f);

void flip_horizontally(figure_t *f);
void flip_vertically(figure_t *f);
void move_left(figure_t *f);
void move_right(figure_t *f);
void move_down(figure_t *f);

figure_t *new_figure(/*color*/);  //alocate memory
void generate_figure(figure_t *f, coord_t *sh);  //generate coordinates of blocks, and shape



