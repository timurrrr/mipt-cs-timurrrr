#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <string>

// Returns false in case of failure, changes *result only on success.
bool ReadFileToString(const char *filename, /*OUT*/ std::string * const result) {
  assert(filename != NULL);
  assert(result != NULL);

  const int BUFFER_SIZE = 128;
  char buffer[BUFFER_SIZE + 1];  // "+ 1" is VERY important! See below.
  FILE *fp = fopen(filename, "r");
  if (!fp)
    return false;

  bool ret;
  std::string local_result;  // So we don't change *result in case of failure
  while(!feof(fp) && !ferror(fp)){
    int i = fread(buffer, sizeof(char), BUFFER_SIZE, fp);
    // Write '\0' after the bytes read so we can use buffer
    // as a C string.
    // VERY important! buffer size is "BUFFER_SIZE + 1",
    // otherwise you corrupt memory.
    buffer[i] = '\0';
    local_result += buffer;
  }
  if (!ferror(fp)) {
    *result = local_result;
    ret = true;
  } else {
    ret = false;
  }
  fclose(fp);

  return ret;
}

using namespace std;

int main(int argc, char** argv){
  if (argc != 2) {
    printf("Usage: %s <file to read>\n", argv[0]);
    return 1;
  }
  string s;
  if (!ReadFileToString(argv[1], &s)) {
    printf("Error reading file '%s'\n", argv[1]);
    return 1;
  }
  cout << s;
  return 0;
}
