#include <assert.h>
#include <pthread.h>
#include <stdio.h>

class Thread {
 public:
  Thread(void (*worker)(void *), void *arg = NULL)
      :w_(worker), arg_(arg) {}

  ~Thread() {}

  void Start() {
    int create_ret = pthread_create(&t_, NULL,
                                    (void* (*)(void*))ThreadBody,
                                    this);
    assert(create_ret == 0);
  }

  void Join()  {
    int join_ret = pthread_join(t_, NULL);
    assert(join_ret == 0);
  }

  pthread_t tid() const { return t_; }

  // Java-like:
  // virtual void Run() = 0;

 private:
  static void ThreadBody(Thread *thr) {
    (thr->w_)(thr->arg_);
    // thr->Run();  // Java-like
  }

  pthread_t t_;

  void (*w_)(void*);
  void *arg_;
};

void ThreadFunction(void *) {
  printf("Hello!\n");
}

int main() {
  Thread t(ThreadFunction);
  printf("Before start\n");
  t.Start();
  t.Join();
  printf("After join\n");
  return 0;
}
