#ifndef INIT_UI_H
#define INIT_UI_H

typedef struct screen_tt{
  int xmin;  //left upper x
  int xmax;
  int ymin;
  int ymax;
  int xmid; //center of screen x
  int ymid;
} screen_t;

typedef struct coords_tt {
  int x;
  int y;
} coords_t;

void init_UI();
void end_UI();

#endif //INIT_UI_H
