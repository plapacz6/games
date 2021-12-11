#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <ncurses.h>
#include "init_UI.h"
#include "menu_definitions.h"
#include "menu_UI.h"

//opis buforow jednej pozycji menu
typedef struct {
  char *text;     //text menu's item
  int *covered;  //part of screen covered by textx
  coords_t xy;     //wspolrzedne poczatku pozycji na ekranie
  size_t bufor_len;
} menu_item_bufor2_t;

//opis opsow byforwo pozycji dla calego jednego menu
typedef struct {
  menu_item_bufor2_t **item_bufor2; //tablica wkaznikow do buforow poszczegolnych pozycji menu
  //menu_title
  //top_screen_border_tekst
  //bottom_screen_border_tekst
} menu_items_bufors_t;

extern screen_t screen;

/**  f.pomocnicza wczytujaca zawartosc ekranu ktory bedzie nadpisany przez menu
     w celu jego przywrocenia po ukryciu menu
*/
void read_covered_area(char *bufor){
}

//tablica wskaznikow do opisow poszczegolnych menu
//#define MAX_NUMBER_OF_MENU = //to zdefinowane tam gdzie definicja menu:
menu_items_bufors_t menus_bufors[MAX_NUMBER_OF_MENU]; //zamiast listy,  globalna wiec wyzerowana
extern screen_t screen;

void menu_item_text(char *text,
      char *s1,  char *s2,
      const char *s3,
      char *s4,
      const char *s5,
      char *s6,  char *s7
    )
    {
    strncpy(text, s1, strlen(s1)); //frame
    strncat(text, s2, strlen(s2));  //margin
    strncat(text, s3, strlen(s3)); //NAME
    strncat(text, s4, strlen(s4));  //margin
    strncat(text, s5, strlen(s5));  //DESCRIPTION
    strncat(text, s6, strlen(s6));  //margin
    strncat(text, s7, strlen(s7));  //frame

    strcat(text, "\0");
}


/*** wczytuje podaja wpolrzednymi obszar ekreanu do bufora
  * y1,x1 - lewy gorny rog
  * y2,x2 - prawy dolny
  * UWAGA, ale ma to sens tylko dla jednego wiersza, chyba, ze
  * znamy dlugosc wiersza i mozemy recznie podzielic bufor pozniej
  * */
int read_screen(int *buf, size_t max_buf, int y1, int x1, int y2, int x2){
   int x, y, i;
   i = 0;
   for(y = y1; y <= y2; y++){
     for(x = x1; x <= x2; x++){
       move(y,x);
       buf[i] = inch();
       i++;
       if(i > max_buf){
         return 2; //zakonczenie spowodowane osiagnieciem max pojemnosci bufora
       }
     }
   }
   return 0; //zakonczenie normalne
 }
int put_readed_screen(int *buf, size_t max_buf, int y1, int x1, int y2, int x2){
   int x, y, i;
   i = 0;
   for(y = y1; y <= y2; y++){
     for(x = x1; x <= x2; x++){
       move(y,x);
       addch(buf[i]);
       i++;
       if(i > max_buf){
         return 2; //zakonczenie spowodowane osiagnieciem max pojemnosci bufora
       }
     }
   }
   return 0; //zakonczenie normalne
 }


/** inicjalizacja menu */
void create_menu(menu_t *pm){
  //elementy stylu menu ncurses - dlatego tutaj,  bo niezmienne
  char *MARGIN_L = "  ";
  char *MARGIN_M = " . ";
  char *MARGIN_R = "  ";
  char *MENU_ITEM_FRAME_L = ">>";
  char *MENU_ITEM_FRAME_R = "<<";
  //
  int menu_coord_y;
  size_t i;
  size_t bufor_len;

  //utworzenie wezla buforow dla jenego menu
  //!!! TU CHYBA JEST WSKAZNIK ???
  //przydzial pamieci dla tablicy wskaznikow na
  //strukture opisujcą bufory jednej pozycji menu
  //zapisany do wskaznika na ta strukture
  menus_bufors[pm->id_menu].item_bufor2 = malloc(pm->number_of_items *
                             sizeof(menu_item_bufor2_t*));
  //obliczenie wsporzednej y menu
  menu_coord_y = screen.ymid - (pm->number_of_items / 2) - 1;  //ilosc pozycji menu i jeszcze tytul menu
  //dla kazdej pozycji w menu
  for(i = 0; i < pm->number_of_items; i++){
    //wyliczenie dlugosci bufora
    bufor_len = strlen(MENU_ITEM_FRAME_L) +
                strlen(MARGIN_L) +
                strlen(pm->items[i].item_name) +
                strlen(MARGIN_M) +
                strlen(pm->items[i].item_description) +
                strlen(MARGIN_R) +
                strlen(MENU_ITEM_FRAME_R) +
                + 1;
    //utworzenie bufora dla jednej pozycji menu (bufor na tekst i na podspod)
    //!!! TU SIE WYWALA  CZY ZAMIAST -> NIE POZIWNNO BYĆ . ???
    menus_bufors[pm->id_menu].item_bufor2[i] = malloc(sizeof(menu_item_bufor2_t));
    menus_bufors[pm->id_menu].item_bufor2[i]->text = malloc(bufor_len * sizeof(int));
    menus_bufors[pm->id_menu].item_bufor2[i]->covered = malloc(bufor_len * sizeof(int));
    menus_bufors[pm->id_menu].item_bufor2[i]->bufor_len = bufor_len;
    //ustawienie wporzlednej x pozycji menu,  i wpolrzednej y
    menus_bufors[pm->id_menu].item_bufor2[i]->xy.x = screen.xmid - (bufor_len / 2);
    menus_bufors[pm->id_menu].item_bufor2[i]->xy.y = menu_coord_y++;
    //wypelnienie trescia buforow
    menu_item_text(
      menus_bufors[pm->id_menu].item_bufor2[i]->text,
      MENU_ITEM_FRAME_L,
      MARGIN_L,
      pm->items[i].item_name,
      MARGIN_M,
      pm->items[i].item_description,
      MARGIN_R,
      MENU_ITEM_FRAME_R
    );
    menus_bufors[pm->id_menu].item_bufor2[i]->covered = malloc(bufor_len * sizeof(int));
    /* na etapie tworzenie, nie ma sensu zapisywac tla, bo ono bedzie inne przy wywolaniu
    read_screen(
      menus_bufors[pm->id_menu].item_bufor2[i]->covered,
      bufor_len,
      menus_bufors[pm->id_menu].item_bufor2[i]->xy.y,
      menus_bufors[pm->id_menu].item_bufor2[i]->xy.x,
      menus_bufors[pm->id_menu].item_bufor2[i]->xy.y,
      menus_bufors[pm->id_menu].item_bufor2[i]->xy.x +
      menus_bufors[pm->id_menu].item_bufor2[i]->bufor_len
    );
    */
  }
}

/** destructor menu */
void destroy_menu(menu_t *pm){
  /* zwolnienie buforow */
  int i;
  //usuniecie buforow na test i tlo pozycji w menu
  for(i = 0; i < pm->number_of_items; i++){
    free(menus_bufors[pm->id_menu].item_bufor2[i]->text);
    free(menus_bufors[pm->id_menu].item_bufor2[i]->covered);
    free(menus_bufors[pm->id_menu].item_bufor2[i]);
  }
  free(menus_bufors[pm->id_menu].item_bufor2);
}


//pokazuje / ukrywa menu
void show_menu(menu_t *pm){
  int i;
  for(i = 0; i < pm->number_of_items; i++){
    read_screen(
      menus_bufors[pm->id_menu].item_bufor2[i]->covered,
      menus_bufors[pm->id_menu].item_bufor2[i]->bufor_len,
      menus_bufors[pm->id_menu].item_bufor2[i]->xy.y,
      menus_bufors[pm->id_menu].item_bufor2[i]->xy.x,
      menus_bufors[pm->id_menu].item_bufor2[i]->xy.y,
      menus_bufors[pm->id_menu].item_bufor2[i]->xy.x +
      menus_bufors[pm->id_menu].item_bufor2[i]->bufor_len
    );
    mvprintw(
      //i + 5,
      menus_bufors[pm->id_menu].item_bufor2[i]->xy.y,
      //2,
      menus_bufors[pm->id_menu].item_bufor2[i]->xy.x,
      "%s",
      menus_bufors[pm->id_menu].item_bufor2[i]->text
    );
  }
  move(0,0);
  refresh();
}
void hide_menu(menu_t *pm){
  int i;
  for(i = 0; i < pm->number_of_items; i++){
    put_readed_screen(
      menus_bufors[pm->id_menu].item_bufor2[i]->covered,
      menus_bufors[pm->id_menu].item_bufor2[i]->bufor_len,
      menus_bufors[pm->id_menu].item_bufor2[i]->xy.y,
      menus_bufors[pm->id_menu].item_bufor2[i]->xy.x,
      menus_bufors[pm->id_menu].item_bufor2[i]->xy.y,
      menus_bufors[pm->id_menu].item_bufor2[i]->xy.x +
      menus_bufors[pm->id_menu].item_bufor2[i]->bufor_len
    );
  }
  refresh();
}
//czeka na wybor
int wait_for_user_choice(menu_t *pm){
  int user_choice;
  char user_key;
  int i;
  while(true){
    user_key = getch();
    for(i = 0; i < pm->number_of_items; i++){
      if(user_key == pm->items[i].item_name[0]){
        user_choice = pm->items[i].item_id;
        return user_choice;
      }
    }
  }
  return -1;
}

