#include "http_lib.h"

static const HTTP_BUFFER_SIZE 1024;

DEFINE_VECTOR(HeaderPair, hpair)

FullSocket get_http_socket(const char *address) {
  return get_bindable_socket(address, "http");
}

char* get_method(ConnectionSocket *connection, void *remaining_data) {
  // sizeof char must be 1
  void* buffer = malloc(HTTP_BUFFER_SIZE);
  size_t total_remaining = HTTP_BUFFER_SIZE;
  int result;
  char* current_location = buffer;
  char* line_end = NULL;
  while (total_remaining) {
    result = connection_receive(connection, buffer, total_remaining);
    if (result < 0) {
      return NULL;
    }
    char last_char = '\0';
    for (char* c = current_location; c < (current_location + result); c++) {
      if (*c == '\r' && last_char == '\n') {
        // Reached the end of the line
        line_end = c;
        break;
      }
    }
    if (line_end) {
      
    }
  }
  
}

Request receive_request(ConnectionSocket *connection) {
  Request request;
  void* data_buffer = malloc(HTTP_BUFFER_SIZE);
  int result = connection_receive(connection, data_buffer, HTTP_BUFFER_SIZE);
  if ()
    }
