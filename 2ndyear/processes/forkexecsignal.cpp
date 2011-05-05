#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

bool child_is_running;

void child_handler(int signum) {
  if (signum == SIGCHLD) {
    assert(child_is_running == true);
    child_is_running = false;
  }
}

void initialize_child_handler() {
  // IMPORTANT: This function MUST be called before fork()
  child_is_running = true;
  signal(SIGCHLD, child_handler);  // register signal handler
}

int main() {
  initialize_child_handler();
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
  while (child_is_running) {
    printf("==%d== Waiting for the child to finish...\n", getpid());
    usleep(100000);
  }
  printf("==%d== Child finished! Bye.\n", getpid());
  return 0;
}
