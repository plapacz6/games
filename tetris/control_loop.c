/* author: plapacz6@gmail.com, date: 2022-05-20 */
#include <time.h>
#include <ncurses.h>
#include "tetris.h"


bool is_time_to_step(clock_t t1, unsigned intv){
  //mvwprintw(win_score.w.w,4,1,"%u  %u",t1, clock());
  //wrefresh(win_score.w.w);
  if( (clock() - t1) > (CLOCKS_PER_SEC/1000) * intv ) {
    //mvwprintw(win_score.w.w,5,1,"%s","time past");
    //wrefresh(win_score.w.w);
    return true;
  }
  return false;
}

void time_control(){ 
}

void user_control(){

  int16_t key;
  //uint8_t keyH;
  //uint8_t keyL;
  char akey[4] = {0,0,0,0};
  bool finishDrawinFigures = false;

  clock_t time_start;
  unsigned interval = 500; //ms

  figure_t *f = &current_figure;
    
  time_start = clock();

  prepare_1figure_to_start(f, &board);

  while(!finishDrawinFigures){
    
    if(is_time_to_step(time_start, interval)){
      
      step_down(f, &board);                    

      time_start = clock();
      /*
      after step don't process key input
      becasue 1 action at time
      (step + move left/right may cause not detect contact with ground)
      */
      continue;  
    }
        
    key = 0;
    key = getch();
    //keyH = key >> 8;
    //keyL = (uint8_t) key;
    akey[0] = key;
    ////print_info(2,1,"%c", akey);  /*DEBUG*/
    
    switch(key){
      case 27: //ESC
      case 'q':
        finishDrawinFigures = true;
        break;     
     
      case KEY_LEFT:
      case 'l':        
        move_left(&board, f);
        break;
      case KEY_RIGHT:
      case 'r':        
        move_right(&board, f);
        break;
      case KEY_UP:        
        turn_left(f, &board);
        break;
      case KEY_DOWN:        
        turn_right(f, &board);
        break; 
      case ' ':
        move_down(f, &board);
        break;
       // break;      
    }//switch
    
    wrefresh(board.w.w);
    refresh();

  }//while
  
  
}//test_draw_shape()
