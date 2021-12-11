#include <stdio.h>
#include <stdlib.h>
#include "init_UI.h"
#include "game_logic.h"

int main(){
    init_UI();
    game_start();
    end_UI();
    return 0;
}
