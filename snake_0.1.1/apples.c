#include "apples.h"
#include <time.h>
#include <stdlib.h>
#include "init_UI.h"
#include <ncurses.h>

extern apples_t apples;
extern screen_t screen;

void init_apples(){
  //TODO: schecking if the places drawn do not collide with:
  //other apples, borders, marginal area, snags, obstacles,
  //because sometimes you don't see the apple when it is inserted 
  //in the place where the sneak is.
  size_t i;
  size_t j;
  for(i = 0; i < apples.len; i++){
    //-2   i +2  primitive way to move the jabled away from the edge (may not work for screen <2)
    apples.a[i].x = (rand()%( (screen.xmax - screen.xmin)  - 4) ) + 2;
    apples.a[i].y = (rand()%( (screen.ymax - screen.ymin)  - 4) ) + 2;
    for(j = 0; j < i; j++){
      if(apples.a[j].x == apples.a[i].x && apples.a[j].y == apples.a[i].y){
        i--;
        break;
      }
    }
  }
}

