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

FullSocket* get_socket(struct addrinfo *address) {
  int socket_id = socket(address->ai_family, address->ai_socktype,
                         address->ai_protocol);
  if (socket_id < 0) {
    perror("Socket error");
  }
  FullSocket* socket_result = malloc(sizeof(FullSocket));
  socket_result->socket_id = socket_id;
  socket_result->address = *address;
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
  free(full_socket);
}

FullSocket* get_bindable_socket(const char *address,
                                const char *service) {
  struct addrinfo *hints = get_hint();
  struct addrinfo *result = get_address_info(address, service, hints);
  struct addrinfo *res;
  FullSocket* full_socket;
  for (res = result; res != NULL; res = res->ai_next) {
    full_socket = get_socket(res);
    if (full_socket->socket_id < 0) continue;
    int bind_status = bind_socket(full_socket);
    if (bind_status < 0) {
      full_close(full_socket);
      continue;
    }
    break;
  }
  freeaddrinfo(result);
  free(hints);
  if (res == NULL) {
    fprintf(stderr, "Couldn't bind to: %s", address);
    return NULL;
  }
  return full_socket;
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

ConnectionSocket* full_accept(FullSocket *full_socket) {
  ConnectionSocket* connection = malloc(sizeof(ConnectionSocket));
  int connection_id = accept(full_socket->socket_id,
                             (struct sockaddr *) &connection->socket_address,
                             &connection->address_size);
  if (connection_id < 0) {
    perror("Accept error");
  }
  connection->socket_id = connection_id;
  connection->parent = full_socket;
  return connection;
}

ConnectionSocket* listen_connect(FullSocket *full_socket, int backlog) {
  int status = full_listen(full_socket, backlog);
  if (status < 0) {
    return NULL;
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

int send_all(ConnectionSocket *connection, void *message,
             int message_length) {
  int total_sent = 0;

  while (total_sent < message_length) {
    int sent = connection_send(connection, message, message_length);
    if (sent < 0){
      total_sent = -1;
      break;
    }
    total_sent += sent;
    message = (void *) ((char*) message + sent);
    message_length -= sent;
  }
  if (total_sent < message_length) {
    fprintf(stderr, "Sendall error: whole message not sent\n");
    return -1;
  }
  return total_sent;
}

int connection_receive(ConnectionSocket *connection, void *memory, size_t length) {
  int total_received = recv(connection->socket_id, memory, length, 0);
  if (total_received < 0) {
    perror("Receive error");
  }
  return total_received;
}

int _receive_all(ConnectionSocket *connection, void **result, bool null_terminate) {
  NetworkBuffer* buffer = new_network_buffer(1024);

  while (true) {
    int received = connection_receive(connection, buffer->current_buffer,
                                      buffer->buffer_length);
    if (received < 1) {
      // Closed or errored
      break;
    }
    next_buffer(buffer, received);
  }
  *result = combine_buffers(buffer, null_terminate);
  return buffer->buffer_length;
}

int receive_all(ConnectionSocket *connection, void **result) {
  return _receive_all(connection, result, false);
}

int send_string(ConnectionSocket *connection, char* string) {
  int length = strlen(string) + 1; // 1 for null terminator
  return send_all(connection, string, length);
}

char* receive_string(ConnectionSocket *connection) {
  char *result;
  int status = _receive_all(connection, (void **) &result, true);
  // string must be null terminated
  if (status < 0 || result[status] != '\0') {
    fprintf(stderr, "Receive string error! status: %d\n", status);
    return NULL;
  }
  return result;
}

// Network Buffer

NetworkBuffer * new_network_buffer(size_t buffer_length) {
  NetworkBuffer* buffer = malloc(sizeof(NetworkBuffer));
  buffer->data = new_byte_vector(0);
  buffer->lengths = new_size_t_vector(0);
  buffer->buffer_length = buffer_length;
  buffer->current_buffer = malloc(buffer_length);
  return buffer;
}

bytes next_buffer(NetworkBuffer *buffer, size_t amount_used) {
  append(&buffer->data, buffer->current_buffer);
  append(&buffer->lengths, amount_used);
  bytes current_buffer = malloc(buffer->buffer_length);
  buffer->current_buffer = current_buffer;
  return current_buffer;
}

bytes combine_buffers(NetworkBuffer* buffer, bool null_terminate) {
  free(buffer->current_buffer);
  size_t total_size = null_terminate;
  for (int i = 0; i < buffer->lengths.length; i++) {
    total_size += get(&buffer->lengths, i);
  }
  bytes combined_buffer = malloc(total_size + null_terminate);
  bytes intermediate_result = combined_buffer;
  for (int i = 0; i < buffer->data.length; i++) {
    bytes data = get(&buffer->data, i);
    size_t length = get(&buffer->lengths, i);
    memcpy(intermediate_result, data, length);
    intermediate_result += length;
    free(data);
  }
  if (null_terminate)
    combined_buffer[total_size - 1] = '\0';
  free(buffer->data.data);
  free(buffer->lengths.data);
  buffer->current_buffer = combined_buffer;
  buffer->buffer_length = total_size;
  return combined_buffer;
}

void free_buffer(NetworkBuffer* buffer) {
  for (int i = 0; i < buffer->data.length; i++) {
    free(get(&buffer->data, i));
  }
  free(buffer->lengths.data);
  free(buffer->current_buffer);
}
