#include "http.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static const char *status_text(int status_code){
  switch(status_code){
    case 200: return "OK";
    case 400: return "Bad Request";
    case 403: return "Forbidden";
    case 404: return "Not Found";
    case 405: return "Method Not Allowed";
    case 500: return "Internal Server Error";
    default: return "Unknown Status";
  }
}

int parse_http_request(const char *request, HttpRequest *http_request){
  char method[16];
  char version[16];
  char line[BUFFER_SIZE];
  char header_name[64];
  char header_value[256];

  strncpy(line, request,sizeof(line)-1);
  line[sizeof(line)-1] = '\0';

  if(sscanf(line, "%15s %4095s %15s", method, http_request->uri, version) != 3 || strcmp(version, "HTTP/1.1") != 0){
    return 400; 
  }

  if(strcmp(method, "GET") != 0){ // Solo se acepta el metodo GET
    return 405;
  }

  http_request -> host[0] = '\0';
  http_request -> user_agent[0] = '\0';
  http_request -> keep_alive = 1;

  strncpy(http_request -> connection, "keep-alive", sizeof(http_request -> connection) - 1); // Copiar cadenas de caracteres

  const char *crlf = strstr(request, "\r\n");
  if(!crlf) return 400;
  const char *cursor = crlf + 2;

  while(*cursor && strncmp(cursor, "\r\n", 2) !=0 ){
    const char *colon = strchr(cursor, ':');
    if(!colon){
      return 400;
    }
    size_t name_len = colon - cursor;
    if(name_len >= sizeof(header_name)){
      return 400;
    }

    strncpy(header_name, cursor, name_len);
    header_name[name_len] = '\0'; // Terminar la cade de caracteres
    const char *value_start = colon + 2;
    const char *eol = strstr(value_start, "\r\n");
    if(!eol){
      return 400;
    }

    size_t value_len = eol - value_start;
    if(value_len >= sizeof(header_value)){
      return 400;
    }

    strncpy(header_value, value_start, value_len); // Copiar a header_value
    header_value[value_len] = '\0';

    if(strcmp(header_name, "Host") == 0){
      strncpy(http_request -> host, header_value, sizeof(http_request -> host) - 1);
    } else if(strcmp(header_name, "User-Agent") == 0){
      strncpy(http_request -> user_agent, header_value, sizeof(http_request -> user_agent) - 1);
    } else if(strcmp(header_name, "Connection") == 0){
      if(strcasecmp(header_value, "keep-alive") == 0){
        http_request -> keep_alive = 1;
      } else {
        http_request -> keep_alive = 0;
      }
    }

    cursor = eol + 2; 

  }

  if(http_request->host[0] == '\0'){
    return 400; 
  }

  return 200;

}

void send_response(int fd, int status_code, const char *content_type, const char *body, size_t body_length) {
  char header[BUFFER_SIZE];
  int header_len = snprintf(header, sizeof(header),
      "HTTP/1.1 %d %s\r\n"
      "Content-Type: %s\r\n"
      "Content-Length: %zu\r\n"
      "Connection: keep-alive\r\n"
      "\r\n",
      status_code, status_text(status_code),
      content_type ? content_type : "text/plain", body_length);

  write(fd, header, header_len);
  if(body && body_length > 0){
    write(fd, body, body_length);
  }
}
