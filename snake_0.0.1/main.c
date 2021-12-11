/*
 * author:Paweł Łapacz
 * licence:free
 * data:2020-03-30
 * ver:0.0.1
 *
 * game snake from scratch for linux terminal
 * c89
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "snake_types.h"
 
//typedef enum {ms_menu,ms_game, ms_cfg, ms_end} menu_state_t;
//typedef enum {gs_beginning,gs_pausa, gs_go, gs_end} game_state_t;
//typedef struct {
//  int board;  //board: empty or various obstacles
//  int speed;
//  //int incresing_speed; //speed increases automaticaly during game
//  //during level or after level's finishing
//  //int snake_growth; //snake is growing after eatign someting
//  //int number_of_snakes;
//} state_of_game_t;
//
///* board definition */
//typedef struct {
//} board_t;
///* state of sanke */
//typedef struct {
//} snake_t;
//state_of_game_t state_of_game;

menu_state_t show_help(int argc, char** argv);
void init_game(void);
menu_state_t menu1(void);
void cfg_edit(void);
void game_start(state_of_game_t *state_of_game);


int  main(int argc, char** argv){
  menu_state_t menu_state = ms_menu;

  if(ms_end == show_help(argc,argv)){
    return 0;
  }

  init_game();
  while(ms_end != menu_state){
    if(ms_menu == menu_state){    
    /* cfg or game or end*/
    menu_state = menu1();
    }
    if(ms_cfg == menu_state){
    /* parameters of play */
    cfg_edit();
    menu_state = ms_menu;
    }
    if(ms_game == menu_state){
    /* waiting for player to start */
    game_start(&state_of_game);
    menu_state = ms_menu;
    }
  }

  return 0; 
}

menu_state_t show_help(int argc, char** argv){
  printf("%s\n","HELP: gra snake na terminal linuksowy");
  if(argc >= 2){
    if(!strncmp(argv[1],"-h",2) || !strncmp(argv[1],"--help",6)){
      printf("%s\n",
      "gra snake dla linksowego terminla\n"
      "prawdowpodobnie w ncurses\n"
      "wersja ..."
      );
    }
    else {
      printf("%s\n",
      "Usage: snake\n"
      "     : sanke -h\n"
      "     : snake --help"
      );
    }
    return ms_end;
  }
  return ms_menu;
}
void init_game(void){
  puts("inicjlaizacja gry ... ");
  //ncurses init
  //wyzerowanie stanu
}
menu_state_t menu1(void){
  char user_answer = '0';
  printf("%s\n",
    "menu:\n"
    "1 - graj\n"
    "2 - cfg\n"
    "3 - koniec\n"
    );    
    user_answer = fgetc(stdin);
    switch(user_answer){
      case '1':
        return ms_game;
      case '2':
        return ms_cfg;
      case '3':
        return ms_end;
    };
    return ms_menu;
}

void cfg_edit(void){
  puts("edycja cfg");  
}

