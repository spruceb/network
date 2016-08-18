#include "network_util.h"

#define PORT "3490"
#define BACKLOG 10

int main(int argc, char* argv[]) {
  char *address = NULL; // localhost
  FullSocket full_socket = get_bindable_socket(address, PORT);
  ConnectionSocket connection_made = listen_connect(&full_socket, BACKLOG);
  if (connection_made.socket_id < 0) {
    return 1;
  }
  char* message = "This is a test message\n";
  int message_len = strlen(message);
  int bytes_sent = connection_send(&connection_made, message, message_len);
  if (bytes_sent < -1) return 1;
  close(connection_made.socket_id);
  full_close(&full_socket);
}
