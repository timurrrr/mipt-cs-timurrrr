#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include <string>

int read_fd  = -1,
    write_fd = -1;

bool common_initialize() {
  int fd[2];
  if (pipe(fd) != 0)
    return false;
  read_fd  = fd[0];
  write_fd = fd[1];
  printf("Created pipe! read_fd = %d, write_fd = %d\n", read_fd, write_fd);
  return true;
}

int child_main() {
  // Close the write-end in child - we don't need it.
  // If it remains open, the read() won't return EOF...
  close(write_fd);

  std::string received;
  const int BUFFER_SIZE = 32;
  char buffer[BUFFER_SIZE + 1];

  int ret;
  while ((ret = read(read_fd, buffer, BUFFER_SIZE)) > 0) {
    printf("Child: read(%d, buffer, BUFFER_SIZE) returned %d\n",
           read_fd, ret);
    if (ret == -1)
      return 1;
    assert(ret > 0);
    buffer[ret] = '\0';
    printf("  buffer now holds '%s'\n", buffer);
    received += buffer;
  }

  printf("Child: received '%s'\n", received.c_str());
  close(read_fd);
  return 0;
}

int parent_main() {
  close(read_fd);  // We don't need it in the parent

  const char MESSAGE[] = "Hello there at the other side of the pipe!";

  const char *buffer = MESSAGE;
  int remaining_bytes = sizeof(MESSAGE);  // Includes '\0'

  while (remaining_bytes > 0) {
    int ret = write(write_fd, buffer, remaining_bytes);
    printf("Parent: write(%d, '%s', %d) returned %d\n",
           write_fd, buffer, remaining_bytes, ret);
    if (ret == -1) {
      fprintf(stderr, "Write error: %s\n", strerror(errno));
      return 1;
    }

    assert(ret >= 0 && ret <= remaining_bytes);
    remaining_bytes -= ret;
  }

  printf("Parent: close(write_fd)\n");
  close(write_fd);
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
