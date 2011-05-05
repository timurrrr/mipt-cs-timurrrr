#include <assert.h>
#include <stdio.h>

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

template<typename T> class MyArray {
 public:
  explicit MyArray(int size) : size_(size) {
    assert(size > 0);
    elements_ = new T[size];
    assert(elements_ != NULL);
  }

  ~MyArray() {
    delete [] elements_;
  }

  T& operator[](int index) {
    assert(index >= 0 && index < size_);
    return elements_[index];
  }

  int size() { return size_; }

 private:
  T *elements_;
  int size_;
  DISALLOW_COPY_AND_ASSIGN(MyArray);
};

int main() {
  // Encapsulation:
  // You can only access class data via interface (public methods/operators).
  // That guarantees that the class fields are always in correct state
  // (e.g. number of |elements_| == size_)
  MyArray<int> array(6);
  // OR std::vector<int> array(6);
  array[0] = 1;
  array[1] = 1;
  array[2] = 2;
  array[3] = 3;
  array[4] = 5;
  array[5] = 8;

  /*
  MyArray<float> array_f(10);
  MyArray<Socket*> array_sockets(...);
  */

  for (int i = 0; i < array.size(); i++)
    printf("%s%d", ((i > 0) ? ", " : ""), array[i]);
  printf("\n");

  return 0;
}
