#include <ncurses.h>

int TESTUJ_KODY_KLAWISZY(){
//sprawdzani kodow klawisy 
//z ncurses.h KEY_... zgadza sie ostatnie 2 cyfry,Tzn. up:0403 103
  int key;
    while(1){
      uint16_t kod;
      uint8_t kodL;
      uint8_t kodH;
      key = getch();
      kod = key;
      kodL = kod;
      kodH = kod>>8;
 
      mvprintw(3, 3,
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

int main(){
  initscr();
  timeout(3); //-1 blokowanie getchar(), 
              //>0 - bez blokowania - odczyt natychmiast
  raw():    //bez buforowania
  kaypad(stdscr, TRUE); //właczenie FunctionKey, Arrows
  noecho();   //echo off
  mvprintw(0, 2, "ncurses gotowe ... COLS == %d, LINES == %d", COLS, LINES);

  TESTUJ_KODY_KLAWISZY();

  endwin(); //zakniecie ncurses
}
