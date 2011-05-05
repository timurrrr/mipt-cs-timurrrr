#include <assert.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

const int ITERATIONS_PER_THREAD = 1000000000;  // One billion

void* thrp(void *arg) {
  int *int_arg = (int*)arg;

  printf("starting thrp() loop...\n");
  for (int i = 0; i < ITERATIONS_PER_THREAD; i++) {
    (*int_arg)++;
  }
  printf("thrp() loop finished; intermediate result = %d\n",
         *int_arg);

  return NULL;
}

int main() {
  pthread_t thr;

  int var = 0;  // Going to be shared 

  printf("main(): Before pthread_create;\n  var=%d\n", var);
  if (pthread_create(&thr, NULL, thrp, &var) != 0) {
    fprintf(stderr, "pthread_create failed\n");
    return 1;
  }

  printf("starting main() loop...\n");
  for (int i = 0; i < ITERATIONS_PER_THREAD; i++) {
    var++;
  }
  printf("main() loop finished; intermediate result = %d\n", var);

  if (pthread_join(thr, NULL) != 0) {
    fprintf(stderr, "pthread_join failed\n");
    return 1;
  }
  printf("\nResults:\n  var      = %d\n  expected = %d\n",
         var, 2*ITERATIONS_PER_THREAD);

  return 0;
}
