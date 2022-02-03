#ifndef TEST_MENU_DEFINITION_H
#define TEST_MENU_DEFINITION_H

#include "menu_logic.h"
/* menu definitions */

//the number of different menus used by the game
//extern enum {MAX_NUMBER_OF_MENU = 3};
#define MAX_NUMBER_OF_MENU (3)

//menu 1:
/** definition of identifiers of menu items returned by menus 
 * and used in the game control logic
 */
enum user_choice_enum_t {
  //menu after starting the program
  M1_START,
  M1_CFG,
  M1_END,

  //pause in the game (space)
  M2_GO,
  M2_END,

  //game over
  M3_RESTART,
  M3_END,
};
typedef enum user_choice_enum_t user_choice_t;


#endif // TEST_MENU_DEFINITION_H

