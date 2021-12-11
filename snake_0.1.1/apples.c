#include "apples.h"
#include <time.h>
#include <stdlib.h>
#include "init_UI.h"
#include <ncurses.h>

extern apples_t apples;
extern screen_t screen;

void init_apples(){
  //TODO: sprawdzenie czy wylosowane miejsca nie koliduja z:
  //innymi jablkami, granicami, obszarem marginesowym, snakami, przeszkodami
  //bo czasem nie widac jablka gdy jest wstawione na miejscu snaka.
  size_t i;
  size_t j;
  for(i = 0; i < apples.len; i++){
    //-2   i +2  prymitywny sposob na oddalenie jabled od krawiedzi (moze nie dzialac dla ekranu < 2)
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

/**  TODO

każdy obiekt gry oddzielny komplet plikow (c,h) (logic, interface)

podsystemy oddzielnie opracowane, i tylko tu uzywane:
- menu
- config/save file


menu:
- definionane w jakims pliku tekstowym (json) i generowane w pamieci dynamicznie
*/
