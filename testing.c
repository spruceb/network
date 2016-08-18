#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>


#define PORT "3490"
#define BACKLOG 10

int main(int argc, char *argv[]) {
  char usage[100];
  if (argc != 2) {
    fprintf(stderr, "Invalid usage\n");
    return 1;
  } else {
    strcpy(usage, argv[1]);
  }
  
  int sockfd;
  int status;
  struct addrinfo hints;
  struct addrinfo *res;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  char *addr = NULL;
  if ((status = getaddrinfo(addr, PORT, &hints, &res)) != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    return 1;
  }
  if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
    fprintf(stderr, "Socket error: %s\n", strerror(errno));
    return 1;
  }
  if (addr == NULL) {
    int bound = 1;
    // lose the pesky "Address already in use" error message
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &bound, sizeof bound) == -1) {
      perror("setsockopt");
      exit(1);
    }
    if ((status = bind(sockfd, res->ai_addr, res->ai_addrlen) == -1)) {
      fprintf(stderr, "Bind error: %s\n", strerror(errno));
      return 1;
    }
  }
  
  if (strcmp(usage, "connect") == 0) {
    if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
      fprintf(stderr, "Connection error: %s\n", strerror(errno));
      return 1;
    }
    printf("Connected!");
  } else if (strcmp(usage, "listen") == 0) {
    struct sockaddr_storage connector_addr;
    socklen_t addr_size = sizeof connector_addr;
    if (listen(sockfd, BACKLOG) == -1) {
      fprintf(stderr, "Listen error: %s\n", strerror(errno));
      return 1;
    }
    int connect_fd = accept(sockfd, (struct sockaddr *) &connector_addr, &addr_size);
    if (connect_fd == -1) {
      fprintf(stderr, "Accept error: %s\n", strerror(errno));
      return 1;
    }
    struct sockaddr them;
    socklen_t addrlen = sizeof them;
    getpeername(connect_fd, &them, &addrlen);
    char result[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, them.sa_data, result, INET_ADDRSTRLEN);
    printf("Them: %s\n", result);
    
    char* message = "Testing!\n";
    int message_len = strlen(message);
    int bytes_sent;
    if ((bytes_sent = send(connect_fd, message, message_len, 0)) == -1) {
      fprintf(stderr, "Send error: %s\n", strerror(errno));
      return 1;
    }
    if (bytes_sent != message_len) {
      fprintf(stderr, "Entire message not sent\n");
    }
    close(connect_fd);
  }
  close(sockfd);
  freeaddrinfo(res);
  return 0;
}
