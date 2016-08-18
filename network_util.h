#ifndef NETWORK_UTIL_H_
#define NETWORK_UTIL_H_

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

struct addrinfo* get_hint();

struct addrinfo* get_address_info(const char *address, const char *service,
                                  const struct addrinfo *hints);

int reuse_socket(int socket_id);

// FullSocket class

typedef struct {
  int socket_id;
  struct addrinfo address;
} FullSocket;

FullSocket get_socket(struct addrinfo *address);

int bind_socket(FullSocket *full_socket);

void full_close(FullSocket *full_socket);

FullSocket get_bindable_socket(const char *address,
                               const char *service);

FullSocket get_http_socket(const char *address);

int full_connect(FullSocket *full_socket);

int full_listen(FullSocket *full_socket, int backlog);

typedef struct {
  int socket_id;
  struct socket_storage * socket_address;
  socklen_t address_size;
  FullSocket *parent;
} ConnectionSocket;

ConnectionSocket full_accept(FullSocket *full_socket);

ConnectionSocket listen_connect(FullSocket *full_socket, int backlog);

int connection_send(ConnectionSocket *connection, const void *message,
                    int message_length);

// End FullSocket definition


#endif
