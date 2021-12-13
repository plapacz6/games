 /*
 * author:plapacz6@gmail.com
 * licence:GPLv3
 */

#include <ncurses.h>

int TESTUJ_KODY_KLAWISZY(){
//sprawdzani kodow klawisy 
//z ncurses.h KEY_... zgadza sie ostatnie 2 cyfry,Tzn. up:0403 103
  int key;
    while(1){
      mvprintw(3, 4, "press a key (function, arrow, alpha) (ESC x 2 == end)");

      uint16_t kod;
      uint8_t kodL;
      uint8_t kodH;
      key = getch();
      kod = key;
      kodL = kod;
      kodH = kod>>8;
 
      mvprintw(5, 4,
          "%0x == %u == %i == %x KOD[%u: %u %u]\n",
          (int)key, 
          (unsigned)key, 
          (int)key, 
          (int)key,
          kod,kodH,kodL
          );                    
      if(key == 27) break;
    }
    return key;
}
#ifdef TEST
int main(){
  initscr();
  timeout(-1); //-1 blokowanie getchar(), 
              //>0 - bez blokowania - odczyt natychmiast
  raw();      //bez buforowania
  keypad(stdscr, TRUE); //właczenie FunctionKey, Arrows
  noecho();   //echo off
  mvprintw(1, 4, "ncurses gotowe ... COLS == %d, LINES == %d", COLS, LINES);

  TESTUJ_KODY_KLAWISZY();

  endwin(); //zamkniecie ncurses
}
#endif //TEST
