#include "network_util.h"
#include <signal.h>
#include <sys/wait.h>

#define PORT "3490"
#define BACKLOG 10

void child_handler(int signal) {
  // Wait for child to die so it doesn't become a zombie
  int saved_error = errno;
  while(waitpid(-1, NULL, WNOHANG) > 0);
  errno = saved_error;
}

int main(int argc, char* argv[]) {
  struct sigaction action;
  action.sa_handler = child_handler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = SA_RESTART;
  int change_action_status = sigaction(SIGCHLD, &action, NULL);
  if (change_action_status < 0) {
    perror("Sigaction error");
    return 1;
  }

  char *address = NULL; // localhost
  FullSocket full_socket = get_bindable_socket(address, PORT);
  if (full_listen(&full_socket, BACKLOG) < 0) {
    return 1;
  }
  while (true) {
    ConnectionSocket connection_made = full_accept(&full_socket);
    if (connection_made.socket_id < 0) {
      continue;
    }
    int child_id = fork();
    if (child_id < 0) {
      perror("Fork");
      break;
    } else if (!child_id) {
      full_close(&full_socket);
      char* message = "Test response\n";
      char* received = receive_string(&connection_made);
      if (received != NULL) {
        int received_len = strlen(received);
        printf("RECEIVED\n%s\n", received);
        int bytes_sent = send_string(&connection_made, message);
        if (bytes_sent < -1) exit(1);
      }
      free(received);
      close(connection_made.socket_id);
      exit(0);
    }
    close(connection_made.socket_id);
  }
  full_close(&full_socket);
  return 0;
}
