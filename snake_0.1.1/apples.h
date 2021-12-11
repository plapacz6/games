#ifndef APPLES_H
#define APPLES_H
#include "game_logic.h"
typedef struct apple_tt{
  int x;
  int y;
} apple_t;

typedef struct apples_tt{
  apple_t a[MAX_NUMBER_APPLES];
  size_t len;
} apples_t;

void init_apples();

#endif // APPLES_H
