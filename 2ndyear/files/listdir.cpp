#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include <string>

#define PATH_DELIMETER "/"

int main() {
  char *my_path = realpath(".", NULL);
  printf("My path = '%s'\n\n", my_path);
  free(my_path);  // !!!

  DIR *dir = opendir(".");  // we could have used my_path as well
  assert(dir != NULL);

  printf("Directory contents:\n");
  for (dirent *entry = readdir(dir); entry != NULL; entry = readdir(dir)) {
    std::string entry_name = entry->d_name;  // Relative file or directory name
    printf("%20s", entry_name.c_str());

    // d_name holds just the file/directory name _relative_ to the argument
    // given to opendir! If you need to access A/B/C/{entry_name}, don't forget
    // to prepend "A/B/C/" !!!
    // "man 2 stat"
    struct stat entry_stat;
    if (stat(entry_name.c_str(), &entry_stat) == 0) {
      printf(" - ");
      if (S_ISDIR(entry_stat.st_mode))
        printf("<DIR>");
      else
        printf("     ");

      // Print the modification time.
      // ctime returns a pointer to global array (D'oh!) so no need to free()
      char *mod_time = ctime(&entry_stat.st_mtime);
      mod_time[strlen(mod_time) - 1] = '\0';  // Ugly hack to cut trailing '\n'
      printf(" %s", mod_time);
    }

    printf("\n");
  }

  closedir(dir);

  // Check that file "listdir.cpp" exists in the current directory
  printf("\nFile 'listdir.cpp' %s in the current directory\n",
         (access("listdir.cpp", F_OK) == 0) ? "exists" : "doesn't exist");

  // Try to open a file that doesn't exist.
  FILE *fp = fopen("ASADASDSA", "r");
  assert(fp == NULL);
  fprintf(stderr, "\nError! %s\n", strerror(errno));

  return 0;
}

