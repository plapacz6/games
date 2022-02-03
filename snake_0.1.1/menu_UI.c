#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <ncurses.h>
#include "init_UI.h"
#include "menu_definitions.h"
#include "menu_UI.h"

#define MALLOC_DEBUG
  #ifdef MALLOC_DEBUG
  #include <stdio.h>  
  #define PMALLOC() {fprintf(stderr, "malloc in file %s line %d\n", __FILE__, __LINE__);}
  #else
  PMALLOC() {}
  #endif
/** description of the buffers of one menu item
 */
typedef struct {
  char *text;      //text menu's item
  int *covered;    //part of screen covered by textx
  coords_t xy;     //coordinates of the beginning of the item on the screen
  size_t bufor_len;
} menu_item_bufor2_t;

/**a description of the set of item buffers for all one menu
 */
typedef struct {
  menu_item_bufor2_t **item_bufor2; //an array of pointers to the buffers of the individual menu items
  //menu_title
  //top_screen_border_tekst
  //bottom_screen_border_tekst
} menu_items_bufors_t;

extern screen_t screen;

/** auxiliary function that loads the screen content, 
 * which will be overwritten by the menu in order to restore it 
 * after hiding the menu
 */
void read_covered_area(char *bufor){
}

/** an array of pointers for the descriptions of individual menus
 */
//#define MAX_NUMBER_OF_MENU = //<- this is defined where the menu definition is
menu_items_bufors_t menus_bufors[MAX_NUMBER_OF_MENU]; //<- instead of a list; global so reset
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


/** reads the specified (in coordinates) screen area into the buffer
 * y1, x1 - top left corner
 * y2, x2 - bottom right
 * NOTE, but it only makes sense for one line, unless
 * we know the length of the line and can manually break the buffer later
 */
int read_screen(int *buf, size_t max_buf, int y1, int x1, int y2, int x2){
   int x, y, i;
   i = 0;
   for(y = y1; y <= y2; y++){
     for(x = x1; x <= x2; x++){
       move(y,x);
       buf[i] = inch();
       i++;
       if(i > max_buf){
         return 2; //termination due to reaching the maximum buffer capacity
       }
     }
   }
   return 0; //normal termination
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
         return 2; //termination due to reaching the maximum buffer capacity
       }
     }
   }
   return 0; //normal termination
 }


/** menu initialization
 */
void create_menu(menu_t *pm){
  //ncurses menu style elements - that's why here, because they do not change
  char *MARGIN_L = "  ";
  char *MARGIN_M = " . ";
  char *MARGIN_R = "  ";
  char *MENU_ITEM_FRAME_L = ">>";
  char *MENU_ITEM_FRAME_R = "<<";
  //
  int menu_coord_y;
  size_t i;
  size_t bufor_len;

  /** Create a buffer set node for one menu.
    * HERE IS THE POINTER I think.
    * Allocate memory for an array of pointers to
    * structures describing the buffers of one menu item
    * written to a pointer on this structure.
   */
  
  PMALLOC();
  menus_bufors[pm->id_menu].item_bufor2 = malloc(pm->number_of_items *
                             sizeof(menu_item_bufor2_t*));
  //calculation of the y coordinate of the menu
  menu_coord_y = screen.ymid - (pm->number_of_items / 2) - 1;  //number of menu items + menu title
  //for each item on the menu
  for(i = 0; i < pm->number_of_items; i++){
    //calculation of the buffer length
    bufor_len = strlen(MENU_ITEM_FRAME_L) +
                strlen(MARGIN_L) +
                strlen(pm->items[i].item_name) +
                strlen(MARGIN_M) +
                strlen(pm->items[i].item_description) +
                strlen(MARGIN_R) +
                strlen(MENU_ITEM_FRAME_R) +
                + 1;
    //creating a buffer for one menu item (buffer for text and underneath)
    //!!! IT FALLS HERE. Should there be (dot) instead of (arrow) ->. ???
    PMALLOC();
    menus_bufors[pm->id_menu].item_bufor2[i] = malloc(sizeof(menu_item_bufor2_t));
    PMALLOC();
    menus_bufors[pm->id_menu].item_bufor2[i]->text = malloc(bufor_len * sizeof(int));
    PMALLOC();
    menus_bufors[pm->id_menu].item_bufor2[i]->covered = malloc(bufor_len * sizeof(int));
    menus_bufors[pm->id_menu].item_bufor2[i]->bufor_len = bufor_len;
    //set the x-coordinate of the menu item, and the y-coordinate
    menus_bufors[pm->id_menu].item_bufor2[i]->xy.x = screen.xmid - (bufor_len / 2);
    menus_bufors[pm->id_menu].item_bufor2[i]->xy.y = menu_coord_y++;
    //filling buffers with content
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
    PMALLOC();
    menus_bufors[pm->id_menu].item_bufor2[i]->covered = malloc(bufor_len * sizeof(int));
    /* at the creation stage, it makes no sense to write down the background, 
     * because it will be different when called up
     */
    read_screen(
      menus_bufors[pm->id_menu].item_bufor2[i]->covered,
      bufor_len,
      menus_bufors[pm->id_menu].item_bufor2[i]->xy.y,
      menus_bufors[pm->id_menu].item_bufor2[i]->xy.x,
      menus_bufors[pm->id_menu].item_bufor2[i]->xy.y,
      menus_bufors[pm->id_menu].item_bufor2[i]->xy.x +
      menus_bufors[pm->id_menu].item_bufor2[i]->bufor_len
    );
    /* 
    */    
  }
}

/** destructor menu */
void destroy_menu(menu_t *pm){
  /*release buffers */
  int i;
  //deletion of menu item buffers to text and background
  for(i = 0; i < pm->number_of_items; i++){
    free(menus_bufors[pm->id_menu].item_bufor2[i]->text);
    free(menus_bufors[pm->id_menu].item_bufor2[i]->covered);
    free(menus_bufors[pm->id_menu].item_bufor2[i]);
  }
  free(menus_bufors[pm->id_menu].item_bufor2);
}


//shows / hides the menu
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
//waiting for a choice
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

