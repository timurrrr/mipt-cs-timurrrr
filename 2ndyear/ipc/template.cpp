#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void common_initialize() {
}

void child_main() {
}

void parent_main() {
}

int main() {
  common_initialize();

  pid_t child_pid = fork();
  if (child_pid == 0) {
    // In child
    return child_main();
  }

  // In parent
  int ret = parent_main();

  int status,
      wait_ret = waitpid(child_pid, &status, 0);
  if (wait_ret != child_pid || !WIFEXITED(status)) {
    return 1 | ret;
  }
  return ret;
}
