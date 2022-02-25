/* authour: plapacz6@gmail.com, date: 2022-02-24 */

#ifndef TETRIS_H
#define TETRIS_H
#include <stdlib.h>
#include <stdbool.h>

typedef struct coord_tt {
  unsigned x;
  unsigned y;
} coord_t;

typedef struct figure_tt {
  coord_t c[4];   //coodrynates of each block of element( of fugure)
  coord_t* b[4];  //pointers to bootom blocks (this changes on every turn)
  //color_t color;
  //bool grounded;  //already on ground
} figure_t;

typedef enum shape_code_tt {
  SQUARE, 
  I_VERTICAL, I_HORIZONTAL, 
  L_UR, L_UL, L_DR, L_DL, 
  l_UR, l_UL, l_DR, l_DL 
} shape_code_t;

bool is_bottom_contact(figure_t *f);
void step_down(figure_t *f);

void flip_horizontally(figure_t *f);
void flip_vertically(figure_t *f);
void move_left(figure_t *f);
void move_right(figure_t *f);
void move_down(figure_t *f);

figure_t *new_figure(/*color*/);  //alocate memory
void generate_figure(figure_t *f, coord_t *sh);  //generate coordinates of blocks, and shape

#define BOARD_WIDTH (10)
typedef struct board_tt {
  unsigned ground_level[BOARD_WIDTH];  //array of coordynate y of ground level
} board_t;

#endif // TETRIS_H