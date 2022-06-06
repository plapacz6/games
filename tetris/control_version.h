/* author: plapacz6@gmail.com, date: 2022-05-20 */

#define CONTROL_LOOP
//#define CONTROL_THREAD
//#define CONTOLO_SIGNAL

#ifdef CONTROL_LOOP
void user_control();
void time_control();
#endif //CONTROL_LOOP

#ifdef CONTROL_THREAD
void user_control();
void time_control();
#endif // CONTROL_THREAD

#ifdef CONTROL_SIGNAL
void user_control();
void time_control();
#endif //CONTROL_SIGNAL
