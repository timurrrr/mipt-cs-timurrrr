#include "ipc_interface.h"

#include <arpa/inet.h>
#include <assert.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

class SocketIpc: public IpcInterface {
 public:
  SocketIpc() : socket_(INVALID_SOCKET),
                queue_limit_(-1),
                num_processed_(0), num_sent_(0) {}

  bool Initialize(int queue_limit) {
    if (queue_limit <= 0)
      return false;
    queue_limit_ = queue_limit;
    return true;
  }

  virtual void Close() {
    if (socket_ != INVALID_SOCKET)
      close(socket_);
    socket_ = INVALID_SOCKET;
  }

  virtual bool InitializeWasher() {
    assert(socket_ == INVALID_SOCKET);

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    socket_ = socket(AF_INET, SOCK_STREAM, 0);
    assert(socket_ != INVALID_SOCKET);

    if (connect(socket_, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
      Close();
      return false;
    }
    return true;
  }

  // Should blocks if QUEUE_LIMIT is reached
  virtual bool SendMessage(char message) {
    while (num_sent_ >= num_processed_ + queue_limit_) {
      // Wait for server to process one message
      char tmp;
      if (!ReceiveByte(&tmp)) {
        printf("ASDASD\n");
        return false;
      }
      num_processed_ ++;
    }

    num_sent_++;
    return SendByte(message);
  }

  virtual bool InitializeDryer() {
    assert(socket_ == INVALID_SOCKET);
    int listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    assert(listen_socket != INVALID_SOCKET);

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (bind(listen_socket, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
      close(listen_socket);
      return false;
    }

    if (listen(listen_socket, 1) == -1) {
      close(listen_socket);
      return false;
    }

    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    socket_ = accept(listen_socket, (struct sockaddr *)&client_addr,
                     &client_addr_size);
    close(listen_socket);

    return (socket_ != INVALID_SOCKET);
  }

  virtual bool GetMessage(char *message) {
    return ReceiveByte(message) && SendByte(0);
  }

 private:
  bool SendByte(char b) {
    assert(socket_ != INVALID_SOCKET);

    return (send(socket_, &b, 1, 0) == 1);
  }

  bool ReceiveByte(char *b) {
    return (recv(socket_, b, 1, 0) == 1);
  }

  enum { PORT = 7777, INVALID_SOCKET = -1 };
  int queue_limit_,
      num_sent_, num_processed_;
  int socket_;
};

IpcInterface *IpcInterface::Create(int queue_limit) {
  SocketIpc *ret = new SocketIpc();
  if (ret->Initialize(queue_limit))
    return ret;
  delete ret;
  return NULL;
}
