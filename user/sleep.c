#include "kernel/types.h"
#include "user/user.h"

//sleep program

int main(int argc, char *argv[]) {
  int ticks;

  ticks = atoi(argv[1]);
  printf("ticks = %d\n", ticks);
  sleep(ticks);
  
  exit(0);
}