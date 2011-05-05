#include "ipc_interface.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

class MessageQueue { // {{{ 1
 public:
  MessageQueue() {
    destroyed_ = false;
    queue_ = msgget(IPC_PRIVATE, 0600 | IPC_CREAT | IPC_EXCL);
    assert(queue_ != -1);
  }

  ~MessageQueue() {
  }

  bool Send(char message) {
    assert(destroyed_ == false);

    struct Message msg;
    msg.type = 1;
    msg.data = message;

    // the "length(3)" parameter doesn't include bytes for 'type'.
    int rv = msgsnd(queue_, &msg, 1, 0);
    return (rv == 0);
  }

  bool Receive(char *message) {
    assert(destroyed_ == false);

    struct Message msg;
    int rv = msgrcv(queue_, &msg, 1, 0, 1);
    if (rv < 0)
      return false;
    assert(rv == 1);
    *message = msg.data;
    return true;
  }

  void Destroy() {
    assert(destroyed_ == false);
    int rv = msgctl(queue_, IPC_RMID, NULL);  // Destroy the message queue.
    assert(rv == 0);
    destroyed_ = true;
  }

 private:
  struct Message {
    long type;
    char data;
  };
  bool destroyed_;
  int queue_;
}; // }}}

class MessageQueueIpc: public IpcInterface {
 public:
  MessageQueueIpc() {}

  bool Initialize(int queue_limit) {
    if (queue_limit <= 0)
      return false;
    for (int i = 0; i < queue_limit; i++)
      empty_slots_.Send(EMPTY_SLOT);
    return true;
  }

  virtual void Close() {
    if (should_free_resources_) {
      empty_slots_.Destroy();
      dishes_.Destroy();
    }
  }

  virtual bool InitializeWasher() {
    return true;
  }

  // Should block if QUEUE_LIMIT is reached
  virtual bool SendMessage(char message) {
    char tmp = -1;
    if (!empty_slots_.Receive(&tmp))
      return false;
    assert(tmp == EMPTY_SLOT);
    return dishes_.Send(message);
  }

  virtual bool InitializeDryer() {
    should_free_resources_ = true;
    return true;
  }

  virtual bool GetMessage(char *message) {
    if (!dishes_.Receive(message))
      return false;
    return empty_slots_.Send(EMPTY_SLOT);
  }

 private:
  static const int EMPTY_SLOT = 42;
  MessageQueue dishes_, empty_slots_;
  bool should_free_resources_;
};

IpcInterface *IpcInterface::Create(int queue_limit) {
  MessageQueueIpc *ret = new MessageQueueIpc();
  if (ret->Initialize(queue_limit))
    return ret;
  delete ret;
  return NULL;
}
