#ifndef _HTTP_LIB_H_
#define _HTTP_LIB_H_

#include "network_util.h"

FullSocket get_http_socket(const char *address);

typedef struct {
  char* scheme;
  char* authority;
  char* path;
  char* options;
  char* fragment;
} URI;

typedef struct {
  unsigned major_version;
  unsigned minor_version; 
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
  char* method;
  URI* uri;
  HTTPVersion version;
  HeaderCollection* headers;
  Body* body;
} Request;

char* get_line(ConnectionSocket *connection, bytes remaining_data);

char* get_method(char* first_line, Request* request);

char* get_uri(char* first_line, Request* request);

char* get_version(char* first_line, Request* request);

Request receive_request(ConnectionSocket* connection);

#endif
