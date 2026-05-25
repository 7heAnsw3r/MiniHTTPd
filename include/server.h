#pragma once

#define PORT 8000
#define BACKLOG 10
#define MAX_EVENTS 64
#define WWW_ROOT "./www"

int init_server(int port);
void run_server(int server_fd);
