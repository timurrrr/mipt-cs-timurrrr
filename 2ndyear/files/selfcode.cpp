#include <assert.h>
#include <stdio.h>

int main() {
  FILE *fp = fopen("selfcode.cpp", "rt");
  assert(fp != NULL);
  while (!feof(fp)) {
    printf("%c", fgetc(fp));
  }
  fclose(fp);
}
