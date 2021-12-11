#include "init_UI.h"
#include <ncurses.h>

screen_t screen;  //game has one screen only

void init_UI(){
#ifdef NCURSES
//void init_ncurses(){
  initscr();
  timeout(3); //-1 - blocking getchar(), >0 - without blocking
  raw();               //disable buffering
  keypad(stdscr,TRUE); //enable FunctionKey, Arrows
  noecho();            //echo off
  mvprintw(0, 2, "ncurses gotowe ... COLS == %d, LINES == %d", COLS, LINES);

  screen.xmin = 0;
  screen.ymin = 0;
  screen.xmax = (COLS - 1);
  screen.ymax = (LINES - 1);
  screen.xmid = (COLS - 1) / 2;
  screen.ymid = (LINES - 1) / 2;
#endif
}

void end_UI(){
#ifdef NCURSES
//void end_ncurses(){
  clear(); //???
  endwin(); //close ncurses
#endif
}
