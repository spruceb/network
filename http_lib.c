#include "http_lib.h"

static const HTTP_BUFFER_SIZE 1024;

DEFINE_VECTOR(HeaderPair, hpair)

FullSocket get_http_socket(const char *address) {
  return get_bindable_socket(address, "http");
}

char* get_line(ConnectionSocket *connection, bytes remaining_data) {
  // sizeof char must be 1
  NetworkBuffer buffer = new_network_buffer(HTTP_BUFFER_SIZE);
  size_t total_remaining = buffer.buffer_length;
  int result;
  char* current_location = buffer.current_buffer;
  char* line_end = NULL;
  while (true) {
    result = connection_receive(connection, current_location,
                                total_remaining);
    if (result < 0) {
      line_end = NULL;
      break;
    }
    total_remaining -= result;
    char last_char = '\0';
    for (char* c = current_location; c < current_location + result; c++) {
      if (*c == '\r' && last_char == '\n') {
        // Reached the end of the line
        line_end = c;
        break;
      }
    }
    if (line_end) {
      size_t amount_used = line_end - current_location;
      memcpy(remaining_data, line_end, result - amount_used);
      next_buffer(&buffer, amount_used);
      break;
    }
    if (total_remaining < 10) {
      current_location = next_buffer(&buffer, buffer.buffer_size - total_remaining);
      total_remaining = buffer.buffer_length;
    } else {
      current_location += result;
    }
  }
  if (line_end) {
    return combine_buffers(&buffer, true);
  } else {
    free_buffer(&buffer);
    return NULL;
  }
}

char* get_method(char* first_line, Request* request) {
  int i;
  char last_char = '\0';
  for (i = 0; i < strlen(first_line); i++) {
    if (last_char == '\r' && first_line[i] == '\n')
      break;
    if (first_line[i] == ' ') {
      if (i == strlen(first_line) - 1) break;
      char* result = malloc(i + 1);
      memcpy(result, first_line, i);
      result[i] = '\0';
      request->method = result;
      return first_line + i + 1;
    }
  }
  return NULL;
}

char* get_uri(char* first_line, Request* request) {
  UIR uri;
}

Request receive_request(ConnectionSocket *connection) {
  Request request;
  void* data_buffer = malloc(HTTP_BUFFER_SIZE);
  int result = connection_receive(connection, data_buffer, HTTP_BUFFER_SIZE);
}
