#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <algorithm>
#include <set>
#include <string>
#include <vector>

void PrintStringSet(const std::set<std::string> &s) {
  for (std::set<std::string>::const_iterator i = s.begin(); i != s.end(); ++i) {
    printf("%s'%s'", ((i == s.begin()) ? "" : ", "), i->c_str());
  }
  printf("\n\n");
}

int main() {
  std::set<std::string> s;

  s.insert("abcd");
  s.insert("1234");
  s.insert("zxcv");
  s.insert("dfgs");
  s.insert("dfgs");

  PrintStringSet(s);

  s.clear();  // Empty the set

  // Inserting the contents of vector<T> into set<T>
  std::vector<std::string> v;
  v.push_back("abcd");
  v.push_back("1234");
  v.push_back("zxcv");
  v.push_back("dfgs");

  s.insert(v.begin(), v.end());
  PrintStringSet(s);

  // Test if the set contains a given value
  if (s.find("1234") != s.end())
    printf("'1234' is in set 's'\n\n");
  else
    printf("'1234' is not in set 's'\n\n");

  // Remove an element by its value
  s.erase("zxcv");

  // Remove an element by its iterator
  std::set<std::string>::iterator it = s.find("1234");
  assert(it != s.end());
  s.erase(it);

  PrintStringSet(s);

  // In <algorithm> there are
  // std::set_union
  // std::set_intersection
  // std::set_difference
  s.insert("dfgs");
  s.insert("zxcv");
  s.insert("1234");
  printf("s = ");
  PrintStringSet(s);

  std::set<std::string> z;
  z.insert("dfgs");
  z.insert("sadfsadf");
  printf("z = ");
  PrintStringSet(z);


  std::set<std::string> result_set;
  // result_set = s\z;
  std::set_difference(s.begin(), s.end(),
                      z.begin(), z.end(),
                      std::inserter(result_set, result_set.begin()));
  printf("s\\z = ");
  PrintStringSet(result_set);
  return 0;
}
