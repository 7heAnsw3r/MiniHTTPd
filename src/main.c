#include "server.h"
#include <stdio.h>

int main(void){
  int server_fd = init_server(PORT);
  if(server_fd < 0){
    fprintf(stderr, "Failed to start server on port %d\n", PORT);
    return 1;
  }
  printf("Server running on http://localhost:%d\n", PORT);
  run_server(server_fd);

  return 0;
}
