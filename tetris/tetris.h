/* author: plapacz6@gmail.com, date: 2022-02-24 */

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
/**
 * @brief appearance of box (element of figure), or board's field
 * 
 */
typedef struct box_appearc_tt {
  box_pattern_t p;
  box_color_t c;
} box_appearc_t;


/**
 * @brief elemenent of figure
 * 
 */
typedef struct box_tt {
  coord_t c;
  box_appearc_t v;

  /** handle background remembering */
  bool bg_filled;
  uint16_t bg;
} box_t;



/**************  figure  ***********************/
typedef struct figure_tt {
  figure_shape_name_t fs;
  coord_t p0;   /**< pivot point */  
  unsigned bx;  /**< refrerence box style */
  box_t box[4];   /**< coodrynates of each block of element( of fugure) */                       
  unsigned xl;    /**< x of left edge of figure */
  unsigned xr;    /**<  x of right edge */    
} figure_t;

typedef struct gm_window_tt{  
  WINDOW *w;
  unsigned left_x;
  unsigned top_y;  
  unsigned width;
  unsigned height;    
} gm_window_t;

/**************  board  ************************/
#define BORAD_HIGHT (24)
#define BOARD_WIDTH (40)
typedef struct board_tt {  
  gm_window_t w;
  unsigned left_x;
  unsigned right_x;
  box_appearc_t b[BORAD_HIGHT][BOARD_WIDTH];    
} board_t;

typedef struct next_fugure_window_tt {
  gm_window_t w;
}next_figure_window_t;

typedef struct score_window_tt {
  gm_window_t w;
} score_window_t;

typedef struct hint_window_tt {
  gm_window_t w;
} hint_window_t;

typedef struct main_window_tt {
  board_t b;
  next_figure_window_t n;
  score_window_t s;
  hint_window_t h;
} main_window_t;

/***************** game *************************/
typedef struct game_state_tt {
  unsigned intv_stepdown; //interval of step down
} game_state_t;

typedef struct game_cfg_tt {
  unsigned intv_start; //starting interval
  int intv_change; //if game will be speed up
} game_cfg_t;
/************************************************/

#define FIGURE_NUMBER (20) //(19)
//#define REF_BOX_A_MAX (8)
#define REF_BOX_A_MAX (5)

extern next_figure_window_t win_next;
extern score_window_t win_score;  
extern hint_window_t win_hint;

extern board_t board;
extern figure_t current_figure;
extern figure_t next_figure;
extern figure_t symulate_figure;
extern coord_t p1;  //point of creating new figure
extern coord_t p2;  //point of creating next figure
extern game_state_t game_state;
extern game_cfg_t game_cfg;

extern coord_t shape[FIGURE_NUMBER][4];
extern box_appearc_t ref_box_v[REF_BOX_A_MAX];
extern box_appearc_t ref_board_empty_field;

void print_info(unsigned y, unsigned x,char *fs, char *msg);
void print_figure(gm_window_t *p_w, figure_t *f);
void unprint_figure(gm_window_t *p_w, figure_t *f);
void print_board();

//bool is_board_field_empty(coord_t *p_c) ;

//unsigned find_ground_y_coord(coord_t *p_c);
//void find_ground_figure_coord(figure_t *p_f);

bool is_bottom_contact(figure_t *f, board_t *b);

int determine_LR_edge(figure_t *fg);
int define_shape_figure(figure_t *fg, figure_shape_name_t fs, unsigned bx);

int create_figure(figure_t *fg, figure_shape_name_t fs, unsigned bx, coord_t *p_p1);
void diassemble_figure(figure_t *f, board_t *b);
void generate_next_figure(figure_t *f, coord_t *p_p1);
void copy_figure(figure_t *f2, figure_t *f1, coord_t *p_p1);

void turn_left(figure_t* f, board_t *p_b);
void turn_right(figure_t* f, board_t *p_b);
void move_right(board_t *p_b, figure_t *f);
void move_left(board_t *p_b, figure_t *f);
void prepare_1figure_to_start(figure_t *f, board_t *p_b);
void step_down(figure_t *f, board_t *p_b);
void move_down(figure_t *f, board_t *p_b);

void create_window_frame(gm_window_t *p_gm, unsigned h, unsigned w, unsigned ty, unsigned lx);
void board_reset();







#endif // TETRIS_H
