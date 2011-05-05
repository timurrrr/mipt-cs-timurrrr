#include "ipc_interface.h"

#include <assert.h>
#include <stdio.h>
#include <unistd.h>

class Pipe {
 public:
  Pipe() : read_fd_(-1), write_fd_(-1) { }

  ~Pipe() {
    Close();
  }

  void Close() {
    if (read_fd_ != -1)
      close(read_fd_);
    if (write_fd_ != -1)
      close(write_fd_);
    read_fd_ = write_fd_ = -1;
  }

  bool Initialize() {
    int fd[2];
    if (pipe(fd) != 0)
      return false;
    read_fd_  = fd[0];
    write_fd_ = fd[1];
    return true;
  }

  bool CopyFrom(const Pipe &other) {
    assert(read_fd_ == -1);
    assert(write_fd_ == -1);
    read_fd_ = other.read_fd_;
    write_fd_ = other.write_fd_;
  }

  void SealReadEnd() {
    assert(read_fd_ != -1);
    close(read_fd_);
    read_fd_ = -1;
  }

  bool SendByte(char b) {
    assert(write_fd_ != -1);
    return (write(write_fd_, &b, 1) == 1);
  }

  void SealWriteEnd() {
    assert(write_fd_ != -1);
    close(write_fd_);
    write_fd_ = -1;
  }

  bool ReceiveByte(char *b) {
    assert(read_fd_ != -1);
    return (read(read_fd_, b, 1) == 1);
  }
 private:
  int read_fd_, write_fd_;
};

class PipeIpc: public IpcInterface {
 public:
  PipeIpc() : num_processed_(0), num_sent_(0) {}
  bool Initialize(int queue_limit) {
    if (queue_limit <= 0)
      return false;
    queue_limit_ = queue_limit;

    return (dishes_.Initialize() && results_.Initialize());
  }

  virtual void Close() {
    dishes_.Close();
    results_.Close();
  }

  virtual bool InitializeWasher() {
    return true;
  }

  // Should blocks if QUEUE_LIMIT is reached
  virtual bool SendMessage(char message) {
    while (num_sent_ >= num_processed_ + queue_limit_) {
      // Wait for server to process one message
      char tmp;
      if (!results_.ReceiveByte(&tmp)) {
        printf("ASDASD\n");
        return false;
      }
      num_processed_ ++;
    }

    num_sent_++;
    return dishes_.SendByte(message);
  }

  virtual bool InitializeDryer() {
    return true;
  }

  virtual bool GetMessage(char *message) {
    return dishes_.ReceiveByte(message) && results_.SendByte(0);
  }

 private:
  int queue_limit_,
      num_sent_, num_processed_;
  Pipe dishes_, results_;
};

IpcInterface *IpcInterface::Create(int queue_limit) {
  PipeIpc *ret = new PipeIpc();
  if (ret->Initialize(queue_limit))
    return ret;
  delete ret;
  return NULL;
}
