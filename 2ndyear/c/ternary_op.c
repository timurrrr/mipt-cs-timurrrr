#include <stdio.h>

int main(void) {
  int x = 3,
      y = 2;
  int r = (x % 2) ? 3 : 2;
  printf("r = %d\n", r);
  return 0;
}
