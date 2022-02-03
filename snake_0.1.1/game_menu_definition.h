#ifndef GAME_MENU_DEFINITION_H
#define GAME_MENU_DEFINITION_H
#include "menu_logic.h"
#include "menu_UI.h" //before <-this must be turn on  'game_menu_definition.h" or 'test_menu_definition.h'

/* definicje menu */

//ilosc roznych menu uzywanych prze gre
#define MAX_NUMBER_OF_MENU (3)

//menu 1:
//menu item identifiers used in the game control logic
//typedef unsigned user_choice_t;
typedef enum user_choice_tt{
  //menu after program starts
  M1_START,
  M1_CFG,
  M1_END,

  //pause in the game (spacja)
  M2_CONTINUE,
  M2_RESTART,

  //game over
  M3_RESTART,
  M3_END,
} user_choice_t;


#endif //GAME_MENU_DEFINITION_H
