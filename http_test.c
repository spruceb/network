#include "http_lib.h"

#define BACKLOG 10

int main() {
  char *address = NULL;
  FullSocket* full_socket = get_http_socket(address);
  if (!full_socket) {
    fprintf(stderr, "Socket problem");
    return 1;
  }
  if (full_listen(full_socket, BACKLOG) < 0) {
    return 1;
  }
  while (true) {
    ConnectionSocket* connection_made = full_accept(full_socket);
    if (connection_made->socket_id < 0) {
      continue;
    }
    Request request;
    receive_request(connection_made, &request);
    printf("%d", (int) request.method);
  }
  full_close(full_socket);
  return 0;
}
