#include "http_lib.h"

static const int HTTP_BUFFER_SIZE = 1024;

DEFINE_VECTOR(HeaderPair, hpair)

FullSocket get_http_socket(const char *address) {
  return get_bindable_socket(address, "http");
}

char* get_line(ConnectionSocket *connection, bytes remaining_data) {
  // sizeof char must be 1
  NetworkBuffer buffer = new_network_buffer(HTTP_BUFFER_SIZE);
  size_t total_remaining = buffer.buffer_length;
  int result;
  char* current_location = (char*) buffer.current_buffer;
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
      current_location = (char*) next_buffer(&buffer, buffer.buffer_length - total_remaining);
      total_remaining = buffer.buffer_length;
    } else {
      current_location += result;
    }
  }
  if (line_end) {
    return (char*) combine_buffers(&buffer, true);
  } else {
    free_buffer(&buffer);
    return NULL;
  }
}

bool char_in_string(char c, char* string) {
  for (char* p = string; *p != '\0'; p++) {
    if (*p == c)
      return true;
  }
  return false;
}

string_vector split_on(const char* to_split, const char* split) {
  string_vector vec = new_string_vector(1);
  size_t length = strlen(to_split);
  int last_index = 0;
  for (int i = 0; i < length; i++) {
    if (char_in_string(to_split[i], split)) {
      char* section = malloc(sizeof(char) * (i - last_index + 1));
      section[i - last_index] = '\0';
      strncpy(to_split + last_index, section, i - last_index);
      append(&vec, section);
      i++;
    }
  }
  return vec;
}

MethodType string_to_methodtype(char* method_name) {
  for (int i = 0; i < num_method_types; i++) {
    if (strcmp(method_name, MethodTypeStrings[i]) == 0) {
      return MethodTypeStrings[i];
    }
  }
  return INVALID;
}

char* methodtype_to_string(MethodType method) {
  return MethodTypeStrings[method];
}

char* get_method(char* first_line, Request* request) {
  char last_char = '\0';
  for (int i = 0; i < strlen(first_line); i++) {
    if (last_char == '\r' && first_line[i] == '\n')
      break;
    if (first_line[i] == ' ') {
      if (i == strlen(first_line) - 1) break;
      char* result = malloc(i + 1);
      memcpy(result, first_line, i);
      result[i] = '\0';
      request->method = string_to_methodtype(result);
      if (request->method == INVALID) {
        return NULL;
      }
      return first_line + i + 1;
    }
  }
  return NULL;
}

char* get_scheme(char* uri_string, URI* uri) {
  size_t string_length = strlen(uri_string);
  if (string_length < 4) {
    return NULL;
  }
  for (char* p = uri_string + 3; p < uri_string + string_length; p++) {
    if (*(p - 2) == ':' && *(p - 1) == '/' && *p == '/') {
      size_t scheme_length = (p - 3) - uri_string;
      char* scheme = malloc(sizeof(char) * (scheme_length + 1));
      strncpy(uri_string, scheme, scheme_length);
      scheme[scheme_length] = '\0';
      uri->scheme = scheme;
      return p + 1;
    }
  }
  return NULL;
}

char* get_host(char* host_string, URI* uri) {
  size_t string_length = strlen(host_string);
  int first_slash = strcspn(host_string, "/");
  if (first_slash == string_length) {

  }
}

char* get_uri(char* input_string, Request* request,
              char* method, URI* uri) {
  char* next_string = get_scheme(input_string, uri);
  if (next_string)
    input_string = next_string;

}

Request receive_request(ConnectionSocket *connection) {
  Request request;
  void* data_buffer = malloc(HTTP_BUFFER_SIZE);
  int result = connection_receive(connection, data_buffer, HTTP_BUFFER_SIZE);
}
