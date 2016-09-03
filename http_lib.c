#include "http_lib.h"

DEFINE_VECTOR(HeaderPair, hpair);

static const int HTTP_BUFFER_SIZE = 1024;

FullSocket* get_http_socket(const char *address) {
  return get_bindable_socket(address, "3333");
}

char* get_line(ConnectionSocket *connection, bytes previous_data,
               bytes remaining_data) {
  // sizeof char must be 1
  NetworkBuffer* buffer = new_network_buffer(HTTP_BUFFER_SIZE);
  if (previous_data != NULL) {
    size_t previous_size = strlen((char*) previous_data) + 1;
    memcpy(buffer->current_buffer, previous_data, previous_size);
    next_buffer(buffer, previous_size);
  }
  size_t total_remaining = buffer->buffer_length;
  int result;
  char* current_location = (char*) buffer->current_buffer;
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
      if (*c == '\n' && last_char == '\r') {
        // Reached the end of the line
        line_end = c;
        break;
      }
      last_char = *c;
    }
    if (line_end) {
      size_t amount_used = line_end - current_location;
      *(line_end - 1) = '\0';
      memcpy(remaining_data, line_end, result - amount_used);
      next_buffer(buffer, amount_used);
      break;
    }
    if (total_remaining < 10) {
      current_location = (char*) next_buffer(buffer, buffer->buffer_length - total_remaining);
      total_remaining = buffer->buffer_length;
    } else {
      current_location += result;
    }
  }
  if (line_end) {
    return (char*) combine_buffers(buffer, true);
  } else {
    free_buffer(buffer);
    return NULL;
  }
}

MethodType string_to_methodtype(const char* method_name) {
  for (MethodType i = GET; i < INVALID; i++) {
    if (strcmp(method_name, MethodTypeStrings[i]) == 0) {
      return i;
    }
  }
  return INVALID;
}

const char* methodtype_to_string(MethodType method) {
  return MethodTypeStrings[method];
}

int get_method(const char* method_string, Request* request) {
  MethodType method = string_to_methodtype(method_string);
  if (method == INVALID) {
    fprintf(stderr, "Error: malformed method '%s'\n", method_string);
    return -1;
  }
  request->method = method;
  return 0;
}

bool has_scheme(const char* uri_string) {
  return strstr(uri_string, "://");
}

const char* get_scheme(const char* uri_string, URI* uri) {
  char* scheme_end = strstr(uri_string, "://");
  if (scheme_end)
    return string_slice(uri_string, NULL, scheme_end);
  return NULL;
}

int get_host(const char* host_string, Authority* authority) {
  free(authority->host->components.data);
  authority->host->components = split_on(host_string, ".", -1);
  return 0;
}

int get_port(const char* port_string, Authority* authority) {
  authority->port = atoi(port_string);
  return 0;
}

Host* new_host() {
  Host* host = malloc(sizeof(*host));
  host->components = new_string_vector(0);
  return host;
}

Authority* new_authority() {
  Authority* authority = malloc(sizeof(*authority));
  authority->host = new_host();
  authority->port = 88;
  return authority;
}

const char* get_authority(const char* uri_string, URI* uri) {
  Authority* authority = new_authority();
  size_t authority_length = strcspn(uri_string, "/");
  const char* rest;
  if (authority_length < strlen(uri_string)) {
    rest = uri_string + authority_length;
  } else {
    rest = NULL;
  }
  string_vector split = split_on(uri_string, ":", authority_length);
  get_host(get(&split, 0), authority);
  get_port(get(&split, 1), authority);
  uri->authority = authority;
  return rest;
}

Path* new_path() {
  Path* path = malloc(sizeof(*path));
  path->components = new_string_vector(0);
  path->root = false;
  return path;
}

int get_path(const char* path_string, Path* path) {
  path->components = split_on(path_string, "/", -1);
  return 0;
}

char* get_relative(const char* uri_string, URI* uri) {
  size_t query_start = strcspn(uri_string, "?");
  size_t fragment_start = strcspn(uri_string, "#");
  size_t path_length;
  if (query_start < fragment_start){
    path_length = query_start;
  } else {
    path_length = fragment_start;
  }
  char* path_string = malloc(sizeof(*path_string) * (path_length + 1));
  path_string[path_length] = '\0';
  memcpy(path_string, uri_string, path_length);
  uri->path = new_path();
  get_path(path_string, uri->path);
  return 1;
}

int get_uri(const char* uri_string, URI* uri, MethodType method) {
  if (strcmp(uri_string, "*") == 0) {
    if (method == OPTIONS) {
      uri->type = ASTERISK;
      return 0;
    } else {
      return -1;
    }
  } else if (method == CONNECT) {
    if (get_authority(uri_string, uri)) {
      uri->type = AUTHORITY;
      return 0;
    } else return -1;
  } else {
    if (has_scheme(uri_string)) {
      const char* rest = get_scheme(uri_string, uri);
      if (rest == NULL) return -1;
      rest = get_authority(rest, uri);
      if (rest == NULL) return -1;
      rest = get_relative(rest, uri);
      if (rest == NULL) return -1;
      uri->type = ABSOLUTE;
      return 0;
    } else {
      if (get_relative(uri_string, uri)) {
        uri->type = RELATIVE;
        return 0;
      } else return -1;
    }
  }
}

char* path_string(Path* path) {
  char* result = join_on(&path->components, "/");
  if (strlen(result) == 0) {
    result = "/";
  }
  return result;
}

URI* new_uri() {
  URI* uri = malloc(sizeof(*uri));
  return uri;
}

int get_version(const char* version_string, Request* request) {
  if (strncmp(version_string, "HTTP", 4) != 0) {
    fprintf(stderr, "Error: unknown protocol with version string %.4s",
            version_string);
    return -1;
  }
  request->version.major = *(version_string + 5) - '0';
  request->version.minor = *(version_string + 7) - '0';
  return 0;
}

int receive_request(ConnectionSocket *connection, Request* request) {
  char* excess_data = malloc(HTTP_BUFFER_SIZE * sizeof(*excess_data));
  char* line = get_line(connection, NULL, (bytes) excess_data);
  if (line == NULL) {
    fprintf(stderr, "Error: invalid line\n");
    return -1;
  }
  string_vector line_components = split_on(line, " ", -1);
  if (line_components.length != 3) {
    fprintf(stderr, "Error: malformed first line |%s|\n", line);
    return -1;
  }
  char* method_string = get(&line_components, 0);
  char* uri_string = get(&line_components, 1);

  char* version_string = get(&line_components, 2);
  MethodType method = string_to_methodtype(method_string);
  if (method == INVALID) {
    fprintf(stderr, "INVALID METHOD: %d\n", method);
    return -1;
  }
  request->method = method;
  request->uri = new_uri();
  if (get_uri(uri_string, request->uri, method) < 0) {
    fprintf(stderr, "Error: problem with URI '%s'\n", uri_string);
    return -1;
  }
  if (get_version(version_string, request) < 0) {
    fprintf(stderr, "Error: problem with version: '%s'", version_string);
    return -1;
  }
  return 0;
}
