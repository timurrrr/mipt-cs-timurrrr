#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <string>

std::string ReadFileToString(const char *filename) {
  const int BUFFER_SIZE = 128;
  char buffer[BUFFER_SIZE + 1];  // "+ 1" is VERY important! See below.
  FILE *fp = fopen(filename, "r");
  assert(fp != NULL);

  std::string result;
  while(!feof(fp) && !ferror(fp)){
    int i = fread(buffer, sizeof(char), BUFFER_SIZE, fp);
    // Write '\0' after the bytes read so we can use buffer
    // as a C string.
    // VERY important! buffer size is "BUFFER_SIZE + 1",
    // otherwise you corrupt memory.
    buffer[i] = '\0';
    result += buffer;
  }
  fclose(fp);

  return result;
}

using namespace std;

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("Usage: %s <file to read>\n", argv[0]);
    return 1;
  }
  string s = ReadFileToString(argv[1]);
  cout << s;
  return 0;
}
