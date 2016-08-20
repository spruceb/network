#include "network_util.h"
#include <signal.h>

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
    char* received = receive_string(&connection_made);
    if (received != NULL) {
      int received_len = strlen(received);
      printf("RECEIVED\n%s\n", received);
      if (strcmp(received, "quit\n") == 0) {
        send_string(&connection_made, "Quitting!\n");
        break;
      }
      int bytes_sent = send_string(&connection_made, message);
      if (bytes_sent < -1) break;
    }
    free(received);
    close(connection_made.socket_id);
  }
  full_close(&full_socket);
}
