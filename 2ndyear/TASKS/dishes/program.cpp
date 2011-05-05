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

using std::map;
using std::queue;

// Washer {{{1
struct WasherJob {
  WasherJob(unsigned int t, unsigned int q) {
    type = t;
    quantity = q;
  }
  unsigned int type, quantity;
};

queue<WasherJob> batch;

bool RunWasher(IpcInterface *ipc) {
  bool initialized = false;

  for (int i = 0; i < 10 && !initialized; i++) {
    initialized = ipc->InitializeWasher();
    usleep(10000);
  }

  if (!initialized) {
    fprintf(stderr, "ipc->InitializeWasher() failed! %s\n", strerror(errno));
    return false;
  }

  while (batch.size() > 0) {
    WasherJob current_job = batch.front();
    batch.pop();

    for (int i = 0; i < current_job.quantity; i++) {
      printf("Washer: washed plate type #%d...\n", current_job.type);
      if (!ipc->SendMessage(current_job.type)) {
        fprintf(stderr, "Washer - error! %s\n", strerror(errno));
        return false;
      }
      printf("Washer: and placed it on the table\n");
    }
  }

  // All jobs processed -> send "-1" and exit.
  printf("Washer: job done!\n");
  return ipc->SendMessage(-1);
}
// Washer }}}

// Dryer {{{1
map<char, unsigned int> dryer_time;  // type -> time (ms)

bool RunDryer(IpcInterface *ipc) {
  if (!ipc->InitializeDryer()) {
    fprintf(stderr, "ipc->InitializeDryer() failed! %s\n", strerror(errno));
    return false;
  }

  printf("Dryer: waiting for plates!\n");
  while (true) {
    char current_type;
    printf("Dryer: getting a dish...\n");
    if (!ipc->GetMessage(&current_type)) {
      fprintf(stderr, "Dryer - error! %s\n", strerror(errno));
      return false;
    }

    if (current_type == -1) {
      // "End of jobs"
      printf("Dryer: all jobs done!\n");
      break;
    }

    unsigned int sleep_time = dryer_time[current_type];
    printf("Dryer: got plate type #%d, drying...\n", current_type);
    usleep(sleep_time * 1000*100);
    printf("Dryer: done!\n");
  }

  return true;
}
// Dryer }}}

int main() {
  // TODO: read from files {{{1
  dryer_time[0] = 1;
  dryer_time[1] = 2;
  dryer_time[2] = 5;

  batch.push(WasherJob(1, 5));
  batch.push(WasherJob(2, 3));
  batch.push(WasherJob(0, 1));
  // }}}

  IpcInterface *ipc = IpcInterface::Create(1);

  pid_t child_pid = fork();
  assert(child_pid != -1);

  if (child_pid == 0) {
    // In child
    int ret = (RunDryer(ipc) != true);
    ipc->Close();
    delete ipc;
    return ret;
  }

  // In parent
  int ret = (RunWasher(ipc) != true);

  int status,
      wait_ret = waitpid(child_pid, &status, 0);
  ipc->Close();
  delete ipc;
  if (wait_ret != child_pid || !WIFEXITED(status)) {
    return 1 | ret;
  }
  return ret;
}
