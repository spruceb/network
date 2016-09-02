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
    int status = receive_request(connection_made, &request);
    if (status < 0) {
      fprintf(stderr, "Request error\n");
      close(connection_made->socket_id);
      free(connection_made);
      break;
    }
    printf("METHOD: %d %s\n", request.method, methodtype_to_string(request.method));
    printf("PATH: %s\n", path_string(request.uri->path));
    printf("VERSION: %d.%d\n", request.version.major, request.version.minor);
    close(connection_made->socket_id);
    free(connection_made);
    break;
  }
  full_close(full_socket);
  return 0;
}
