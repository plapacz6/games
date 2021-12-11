#ifndef GAME_MENU_DEFINITION_H
#define GAME_MENU_DEFINITION_H
#include "menu_logic.h"
#include "menu_UI.h" //przed tym musi być  'game_menu_definition.h" lub 'test_menu_definition.h' wlaczone

/* definicje menu */

//ilosc roznych menu uzywanych prze gre
#define MAX_NUMBER_OF_MENU (3)

//menu 1:
//definicja identyfikatorow pozycji  zwracane przez menu i wykorzystywane w logice sterowania gra
//typedef unsigned user_choice_t;
typedef enum user_choice_tt{
  //menu po uruchomieniu programu
  M1_START,
  M1_CFG,
  M1_END,

  //pausa w grze (spacja)
  M2_CONTINUE,
  M2_RESTART,

  //game over
  M3_RESTART,
  M3_END,
} user_choice_t;


#endif //GAME_MENU_DEFINITION_H
