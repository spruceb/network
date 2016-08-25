#ifndef _NETWORK_UTIL_H_
#define _NETWORK_UTIL_H_

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include "vector.h"


struct addrinfo* get_hint();

struct addrinfo* get_address_info(const char *address, const char *service,
                                  const struct addrinfo *hints);

int reuse_socket(int socket_id);

// FullSocket class

typedef struct {
  int socket_id;
  struct addrinfo address;
} FullSocket;

FullSocket* get_socket(struct addrinfo *address);

int bind_socket(FullSocket *full_socket);

void full_close(FullSocket *full_socket);

FullSocket* get_bindable_socket(const char *address,
                                const char *service);

int full_connect(FullSocket *full_socket);

int full_listen(FullSocket *full_socket, int backlog);

typedef struct {
  int socket_id;
  struct socket_storage * socket_address;
  socklen_t address_size;
  FullSocket *parent;
} ConnectionSocket;

ConnectionSocket* full_accept(FullSocket *full_socket);

ConnectionSocket* listen_connect(FullSocket *full_socket, int backlog);

int connection_send(ConnectionSocket *connection, const void *message,
                    int message_length);

int send_all(ConnectionSocket *connection, void *message,
             int message_length);

int connection_receive(ConnectionSocket *connection, void *memory, size_t length);

int receive_all(ConnectionSocket *connection, void **result);

int send_string(ConnectionSocket *connection, char* string);

char* receive_string(ConnectionSocket *connection);

// End FullSocket definition

// NetworkBuffer class definition
typedef struct {
  byte_vector data;
  size_t_vector lengths;
  bytes current_buffer;
  size_t buffer_length;
} NetworkBuffer;

NetworkBuffer * new_network_buffer(size_t buffer_length);

bytes next_buffer(NetworkBuffer *buffer, size_t amount_used);

bytes combine_buffers(NetworkBuffer* buffer, bool null_terminate);

void free_buffer(NetworkBuffer* buffer);

#endif
