#include <assert.h>
#include <stdio.h>
#include <pthread.h>

void* thread_procedure(void *arg) {
  printf("inside thread_procedure(), arg=%p\n", arg);
  return NULL;
}

int main() {
  // Get man pages:
  // sudo apt-get install manpages-posix-dev # Ubuntu
  //
  // Compile with -lpthread, like this:
  // g++ -o start_thread start_thread.cpp -lpthread
  pthread_t thr;

  int var = 42;

  printf("main(): Before pthread_create; &var=%p\n", &var);
  if (pthread_create(&thr, NULL /* special params - e.g. stack size */,
                     thread_procedure, &var /* arg */) != 0) {
    fprintf(stderr, "pthread_create failed\n");
    return 1;
  }

  printf("main(): Between pthread_create and pthread_join\n");

  if (pthread_join(thr, NULL) != 0) {
    fprintf(stderr, "pthread_join failed\n");
    return 1;
  }
  printf("main(): After pthread_join\n");

  return 0;
}
