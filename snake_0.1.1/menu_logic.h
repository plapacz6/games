#ifndef MENU_LOGIC_H
#define MENU_LOGIC_H

#include <stdlib.h>

typedef struct menu_item_tt{
  const char* item_name;
  const char* item_description;
  const int item_id;
} menu_item_t;

typedef struct menu_tt{
  const size_t id_menu;
  const size_t number_of_items;
  const menu_item_t* const items;
  const char *menu_title;
} menu_t;

/** launches the menu and returns the user selection
*/
int call_menu(menu_t *pm);
/** inicjalizacja menu */
void create_menu(menu_t *pm);
/** destructor menu */
void destroy_menu(menu_t *pm);

#endif //MENU_LOGIC_H

