#include <assert.h>
#include <stdlib.h>

#include <iostream>

using namespace std;

class Stack{
  struct Node {
    int value;
    Node * prev;
  };

 public:
  Stack() : top(NULL) {}

  ~Stack() {
    /* Check that we didn't leak memory! */
    assert(top == NULL);
  }

  void push(int value) {
    Node * old_top = top;
    top = new Node();
    assert(top != NULL);
    top->value = value;
    top->prev = old_top;
  }

  int pop() {
    assert(top != NULL);
    Node *old_top = top;
    int retval = old_top->value;
    top = top->prev;
    delete old_top;
    return retval;
  }

  bool is_empty() const {
    return (top == NULL);
  }
 private:
  Node *top;
};


#if 1

int main(int argc, char **argv){
  Stack st;
  for (int i = 1; i < argc; i++) {
    st.push(atoi(argv[i]));
  }
  while (!st.is_empty())
    cout << st.pop() << "\n";
  return 0;
}

#else

void test1() {
  Stack st;
  assert(st.is_empty() == true);
  cout << __FUNCTION__ << " PASSED\n";
}

void test2() {
  Stack st;
  assert(st.is_empty() == true);
  st.push(13);
  assert(st.is_empty() == false);
  assert(st.pop() == 13);
  assert(st.is_empty() == true);
  cout << __FUNCTION__ << " PASSED\n";
}

void test3() {
  Stack st;
  assert(st.is_empty() == true);
  st.push(13);
  assert(st.is_empty() == false);
  st.push(10);
  st.push(17);
  assert(st.is_empty() == false);
  assert(st.pop() == 17);
  st.push(23);
  assert(st.pop() == 23);
  assert(st.pop() == 10);
  assert(st.pop() == 13);
  assert(st.is_empty() == true);
  cout << __FUNCTION__ << " PASSED\n";
}

int main(int argc, char **argv){
  cout << "Starting Stack tests...\n";
  test1();
  test2();
  test3();
  cout << "ALL TESTS PASSED!\n";
  return 0;
}

#endif
