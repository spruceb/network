#include "network_util.h"

#define PORT "3490"
#define BACKLOG 10

int main(int argc, char* argv[]) {
  char *address = NULL; // localhost
  FullSocket full_socket = get_bindable_socket(address, PORT);
  while (true) {
    ConnectionSocket connection_made = listen_connect(&full_socket, BACKLOG);
    if (connection_made.socket_id < 0) {
      return 1;
    }
    char* message = "Test response\n";
    int message_len = strlen(message);
    void *received;
    int received_len = receive_all(&connection_made, &received);
    if (received_len > 0) {
      printf("RECEIVED\n\n%s", received);
      free(received);
      int bytes_sent = send_all(&connection_made, message, message_len);
      if (bytes_sent < -1) break;
    }
    close(connection_made.socket_id);
  }
  full_close(&full_socket);
}
