#include <assert.h>
#include <stdio.h>

void my_foo(int i) {
  printf("%d\n", i);
}

void call_foo(void (*foo_ptr)(int)) {
  (*foo_ptr)(1);
  (*foo_ptr)(2);
  (*foo_ptr)(3);
}

int main(void) {
  call_foo(my_foo);
  return 0;
}

