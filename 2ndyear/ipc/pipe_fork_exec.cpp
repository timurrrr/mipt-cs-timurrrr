#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Demo: "cat <mysource> | grep pipe"
int main(int argc, char **argv) {
  int pfds[2];
  pipe(pfds);

  if (!fork()) {
    close(1);  // close stdout

    // make stdout same as pfds[1], thanks to reusing the
    // first available file descriptor
    dup(pfds[1]);
    close(pfds[0]);  // we don't need to read from pipe
    execlp("cat", "cat", "pipe_fork_exec.cpp", NULL);
  } else {
    close(0);        // close stdin
    dup(pfds[0]);    // make stdin same as pfds[0]
    close(pfds[1]);  // we don't need to write to pipe
    execlp("grep", "grep", "pipe", NULL);
  }

  return 0;
}
