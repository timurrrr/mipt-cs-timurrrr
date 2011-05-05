#include <stdio.h>

#include <algorithm>
#include <vector>

class MyClass {
 public:
  explicit MyClass(int v) { value = v; }

  int value;

  // {{{1
  // By the way, this if forbidden by Google C++ Code Style Guide
  bool operator< (const MyClass &other) const {
    return value < other.value;
  }
  // }}}
};

// {{{1
struct MyReverseComparator {
  bool operator() (const MyClass &a, const MyClass &b) {
    return (a.value > b.value);
  }
} myobject;
// }}}

bool MyCompareFunction(const MyClass &a, const MyClass &b) {
  return (a.value < b.value);
}

int main() {
  std::vector<MyClass> v;

  printf("Initializing v[]...\n");
  v.push_back(MyClass(4));
  v.push_back(MyClass(2));
  v.push_back(MyClass(3));
  v.push_back(MyClass(1));

  printf("\nSorting using MyCompareFunction\n");
  sort(v.begin(), v.end(), MyCompareFunction);
  for (int i = 0; i < v.size(); i++)
    printf("%d, ", v[i].value);
  printf("<END>\n");

  printf("\nSorting using MyReverseComparator\n");
  sort(v.begin(), v.end(), MyReverseComparator());
  for (int i = 0; i < v.size(); i++)
    printf("%d, ", v[i].value);
  printf("<END>\n");

  printf("\nSorting using MyClass::operator<\n");
  sort(v.begin(), v.end());
  for (int i = 0; i < v.size(); i++)
    printf("%d, ", v[i].value);
  printf("<END>\n");

  return 0;
}
