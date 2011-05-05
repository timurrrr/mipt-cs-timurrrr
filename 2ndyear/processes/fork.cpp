#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
  pid_t child_pid = fork();
  printf("==%d== After fork()\n", getpid());
  if (child_pid == 0) {
    // In child
    printf("==%d== In child. PPID = %d\n",
           getpid(), getppid());
  } else {
    // In parent
    printf("==%d== In parent. child_pid = %d\n",
           getpid(), child_pid);
  }
  printf("==%d== End\n", getpid());
  //usleep(1000000);
  return 0;
}
