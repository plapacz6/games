/* authour: plapacz6@gmail.com, date: 2022-02-24 */

#ifndef TETRIS_H
#define TETRIS_H
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
//#include <stdarg.h>
#include <ncurses.h>

typedef enum  {
  FS_DEBUG,
  FS_B1_BOX,     //B1

  FS_LN1_LINE_V,  //LN1
  FS_LN2_LINE_H,  //LN2

  FS_L1_L_LEFT,   //L1
  FS_L2_L_,       //L2
  FS_L3_L_RIGHT,  //L3
  FS_L4_L_BOTTOMUP,  //L4

  FS_RL1_RL_LEFT,  //RL1
  FS_RL2_RL_,      //RL2
  FS_RL3_RL_RIGHT, //RL3
  FS_RL4_RL_BOTTOMUP, //RL4

  FS_RZ1_RZ_,      //RZ1
  FS_RZ2_RZ_LEFT,  //RZ2

  FS_Z1_Z_,       //Z1
  FS_Z2_Z_LEFT,   //Z2
  
  
  FS_H1_HAT,       //H1
  FS_H2_HAT_VL,    //H2
  FS_H3_RHAT,      //H3
  FS_H4_RHAT_VL,   //H4
} figure_shape_name_t;



typedef struct coord_tt {
  unsigned x;
  unsigned y;
} coord_t;



/**************  box  *************************/
typedef char box_pattern_t;
typedef int box_color_t;
typedef struct box_visual_tt {
  box_pattern_t p;
  box_color_t c;
} box_visual_t;
/**
 * @brief elemenent of figure
 * 
 */
typedef struct box_tt {
  coord_t c;
  box_visual_t v;

  /** handle background remembering */
  bool bg_filled;
  uint16_t bg;
} box_t;



/**************  figure  ***********************/
typedef struct figure_tt {
  figure_shape_name_t fs;
  coord_t p0;   /**< pivot point */  
  
  box_t *box[4];   /**< coodrynates of each block of element( of fugure) */                       
  unsigned xl;    /**< x of left edge of figure */
  unsigned xr;    /**<  x of right edge */  
  coord_t* bt[4];  /**< pointers to bootom blocks (this changes on every turn) */    
} figure_t;

typedef enum shape_code_tt {
  SQUARE, //
  I_VERTICAL, I_HORIZONTAL, 
  L_UR, L_UL, L_DR, L_DL, 
  l_UR, l_UL, l_DR, l_DL 
} shape_code_t;


/**************  board  ************************/
#define BORAD_HIGHT (60)
#define BOARD_WIDTH (10)
typedef struct board_tt {
  box_visual_t b[BORAD_HIGHT][BOARD_WIDTH];
  unsigned ground_level[BOARD_WIDTH];  //array of coordynate y of ground level
  unsigned top_y;
  unsigned bottom_y;
  unsigned left_x;
  unsigned right_x;
} board_t;





extern figure_t *create_figure(figure_shape_name_t fs, box_t *p_bx, coord_t *p_p1);
extern void diassemble_figure(figure_t *f, board_t *b);

extern void turn_left(figure_t* f, coord_t *p1, board_t *p_b);
extern void turn_right(figure_t* f, coord_t *p1, board_t *p_b);
extern void flip_horizontally(figure_t *f);
extern void flip_vertically(figure_t *f);
extern void move_left(figure_t *f, board_t *b);
extern void move_right(figure_t *f, board_t *b);

extern bool is_bottom_contact(figure_t *f, board_t *b);
extern void step_down(figure_t *f);
extern void move_down(figure_t *f);


#endif // TETRIS_H
