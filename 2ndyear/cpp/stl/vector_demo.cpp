#include <stdio.h>

#include <algorithm>
#include <vector>

int main() {
  std::vector<int> v;

  printf("push_back x6\n");
  v.push_back(1);
  v.push_back(1);
  v.push_back(2);
  v.push_back(3);
  v.push_back(5);
  v.push_back(8);

  // size
  printf("v.size() = %d\n", v.size());

  // operator[]
  printf("\noperator[] demo\n");
  for (int i = 0; i < v.size(); i++) {
    int value_i = v[i];
    printf("v[%d] = %d\n", i, value_i);
    // you can write to v[i], e.g. "v[i] = 42"
  }

  // Shuffle in a single function call!
  printf("\nshuffling the contents of v...\n");
  random_shuffle(v.begin(), v.end());

  // iterator, begin, end
  printf("\niterator demo\n");
  for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it) {
    int value_i = *it;
    printf("%d, ", value_i);
    // you can write to *it, e.g. "*it = 42;"
  }
  printf("<END>\n");

  // Sort
  printf("\nsort(v.begin(), v.end());\n");
  sort(v.begin(), v.end());
  for (int i = 0; i < v.size(); i++)
    printf("%d, ", v[i]);
  printf("<END>\n");

  // Reverse sort!
  printf("\nsort(v.rbegin(), v.rend());\n");
  sort(v.rbegin(), v.rend());
  for (int i = 0; i < v.size(); i++)
    printf("%d, ", v[i]);
  printf("<END>\n");

  printf("\nDONE\n");
  return 0;
}
