#include <stdio.h>

// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

class Foo {
 public:
  Foo() {
    array_ = new int[10];
    printf("array_ = new int[10];  // returned %p\n", array_);
  }
  ~Foo() {
    printf("delete [] array_;  // array_ = %p\n", array_);
    delete [] array_;
  }

 private:
  int *array_;
#if 0
  // Uncommenting this will make wrong usage impossible.
  DISALLOW_COPY_AND_ASSIGN(Foo);
#endif
};

void PassFooByValue(Foo f) {
}

int main() {
  Foo a,
      b = a;  // This line calls operator=.
  PassFooByValue(a);  // This line calls copy constructor.
  // We end up calling delete [] three times on the same address. Boom.
  return 0;
}
