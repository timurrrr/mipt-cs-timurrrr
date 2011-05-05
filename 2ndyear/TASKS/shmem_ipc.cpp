#include "ipc_interface.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>

class Semaphore {
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
};

class ShmemIpc: public IpcInterface {
 public:
  ShmemIpc() : queue_limit_(-1),
               shmem_(-1),
               read_index_(0),
               write_index_(0),
               shared_buffer_(NULL) {}

  bool Initialize(int queue_limit) {
    if (queue_limit <= 0)
      return false;
    shmem_ = shmget(IPC_PRIVATE, queue_limit, 0600 | IPC_CREAT | IPC_EXCL);
    if (shmem_ < 0)
      return false;
    shared_buffer_ = (char*)shmat(shmem_, NULL, 0);
    assert(shared_buffer_ != (void*)-1);

    // Release the shmem ID when everyone detaches.
    int rv = shmctl(shmem_, IPC_RMID, NULL);
    assert(rv == 0);

    queue_limit_ = queue_limit;
    queue_not_empty_ = new Semaphore();
    queue_not_full_  = new Semaphore();
    for (int i = 0; i < queue_limit; i++)
      queue_not_full_->Signal();
    return true;
  }

  virtual void Close() {
    assert(queue_not_empty_ != NULL);
    if (should_free_resources_) {
      queue_not_full_->Destroy();
      queue_not_empty_->Destroy();
    }
    delete queue_not_empty_;
    delete queue_not_full_;
    queue_not_empty_ = queue_not_full_ = NULL;
  }

  virtual bool InitializeWasher() {
    return true;
  }

  // Should blocks if QUEUE_LIMIT is reached
  virtual bool SendMessage(char message) {
    queue_not_full_->Wait();
    shared_buffer_[write_index_] = message;
    write_index_ = (write_index_ + 1) % queue_limit_;
    queue_not_empty_->Signal();
    return true;
  }

  virtual bool InitializeDryer() {
    should_free_resources_ = true;
    return true;
  }

  virtual bool GetMessage(char *message) {
    queue_not_empty_->Wait();
    *message = shared_buffer_[read_index_];
    read_index_ = (read_index_ + 1) % queue_limit_;
    queue_not_full_->Signal();
    return true;
  }

 private:
  int shmem_;
  Semaphore *queue_not_empty_, *queue_not_full_;

  int queue_limit_, read_index_, write_index_;
  char *shared_buffer_;
  bool should_free_resources_;
};

IpcInterface *IpcInterface::Create(int queue_limit) {
  ShmemIpc *ret = new ShmemIpc();
  if (ret->Initialize(queue_limit))
    return ret;
  delete ret;
  return NULL;
}
