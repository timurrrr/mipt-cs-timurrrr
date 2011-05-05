#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include <map>
#include <queue>

#include "ipc_interface.h"

#define CHECK(x) do {\  // {{{1
      if ((x) == 0) { \
        fprintf(stderr, "\"%s\" failed at %s:%d\n", \
                #x, __FILE__, __LINE__); \
        abort(); \
      } else { \
        printf("  \"%s\" - OK\n", #x); \
      } \
    } while (0)
// }}}

void test1() {
  printf("Testing the InitializeCommon factory method");
  IpcInterface *ipc = IpcInterface::InitializeCommon(10);
  CHECK(ipc != NULL);
  delete ipc;
}

void test2() {
  printf("Testing the other methods\n");
  IpcInterface *ipc = IpcInterface::InitializeCommon(10);
  assert(ipc != NULL);

  CHECK(ipc->InitializeServer());
  CHECK(ipc->InitializeClient());

  CHECK(ipc->SendMessage(42));
  char msg;
  CHECK(ipc->GetMessage(&msg));
  CHECK(msg == 42);

  CHECK(ipc->NotifyMessageProcessed());
  ipc->Close();

  delete ipc;
}

int main() {
  printf("Running IpcInterface tests...\n");
  test1();
  test2();
  printf("PASS\n");
  return 0;
}
