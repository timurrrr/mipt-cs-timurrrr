#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

template <typename T>
class Stack {
 public:
  Stack(size_t size = 20) : size_(size), written_(0) {
    data_ = new T[size_];
    assert(data_ != NULL);
  }

  ~Stack() {
    delete [] data_;
  }

  void Push(const T& value) {
    assert(written_ < size_);
    data_[written_] = value;
    written_++;
  }

  T Pop() {
    assert(written_ > 0);
    written_--;
    return data_[written_];
  }

  bool IsEmpty() const { return written_ == 0; }

 private:
  T *data_;
  size_t size_, written_;
};

int main () {
  Stack<int> int_stack;
  for (int i = 0; i < 4; i++)
    int_stack.Push(i);

  while (!int_stack.IsEmpty())
    printf("%d\n", int_stack.Pop());

  Stack<float> float_stack;
  for (int i = 0; i < 4; i++)
    float_stack.Push(sin((float)i));

  while (!float_stack.IsEmpty())
    printf("%f\n", float_stack.Pop());

  return 0;
}
