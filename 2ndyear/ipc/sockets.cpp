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

#include <string>

std::string AddrToString(const struct sockaddr_in *addr) {
  static char buffer[100];
  snprintf(buffer, sizeof(buffer), "%s:%i",
           inet_ntoa(addr->sin_addr), (int)ntohs(addr->sin_port));
  return std::string(buffer);
}

const int INVALID_SOCKET = -1;
const int SERVER_PORT = 7777;

bool RunServer() {
  int listen_socket = socket(AF_INET, SOCK_STREAM, 0);
  assert(listen_socket != INVALID_SOCKET);

  // Set up server address
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family      = AF_INET;
  addr.sin_port        = htons(SERVER_PORT);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  // Bind to ANY local address and port SERVER_PORT.
  if (bind(listen_socket, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    close(listen_socket);
    return false;
  }

  // Wait for incoming connections (just one actually).
  if (listen(listen_socket, 1) == -1) {
    close(listen_socket);
    return false;
  }

  struct sockaddr_in client_addr;
  memset(&client_addr, 0, sizeof(client_addr));
  socklen_t client_addr_size = sizeof(client_addr);
  // Try to establish one connection.
  int client_socket = accept(listen_socket,
                             (struct sockaddr*)&client_addr,
                             &client_addr_size);
  close(listen_socket);
  if (client_socket == INVALID_SOCKET)
    return false;

  printf("Incoming client from %s!\nWaiting for the message...\n",
         AddrToString(&client_addr).c_str());

  const int BUFFER_SIZE = 10;
  char buffer[BUFFER_SIZE + 1];
  std::string message;

  // Receive a text message from the client.
  // Very important: ret may be < BUFFER_SIZE
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
  return true;
}

bool RunClient(const char *server_ip) {
  int s = socket(AF_INET, SOCK_STREAM, 0);
  assert(s != INVALID_SOCKET);

  struct sockaddr_in addr;
  addr.sin_family      = AF_INET;
  addr.sin_port        = htons(SERVER_PORT);
  addr.sin_addr.s_addr = inet_addr(server_ip);  // inet_addr doesn't need htonl
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

  printf("Please type a message to send to the server:\n");
  char message[100];
  if (fgets(message, sizeof(message), stdin) == NULL)
    return false;

  printf("Okay, sending it to the server...\n");

  char *to_send = message;
  int remaining_bytes = strlen(message);  // No need to +1, see server code.
  int ret;
  // VERY important: ret can be < remaining_bytes.
  while (remaining_bytes > 0 &&
         (ret = send(s, to_send, remaining_bytes, 0)) > 0) {
    assert(remaining_bytes >= ret);
    remaining_bytes -= ret;
    to_send += ret;
  }
  close(s);
  if (ret == -1)
    return false;
  return true;
}

int main(int argc, char **argv) {
  // TODO: run both, using threads or fork.

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
