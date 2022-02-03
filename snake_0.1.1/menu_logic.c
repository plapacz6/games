#include <stdbool.h>
#include "menu_logic.h"
#include "menu_UI.h"
/** launches the menu and returns the user selection
*/

int call_menu(menu_t *pm){
  int user_choice;
  show_menu(pm);
  user_choice = wait_for_user_choice(pm);
  hide_menu(pm);
  return user_choice;
}


