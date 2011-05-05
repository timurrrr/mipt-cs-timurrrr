#include <assert.h>
#include <stdio.h>

#include <queue>

class CallMyMethodPlease {
 public:
  void ThisMethod() {
    printf("Bingo!\n");
  }
};

#if 0
template<class T>
void CallClassMethod(T *object, void (T::*method_ptr)()) {
  printf("Let's call a callback?\n");
  (object->*method_ptr)();
  printf("Done!\n");
}

int main() {
  CallMyMethodPlease object;
  CallClassMethod(&object, &CallMyMethodPlease::ThisMethod);
  return 0;
}
#else

class Callback {
 public:
  virtual void Run() = 0;
};

template<class T>
class ClassMethodCallback : public Callback {
 public:
  ClassMethodCallback(T *object, void (T::*method_ptr)())
      : object_(object), method_ptr_(method_ptr) {
    assert(object_ != NULL);
    assert(method_ptr_ != NULL);
  }

  virtual void Run() {
    (object_->*method_ptr_)();
  }

 private:
  T *object_;
  void (T::*method_ptr_)();
};

class CallbackQueue {
  // this is NOT a thread-safe queue!
 public:
  void Push(Callback *cb) {
    assert(cb != NULL);
    messages_.push(cb);
  }

  Callback* Pop() {
    if (messages_.empty())
      return NULL;
    Callback *ret = messages_.front();
    messages_.pop();
    return ret;
  }

 private:
  std::queue<Callback*> messages_;
};

int main() {
  CallbackQueue queue;

  CallMyMethodPlease object;
  queue.Push(new ClassMethodCallback<CallMyMethodPlease>
                     (&object, &CallMyMethodPlease::ThisMethod));
  printf("Posted a callback on the message queue\n");
  // ...

  Callback *cb;
  while ((cb = queue.Pop()) != NULL) {
    // Make sure the object is not deleted yet!
    printf("Will run the callback now\n");
    cb->Run();
    delete cb;
  }
  printf("Done!\n");
}

#endif
