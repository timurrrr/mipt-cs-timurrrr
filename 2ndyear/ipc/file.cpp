#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>

FILE *fp = NULL;

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
  fp = fopen("shared_file", "w+");
  assert(fp != NULL);
}

void child_main() {
  printf("Child: waiting for parent to start\n");
  s2.Wait();
  const char MESSAGE[]= "Hello, world!";
  printf("Child: writing a message to file...\n");
  int message_size = strlen(MESSAGE);
  int rv = fwrite(&message_size, sizeof(message_size), 1, fp);
  assert(rv == 1);
  rv = fwrite(MESSAGE, message_size, 1, fp);
  assert(rv == 1);
  printf("Child: done; position in file = %ld\n", ftell(fp));
  s1.Signal();
}

void parent_main() {
  printf("Parent: started\n");
  s2.Signal();
  s1.Wait();
  printf("Parent: position in file = %ld\n", ftell(fp));
  fseek(fp, 0, SEEK_SET);  // go to the beginnig of the file; see also rewind().
  printf("Parent: position in file = %ld\n", ftell(fp));

  int message_size = -1;
  int rv = fread(&message_size, sizeof(message_size), 1, fp);
  assert(rv == 1);
  const int MAX_MSG_SIZE = 1024;
  assert(message_size >= 0 && message_size < MAX_MSG_SIZE);

  char buffer[MAX_MSG_SIZE];
  rv = fread(buffer, message_size, 1, fp);
  assert(rv == 1);
  buffer[message_size] = '\0';
  printf("Parent: received a '%s' message\n", buffer);
}

void parent_deinitialize() {
  fclose(fp);
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
