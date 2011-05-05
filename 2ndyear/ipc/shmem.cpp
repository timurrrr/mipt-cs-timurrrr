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

int shmem_id;
char * shared_memory;

class Semaphore { // {{{ 1
 public:
  Semaphore() {
    destroyed_ = false;
    sem_ = semget(IPC_PRIVATE, 1, 0600 | IPC_CREAT | IPC_EXCL);
    assert(sem_ != -1);
  }

  ~Semaphore() {
  }

  void Signal() {
    assert(destroyed_ == false);

    struct sembuf mybuf;
    mybuf.sem_op = 1;
    mybuf.sem_flg = 0;
    mybuf.sem_num = 0;
    int rv = semop(sem_, &mybuf, 1);
    assert(rv == 0);
  }

  void Wait() {
    assert(destroyed_ == false);

    struct sembuf mybuf;
    mybuf.sem_op = -1;
    mybuf.sem_flg = 0;
    mybuf.sem_num = 0;
    int rv = semop(sem_, &mybuf, 1);
    assert(rv == 0);
  }

  void Destroy() {
    assert(destroyed_ == false);
    int rv = semctl(sem_, 0, IPC_RMID, 0);  // Destroy semaphore
    assert(rv == 0);
    destroyed_ = true;
  }

 private:
  bool destroyed_;
  int sem_;
}; // }}}
Semaphore s1, s2;

void common_initialize() {
  // Create a new shmem ID
  shmem_id = shmget(IPC_PRIVATE, 64 * 1024, 0600 | IPC_CREAT | IPC_EXCL);
  assert(shmem_id > 0);

  // Attach shared_memory to shmem_id in the current process.
  shared_memory = (char*)shmat(shmem_id, NULL, 0);
  assert(shared_memory != (void*)-1);
  *shared_memory = '\0';

  // Release the shmem ID when everyone detaches.
  int rv = shmctl(shmem_id, IPC_RMID, NULL);
  assert(rv == 0);
}

void child_main() {
  printf("Child - shared memory contents: '%s'\n", shared_memory);
  s2.Wait();
  strcpy(shared_memory, "Hello, world!");
  printf("Child - shared memory contents: '%s'\n", shared_memory);
  s1.Signal();
  int rv = shmdt(shared_memory);  // Detach current process from shared_memory.
  assert(rv == 0);
}

void parent_main() {
  printf("Parent - shared memory contents: '%s'\n", shared_memory);
  s2.Signal();
  s1.Wait();
  printf("Parent - shared memory contents: '%s'\n", shared_memory);
  int rv = shmdt(shared_memory);  // Detach current process from shared_memory.
  assert(rv == 0);
}

void parent_deinitialize() {
  s1.Destroy();
  s2.Destroy();
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
