#ifndef HTTP_LIB_H_
#define HTTP_LIB_H_

#include "network_util.h"
#include "strings.h"

FullSocket* get_http_socket(const char *address);

typedef enum {RELATIVE, ABSOLUTE, AUTHORITY, ASTERISK} URIType;

typedef enum {GET, HEAD, POST, PUT, DELETE, CONNECT, OPTIONS, TRACE, INVALID} MethodType;

static const char* MethodTypeStrings[] = {
  [GET] = "GET",
  [HEAD] = "HEAD",
  [POST] = "POST",
  [PUT] = "PUT",
  [DELETE] = "DELETE",
  [CONNECT] = "CONNECT",
  [OPTIONS] = "OPTIONS",
  [TRACE] = "TRACE"
};
const size_t num_method_types;

typedef struct {
  string_vector components;
  bool root;
} Path;

typedef struct {
  string_vector names;
  string_vector values;
} Query;

typedef struct {
  string_vector components;
} Host;

typedef struct {
  Host* host;
  unsigned port;
} Authority;

typedef struct {
  char* scheme;
  Authority* authority;
  Path* path;
  Query* options;
  char* fragment;
  URIType type;
} URI;

typedef struct {
  unsigned major;
  unsigned minor;
} HTTPVersion;

typedef struct {
  char* name;
  char* value;
} HeaderPair;

DEFINE_VECTOR_HEADER(HeaderPair, hpair)

typedef struct {
  hpair_vector pairs;
} HeaderCollection;

typedef struct {
  char* data;
  size_t length;
} Body;

typedef struct {
  MethodType method;
  URI* uri;
  HTTPVersion version;
  HeaderCollection* headers;
  Body* body;
  URIType type;
} Request;

char* get_line(ConnectionSocket *connection, bytes previous_data,
               bytes remaining_data);

MethodType string_to_methodtype(const char* method_name);

const char* methodtype_to_string(MethodType method);

int get_method(const char* method_string, Request* request);

bool has_scheme(const char* uri_string);

const char* get_scheme(const char* uri_string, URI* uri);

int get_host(const char* host_string, Authority* authority);

int get_port(const char* port_string, Authority* authority);

Host* new_host();

Authority* new_authority();

const char* get_authority(const char* uri_string, URI* uri);

Path* new_path();

int get_path(const char* path_string, Path* path);

char* get_relative(const char* uri_string, URI* uri);

int get_uri(const char* uri_string, URI* uri, MethodType method);

URI* new_uri();

int receive_request(ConnectionSocket *connection, Request* request);

char* path_string(Path* path);

#endif
