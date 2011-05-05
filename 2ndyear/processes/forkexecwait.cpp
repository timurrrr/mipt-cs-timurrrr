#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
  pid_t child_pid = fork();
  printf("==%d== After fork()\n", getpid());
  if (child_pid == 0) {
    // In child
    char* const args[] = {"echo", "1", "2", "3", NULL};
    execvp(args[0], args);
    // Shouldn't continue on success.
    printf("Error running '%s'\n", args[0]);
    abort();
  } else {
    // In parent
    printf("==%d== In parent. child_pid = %d\n",
           getpid(), child_pid);
  }

  printf("==%d== Waiting for child to complete...\n", getpid());
  int status,
      ret = waitpid(child_pid, &status, 0);
  if (ret != child_pid) {
    // Error during waitpid
    printf("==%d== Hm, waitpid returned %d?..\n", getpid(), ret);
    return 1;
  }
  if (!WIFEXITED(status)) {
    // Exited abnormally
    printf("==%d== Hm, WIFEXITED == false!\n", getpid());
    return 1;
  }
  printf("==%d== DONE\n", getpid());
  return 0;
}
