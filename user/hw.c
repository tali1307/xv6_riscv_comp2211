#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  char str[20] = "hello world\n";
  printf("%s", str);
  // insert  code below to print "Hello world" to the console
  // you can use the printf function as you would normally.

  // Terminate the process
  exit(0);
}