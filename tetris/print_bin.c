#include "print_bin.h"

/**
 * @brief generates binary representation of number x (max length 64 bit)
 * 
 * @param x number to analize
 * @return char* pointer to buffor containing null ended string with binary representation of x
 */
static char buffor[65];
//char *print_bin(char *buffor, size_t size, int x){
const char *print_bin(int x){
  int i;
  for(i = 0; i < (sizeof(x) * 8) && i < 65; i++){
    buffor[i] = ( x & 1) + '0';    
    x = x >> 1;
  }
  buffor[i] = '\0';
  return buffor;
}