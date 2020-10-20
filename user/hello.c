#include "kernel/types.h"
#include "user/user.h"

//hello "name" program

int main(int argc, char *argv[]) {
  char name[50];

  printf("What is your name?");
  gets(name, 50);
  printf("Hello %s\n", name);
  
  exit(0);
}