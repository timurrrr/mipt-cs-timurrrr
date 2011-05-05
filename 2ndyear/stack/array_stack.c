#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define STACK_SIZE 10

/* Index of the element which will contain the next push'ed value. */
int stack_cursor = 0;

int stack_array[STACK_SIZE];

void push(int value) {
  assert(stack_cursor < STACK_SIZE);
  assert(stack_cursor >= 0);  /* Can't happen if stack_cursor is only accessed
                                 from push, pop and is_empty */
  stack_array[stack_cursor++] = value;
}

int pop(void) {
  assert(stack_cursor <= STACK_SIZE);
  assert(stack_cursor > 0);

  return stack_array[--stack_cursor];
}

int is_empty(void) {
  return (stack_cursor > 0) ? 0 : 1;
}

int main(int argc, char **argv){
  int i;
  for (i = 1; i < argc; i++) {
    push(atoi(argv[i]));
  }
  while (!is_empty())
    printf("%d\n", pop());
  return 0;
}
