#include <assert.h>
#include <arpa/inet.h>
#include <errno.h>
#include <malloc.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <string>
#include <vector>

std::string AddrToString(const struct sockaddr_in *addr) {
  static char buffer[100];
  snprintf(buffer, sizeof(buffer), "%s:%i",
           inet_ntoa(addr->sin_addr), (int)ntohs(addr->sin_port));
  return std::string(buffer);
}

std::vector<int> merge(const std::vector<int> &a, const std::vector<int> &b) {
  std::vector<int> ret;
  ret.reserve(a.size() + b.size());

  std::vector<int>::const_iterator it_a = a.begin(), it_b = b.begin();
  while (it_a != a.end() && it_b != b.end()) {
    if (*it_a < *it_b)
      ret.push_back(*(it_a++));
    else
      ret.push_back(*(it_b++));
  }
  if (it_a != a.end()) {
    assert(it_b == b.end());
    ret.insert(ret.end(), it_a, a.end());
  } else if (it_b != b.end()) {
    ret.insert(ret.end(), it_b, b.end());
  }

  return ret;
}

std::vector<int> input;

const int INVALID_SOCKET = -1;
const int SERVER_PORT = 7777;
const int NUM_WORKERS = 2;

bool SendInt(int socket, int value) {
  return (sizeof(value) == send(socket, &value, sizeof(value), 0));
}

bool RecvInt(int socket, int *value) {
  return (sizeof(value) == recv(socket, value, sizeof(*value), 0));
}

bool RunServer() {
  int listen_socket = socket(AF_INET, SOCK_STREAM, 0);
  assert(listen_socket != INVALID_SOCKET);

  // Set up server port
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(SERVER_PORT);
  addr.sin_addr.s_addr = inet_addr("0.0.0.0");
  // Bind to local address and port PORT.
  if (bind(listen_socket, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    close(listen_socket);
    return false;
  }

  // Wait for an incoming connection
  if (listen(listen_socket, NUM_WORKERS) == -1) {
    close(listen_socket);
    return false;
  }

  std::vector<int> clients;
  for (int i = 0; i < NUM_WORKERS; i++) {
    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    int client_socket = accept(listen_socket,
                               (struct sockaddr *)&client_addr,
                               &client_addr_size);
    close(listen_socket);
    if (client_socket == INVALID_SOCKET)
      return false;

    clients.push_back(client_socket);

    printf("Incoming client from %s!\nWaiting for the message...\n",
           AddrToString(&client_addr).c_str());

  }

  // Format:
  // <N> N more numbers
  // <0> -> sort

  const size_t SEND_CHUNK = 2;
  size_t send_idx = 0, cur_send_client = 0;
  while (send_idx < input.size()) {
    size_t to_send = input.size() - send_idx;
    if (to_send > SEND_CHUNK)
      to_send = SEND_CHUNK;
    SendInt(clients[cur_send_client], to_send);
    for (size_t i = 0; i < to_send; i++, send_idx++)
      // TODO: check return code
      SendInt(clients[cur_send_client], input[to_send]);
    cur_send_client = (cur_send_client + 1) % clients.size();
  }

  for (size_t i = 0; i < clients.size(); i++)
    SendInt(clients[cur_send_client], 0);

  // Read sorted sub-arrays.

  const int BUFFER_SIZE = 10;
  char buffer[BUFFER_SIZE + 1];
  std::string message;

#if 0
  // Receive a text message from the client
  int ret;
  while ((ret = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
    buffer[ret] = '\0';
    message += buffer;
  }
  if (ret == -1) {
    // Something gone wrong while receiving the message...
    return false;
  }

  printf("Received a message!\n%s", message.c_str());

  close(client_socket);
#endif
  return true;
}

bool RunClient(const char *server_ip) {
  int s = socket(AF_INET, SOCK_STREAM, 0);
  assert(s != INVALID_SOCKET);

  struct sockaddr_in addr;
  addr.sin_family    = AF_INET;
  addr.sin_port      = htons(SERVER_PORT);
  addr.sin_addr.s_addr = inet_addr(server_ip);
  if (addr.sin_addr.s_addr == -1)
    return false;

  const int NUM_ATTEMPTS = 10;
  int i;
  for (i = 0; i < NUM_ATTEMPTS; i++) {
    if (connect(s, (sockaddr*)&addr, sizeof(addr)) != -1)
      break;
    sleep(1);  // Sleep 1 second between attempts
  }
  if (i == NUM_ATTEMPTS)
    return false;

  printf("Connected to server at %s:%d!\n", server_ip, SERVER_PORT);

  // TODO
  close(s);
  if (ret == -1)
    return false;
  return true;
}

int main(int argc, char **argv) {
  for (int i = 0; i < 16; i++)
    input.push_back(i);
  std::random_shuffle(input.begin(), input.end());

  printf("Input:\n");
  for (int i = 0; i < input.size(); i++)
    printf("%s%d", (i == 0) ? "" : ", ", input[i]);
  printf("\n\n");

  if (argc == 2 && !strcmp(argv[1], "server")) {
    if (!RunServer()) {
      fprintf(stderr, "Error! %s\n", strerror(errno));
      return 1;
    }
  } else if (argc == 3 && !strcmp(argv[1], "client")) {
    if (!RunClient(argv[2])) {
      fprintf(stderr, "Error! %s\n", strerror(errno));
      return 1;
    }
  } else {
    fprintf(stderr, "Usage: sockets client <ip>|server\n");
    return 1;
  }
  return 0;
}
