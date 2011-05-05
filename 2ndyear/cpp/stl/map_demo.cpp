#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <algorithm>
#include <map>
#include <string>
#include <vector>

void PrintStringMap(const std::map<std::string, std::string> &m) {
  printf("Map contents:\n");
  for (std::map<std::string, std::string>::const_iterator i = m.begin();
       i != m.end();
       ++i) {
    // i->first == Key
    // i->second == Value
    printf("  '%s' -> '%s'\n", i->first.c_str(), i->second.c_str());
  }
  printf("\n");
}

int main() {
  std::map<std::string, std::string> m;

  m["Alice"] = "1234567";
  m["Bob"] = "2345678";
  m["Colin"] = "3456789";

  PrintStringMap(m);

  // Test if the map contains a given key
  if (m.find("Alice") != m.end())
    printf("'Alice' is in map 'm'\n\n");
  else
    printf("'Alice' is not in map 'm'\n\n");

  // Remove an element by its key
  m.erase("Colin");

  // Remove an element by its iterator
  std::map<std::string, std::string>::iterator it = m.find("Bob");
  assert(it != m.end());
  m.erase(it);

  PrintStringMap(m);

  m.clear();  // clear the map
  PrintStringMap(m);

  return 0;
}
