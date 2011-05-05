#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#include <string>

int shmem_id;
char * shared_memory;

class MessageQueue { // {{{ 1
 public:
  MessageQueue() {
    destroyed_ = false;
    queue_ = msgget(IPC_PRIVATE, 0600 | IPC_CREAT | IPC_EXCL);
    assert(queue_ != -1);
  }

  ~MessageQueue() {
  }

  void Send(const std::string &message) {
    assert(destroyed_ == false);
    assert(message.size() < Message::BUFFER_SIZE);

    struct Message msg;
    msg.type = 1;
    strcpy(msg.data, message.c_str());

    // the "length(3)" parameter doesn't include bytes for 'type'.
    int rv = msgsnd(queue_, &msg, message.size(), 0);
    assert(rv == 0);
  }

  void Receive(std::string *message) {
    assert(destroyed_ == false);

    struct Message msg;
    /*
       type == 0: get any message, FIFO
       type = n > 0: get any message with type=n, FIFO
       type = -n < 0: get a message with the least type <= n, FIFO for that type.

       Blocks if the queue is empty.
     */
    int rv = msgrcv(queue_, &msg,
                    Message::BUFFER_SIZE - 1 /* !!! */,
                    0 /* type */, 0);
    assert(rv > 0);
    msg.data[rv] = '\0';
    *message = msg.data;
  }

  void Destroy() {
    assert(destroyed_ == false);
    int rv = msgctl(queue_, IPC_RMID, NULL);  // Destroy the message queue.
    assert(rv == 0);
    destroyed_ = true;
  }

 private:
  struct Message {
    static const int BUFFER_SIZE = 1024;  // Up to 4080

    long type;
    char data[BUFFER_SIZE];
  };
  bool destroyed_;
  int queue_;
}; // }}}

MessageQueue message_queue;

void common_initialize() {
}

void child_main() {
  printf("Client: sending a message to the parent...\n");
  message_queue.Send("Hello, world!");
}

void parent_main() {
  printf("Parent: waiting for a message from the child...\n");
  std::string str;
  message_queue.Receive(&str);
  printf("Parent: received a '%s' message.\n", str.c_str());
}

void parent_deinitialize() {
  message_queue.Destroy();
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
