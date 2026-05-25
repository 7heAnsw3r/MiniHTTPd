#pragma once
#include <stddef.h>

#define MAX_URI_LENGTH   4096
#define BUFFER_SIZE      8192

typedef struct{
  char uri[MAX_URI_LENGTH];
  char host[256];
  char connection[32];
  char user_agent[256];
  int  keep_alive;
} HttpRequest;

int  parse_http_request(const char *request, HttpRequest *http_request);
void send_response(int fd, int status_code, const char *content_type, const char *body, size_t body_length);
