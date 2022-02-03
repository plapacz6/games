#ifndef MENU_UI_H
#define MENU_UI_H
#include "init_UI.h"
#include "menu_logic.h"   //user_choice_t, menu_t
#include "menu_definitions.h"

//shows / hides the menu
void show_menu(menu_t *pm);
void hide_menu(menu_t *pm);
//czeka na wybor
int wait_for_user_choice(menu_t *pm);

/** menu initialization */
void create_menu(menu_t *pm);
/** destructor menu */
void destroy_menu(menu_t *pm);

#endif // MENU_UI_H

