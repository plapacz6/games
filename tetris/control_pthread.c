/* author: plapacz6@gmail.com, date: 2022-05-20 */
void user_control(){

}

void time_control(){
  while(true){
    sleep(game_state.intv);
    if(is_bottom_contact(current_figure, ptr_board)){
      return;
    }
  }
}