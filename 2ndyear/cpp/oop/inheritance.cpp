#include <stdio.h>

class Parent {
 public:
  explicit Parent(int value = 0) {
    printf("Parent::Parent(), value = %d\n", value);
    value_ = value;
  }
  void ParentMethod() {
    printf("Parent, value_ = %d\n", value_);
  }
 private:
  int value_;
};

// ": public Parent" means
// "Child should inherent Parent"
class Child : public Parent {
 public:
  // ": Parent(5)" means
  // "call Parent constructor with (5) as argument
  // before executing Child constructor"
  Child() : Parent(5) {}

  void ChildMethod() {
    printf("Child\n");
  }
};

int main() {
  Parent a;
  printf("a.ParentMethod():\n  ");
  a.ParentMethod();
  // a.ChildMethod();  // <- can't compile!

  printf("\n");

  Child b;
  printf("b.ParentMethod():\n  ");
  b.ParentMethod();
  printf("b.ChildMethod():\n  ");
  b.ChildMethod();

  Parent *p = &b;  // You can always up-cast Child* to Parent*
  printf("p->ParentMethod():\n  ");
  p->ParentMethod();
  return 0;
}
