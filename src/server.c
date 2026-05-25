#include "server.h"
#include "http.h"
#include "files.h"
#include "mime.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>


static void set_nonblocking(int fd){
  int flags = fcntl(fd, F_GETFL,0);
  fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int init_server(int port){
  int server_fd = socket(AF_INET,SOCK_STREAM,0);
  if(server_fd < 0){
    perror("socket");
    return -1;
  }

  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  struct sockaddr_in addr = {0};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  if(bind(server_fd,(struct sockaddr *)&addr, sizeof(addr)) <0){
    perror("bind");
    close(server_fd);
    return -1;
  }

  if(listen(server_fd, BACKLOG) < 0){
    perror("listen");
    close(server_fd);
    return -1;
  }

  set_nonblocking(server_fd);
  return server_fd;
}

void run_server(int server_fd){
  int epoll_fd = epoll_create1(0);
  if(epoll_fd < 0){
    perror("epoll_create1");
    return;
  }

  struct epoll_event ev, events[MAX_EVENTS];
  ev.events = EPOLLIN;
  ev.data.fd = server_fd;
  epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev);

  while(1){
    int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
    for(int i=0; i < n; i++){
      int fd = events[i].data.fd;

      if(fd == server_fd){
        int client_fd = accept(server_fd, NULL, NULL);
        if(client_fd < 0){
          continue;
        }
        set_nonblocking(client_fd);
        ev.events = EPOLLIN;
        ev.data.fd = client_fd;
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev);
      }else{
        char buffer[BUFFER_SIZE];
        ssize_t bytes_read = read(fd, buffer, sizeof(buffer)-1);
        if(bytes_read <= 0){
          epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
          close(fd);
          continue;
        }
        buffer[bytes_read] = '\0';
        HttpRequest request;
        int status = parse_http_request(buffer, &request);

        if(status != 200){
          send_response(fd, status, NULL, NULL, 0);
          epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
          close(fd);
          continue;
        }

        const char *uri = request.uri;
        if(strcmp(uri, "/") == 0){
          uri = "/index.html";
        }

        FileResult file = serve_file(uri, WWW_ROOT);
        send_response(fd, file.status, get_mime_type(uri), file.data, file.size);

        free_file_result(&file);

        if(!request.keep_alive){
          epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
          close(fd);
        }
      }
    }
  }

  close(epoll_fd);

}
