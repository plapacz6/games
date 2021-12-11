#ifndef TEST_MENU_DEFINITION_H
#define TEST_MENU_DEFINITION_H

#include "menu_logic.h"
/* definicje menu */

//ilosc roznych menu uzywanych prze gre
//extern enum {MAX_NUMBER_OF_MENU = 3};
#define MAX_NUMBER_OF_MENU (3)

//menu 1:
//definicja identyfikatorow pozycji  zwracane przez menu i wykorzystywane w logice sterowania gra
enum user_choice_enum_t {
  //menu po uruchomieniu programu
  M1_START,
  M1_CFG,
  M1_END,

  //pausa w grze (spacja)
  M2_GO,
  M2_END,

  //game over
  M3_RESTART,
  M3_END,
};
typedef enum user_choice_enum_t user_choice_t;


#endif // TEST_MENU_DEFINITION_H

