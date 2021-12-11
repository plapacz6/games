#include "game_menu_definition.h"
/*
typedef struct {
  char* item_name;
  char* item_description;
  user_choice_t item_id;
} menu_item_t;

typedef struct {
  size_t id_menu;
  size_t number_of_items;
  menu_item_t* items;
  char *menu_title;
} menu_t;
*/
menu_item_t m1[] = {
  {"1", "URUCHOM GRE", M1_START},
  {"2", "USTAWIENIA", M1_CFG},
  {"x", "ZAKONCZ", M1_END},
};
menu_t menu1 = {
  1, 3,
  m1,
  "SNAKE",
};
menu_item_t m2[] = {
  {" ", "KONTYNUUJ", M2_CONTINUE},
  {"x", "MENU GLOWNE", M2_RESTART},
};
menu_t menu2 = {
  2, 2,
  m2,
  "PAUZA",
};
menu_item_t m3[] = {
  {"y", "ROZPOCZNIK JESZCZE RAZ", M3_RESTART},
  {"x", "MENU GLOWNE", M3_END},
};
menu_t menu3 = {
  3, 2,
  m3,
  "GAME OVER!",
};
