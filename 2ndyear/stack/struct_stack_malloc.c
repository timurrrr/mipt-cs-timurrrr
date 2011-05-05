#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

struct StackNode {
  int value;
  struct StackNode *prev;
};

struct Stack {
  struct StackNode *top;
};

void initialize(struct Stack* stack) {
  assert(stack != NULL);

  stack->top = NULL;
}

void destroy(struct Stack* stack) {
  assert(stack != NULL);
  /* Check that we didn't leak memory! */
  assert(stack->top == NULL);
}

void push(struct Stack* stack, int value) {
  struct StackNode *old_top;

  assert(stack != NULL);

  old_top = stack->top;
  stack->top = (struct StackNode*)malloc(sizeof(struct StackNode));
  assert(stack->top != NULL);
  stack->top->value = value;
  stack->top->prev = old_top;
}

int pop(struct Stack* stack) {
  struct StackNode *old_top;

  assert(stack != NULL);
  assert(stack->top != NULL);

  old_top = stack->top;
  int retval = old_top->value;
  stack->top = stack->top->prev;
  free(old_top);
  return retval;
}

int is_empty(const struct Stack* stack) {
  assert(stack != NULL);

  return (stack->top == NULL);
}

int main(int argc, char **argv) {
  int i;
  struct Stack st;
  initialize(&st);
  for (i = 1; i < argc; i++) {
    push(&st, atoi(argv[i]));
  }
  while (!is_empty(&st))
    printf("%d\n", pop(&st));
  destroy(&st);
  return 0;
}
