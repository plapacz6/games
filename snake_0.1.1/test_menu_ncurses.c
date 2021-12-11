#include <ncurses.h>
#include <string.h>
#include "menu_definitions.h"
#include "menu_UI.h"
#include "init_UI.h"

extern menu_t menu1;
extern menu_t menu2;
extern menu_t menu3;
extern screen_t screen;

void wypelnij_tlo(){
  char *tlo = ".:!~%^*_#";
  int x, y, i, i_max;
  i = 0;
  i_max = strlen(tlo);
  for(y = screen.ymin; y < screen.ymax; y++){
    for(x = screen.xmin; x < screen.xmax; x++){
      if(i > i_max) {
        i = 0;
      }
      mvaddch(y, x, tlo[i]);
      i++;
    }
  }
  refresh();
}

int main(int argc, char** argv){
  int user_choice;
  bool koniec = false;

  init_UI();

  create_menu(&menu1); //menu_UI
  create_menu(&menu2);
  create_menu(&menu3);
  wypelnij_tlo();
  show_menu(&menu1);  //
  user_choice = wait_for_user_choice(&menu1);
  hide_menu(&menu1);
  while(koniec != true){
    switch(user_choice){
      case M1_START:
        user_choice = call_menu(&menu2);
        break;
      case M1_CFG:
        user_choice = call_menu(&menu3);
        break;
      case M1_END:
        koniec = true;
        break;
      case M2_GO:
        user_choice = call_menu(&menu3);
        break;
      case M2_END:
        user_choice = call_menu(&menu1);
        break;
      case M3_RESTART:
        user_choice = call_menu(&menu2);
        break;
      case M3_END:
        user_choice = call_menu(&menu1);
        break;
    }
  }
  destroy_menu(&menu3);
  destroy_menu(&menu2);
  destroy_menu(&menu1);

  end_UI();
  return 0;
}
menu_item_t m1[] = {
                  {"1", "MENU 2", M1_START},
                  {"2", "MENU 3", M1_CFG},
                  {"3", "ZAKONCZ", M1_END}
                  };
menu_t menu1 = {
  1, 3,
  m1,
  "SNAKE",
};
menu_item_t m2[] = {
  {" ", "MENU 3", M2_GO},
  {"x", "MENU 1", M2_END,}
  };
menu_t menu2 = {
  2, 2,
  m2,
  "PAUZA",
};
menu_item_t m3[] = {
  {"y", "MENU 2", M3_RESTART},
  {"x", "MENU 1", M3_END}
  };
menu_t menu3 = {
  3, 2,
  m3,
  "GMAE OVER!",
};
