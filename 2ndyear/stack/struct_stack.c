#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define STACK_SIZE 10

struct Stack {
  /* Index of the element which will contain the next push'ed value. */
  int cursor;
  int array[STACK_SIZE];
};

void initialize(struct Stack* stack) {
  assert(stack != NULL);

  stack->cursor = 0;
}

void push(struct Stack* stack, int value) {
  assert(stack != NULL);
  assert(stack->cursor < STACK_SIZE);
  assert(stack->cursor >= 0);  /* Can't happen if stack_cursor is only accessed
                                  from push, pop and is_empty */
  stack->array[stack->cursor++] = value;
}

int pop(struct Stack* stack) {
  assert(stack != NULL);
  assert(stack->cursor > 0);

  return stack->array[--stack->cursor];
}

int is_empty(const struct Stack* stack) {
  assert(stack != NULL);

  return (stack->cursor > 0) ? 0 : 1;
}

int main(int argc, char **argv){
  int i;
  struct Stack st;
  initialize(&st);
  for (i = 1; i < argc; i++) {
    push(&st, atoi(argv[i]));
  }
  while (!is_empty(&st))
    printf("%d\n", pop(&st));
  return 0;
}
