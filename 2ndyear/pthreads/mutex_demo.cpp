#include <assert.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

class Mutex {
 public:
  Mutex()  { pthread_mutex_init(&mutex_, NULL); }
  ~Mutex() { pthread_mutex_destroy(&mutex_); }
  void Lock()   { pthread_mutex_lock(&mutex_); }
  void Unlock() { pthread_mutex_unlock(&mutex_); }

 private:
  pthread_mutex_t mutex_;
};

const int ITERATIONS_PER_THREAD = 1000000000;  // One billion
const int ITERATIONS_PER_BURST  = 1000;

Mutex mu;

void* thrp(void *arg) {
  int *int_arg = (int*)arg;

  printf("starting thrp() loop...\n");
  for (int i = 0;
       i < ITERATIONS_PER_THREAD / ITERATIONS_PER_BURST;
       i++) {
    mu.Lock();
    for (int j = 0; j < ITERATIONS_PER_BURST; j++) {
      (*int_arg)++;
    }
    mu.Unlock();
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
  for (int i = 0;
       i < ITERATIONS_PER_THREAD / ITERATIONS_PER_BURST;
       i++) {
    mu.Lock();
    for (int j = 0; j < ITERATIONS_PER_BURST; j++) {
      var++;
    }
    mu.Unlock();
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
