#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>

#include "ipc_interface.h"

IpcInterface *ipc = NULL;

void common_initialize() {
  // Create a new IPC
  ipc = IpcInterface::Create(1);
  assert(ipc != NULL);
}

void child_main() {
  // You should check the return values more accuretely (without 'assert').
  int rv = ipc->InitializeWasher();
  assert(rv == true);
  printf("Child - sending 'x' message\n");
  rv = ipc->SendMessage('x');
  assert(rv == true);
  ipc->Close();
  delete ipc;
  ipc = NULL;
}

void parent_main() {
  int rv = ipc->InitializeDryer();
  assert(rv == true);
  printf("Parent - waiting for an incoming message\n");
  char msg = -1;
  rv = ipc->GetMessage(&msg);
  assert(rv == true);
  printf("Parent - received '%c' message\n", msg);
  assert(msg == 'x');
}

void parent_deinitialize() {
  ipc->Close();
  delete ipc;
  ipc = NULL;
}

int main() {
  common_initialize();

  pid_t child_pid = fork();
  if (child_pid == 0) {
    // In child
    child_main();
    return 0;
  }

  // In parent
  parent_main();

  int status,
      wait_ret = waitpid(child_pid, &status, 0);
  parent_deinitialize();
  if (wait_ret != child_pid || !WIFEXITED(status)) {
    return 1;
  }
  return 0;
}
