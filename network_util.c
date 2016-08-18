#include "network_util.h"

struct addrinfo* get_hint() {
  struct addrinfo *hints = malloc(sizeof(struct addrinfo));
  memset(hints, 0, sizeof(struct addrinfo));
  hints->ai_family = AF_UNSPEC;
  hints->ai_socktype = SOCK_STREAM;
  hints->ai_flags = AI_PASSIVE;
  return hints;
}

struct addrinfo* get_address_info(const char *address, const char *service,
                                  const struct addrinfo *hints) {
  struct addrinfo *result;
  int status = getaddrinfo(address, service, hints, &result);
  if (status != 0) {
    fprintf(stderr, "get_address error: %s", gai_strerror(status));
    exit(EXIT_FAILURE);
  }
  return result;
}

int reuse_socket(int socket_id) {
  int yes_option = 1;
  int status = setsockopt(socket_id, SOL_SOCKET, SO_REUSEADDR,
                          &yes_option, sizeof(yes_option));
  if (status < 0) {
    perror("Reuse error");
    exit(EXIT_FAILURE);
  }
  return status;
}

FullSocket get_socket(struct addrinfo *address) {
  int socket_id = socket(address->ai_family, address->ai_socktype,
                         address->ai_protocol);
  if (socket_id < 0) {
    perror("Socket error");
  }
  FullSocket socket_result;
  socket_result.socket_id = socket_id;
  socket_result.address = *address; 
  return socket_result;
}



int bind_socket(FullSocket *full_socket) {
  int status = reuse_socket(full_socket->socket_id);
  if (status < 0) {
    return status;
  }
  int bind_status = bind(full_socket->socket_id,
                         full_socket->address.ai_addr,
                         full_socket->address.ai_addrlen);
  if (bind_status < 0) {
    perror("Bind error");
  }
  return bind_status;
}

void full_close(FullSocket *full_socket) {
  close(full_socket->socket_id);
}

FullSocket get_bindable_socket(const char *address,
                               const char *service) {
  struct addrinfo *hints = get_hint();
  struct addrinfo *result = get_address_info(address, service, hints);
  struct addrinfo *res;
  FullSocket full_socket;
  for (res = result; res != NULL; res = res->ai_next) {
    full_socket = get_socket(res);
    if (full_socket.socket_id < 0) continue;
    int bind_status = bind_socket(&full_socket);
    if (bind_status < 0) {
      full_close(&full_socket);
      continue;
    }
    break;
  }
  freeaddrinfo(result);
  free(hints);
  if (res == NULL) {
    fprintf(stderr, "Couldn't bind to: %s", address);
    full_socket.socket_id = -1;
  }
  return full_socket;
}

FullSocket get_http_socket(const char *address) {
  return get_bindable_socket(address, "http");
}

int full_connect(FullSocket *full_socket) {
  int status = connect(full_socket->socket_id, full_socket->address.ai_addr,
                       full_socket->address.ai_addrlen);
  if (status < 0) {
    perror("Connect error");
  }
  return status;
}

int full_listen(FullSocket *full_socket, int backlog) {
  int status = listen(full_socket->socket_id, backlog);
  if (status < 0) {
    perror("Listen error");
  }
  return status;
}

ConnectionSocket full_accept(FullSocket *full_socket) {
  ConnectionSocket connection;
  int connection_id = accept(full_socket->socket_id,
                             (struct sockaddr *) &connection.socket_address,
                             &connection.address_size);
  if (connection_id < 0) {
    perror("Accept error");    
  }
  connection.socket_id = connection_id;
  connection.parent = full_socket;
  return connection;
}

ConnectionSocket listen_connect(FullSocket *full_socket, int backlog) {
  int status = full_listen(full_socket, backlog);
  if (status < 0) {
    ConnectionSocket fake;
    fake.socket_id = -1;
    return fake;
  } 
  return full_accept(full_socket);
}

int connection_send(ConnectionSocket *connection, const void *message,
                    int message_length) {
  int sent = send(connection->socket_id, message, message_length, 0);
  if (sent < 0) {
    perror("Send error");
  }
  return sent;
}