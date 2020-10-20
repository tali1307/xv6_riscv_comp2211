#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

//sleep program

int main(int argc, char *argv[]) {
  int ticks, n, sum, n1 = 0, n2 = 1;

  n = atoi(argv[1]);
  char filename[100] = "fib_";
  strcpy(filename+4, argv[1]);
  int fd = open(filename, O_CREATE | O_WRONLY);
  printf("Fibonacci of %d numbers:", n);
  for (int i = 0; i < n; i++) {
      printf("%d, ", n1);
      fprintf(fd, "%d, ", n1);
      sum = n1 + n2;
      n1 = n2;
      n2 = sum;
  }
  close(fd);
  ticks = uptime();
  printf("\nTicks  = %d\n", ticks);
  
  exit(0);
}