#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <assert.h>

#define IP "127.0.0.1"
#define PORT 8080
#define ASSERT(exp) assert(exp)
#define Z_CLEAR(obj) memset(&obj, 0, sizeof(obj))

int main(int argc, char const* argv[]){
  struct sockaddr_in addr;
  int server_fd, opt = 1, new_socket;
  ssize_t val_read;
  int addr_len = sizeof(IP);
  char buffer[1024];
  char *hello = "Hello from server\n";

  ASSERT((server_fd = socket(AF_INET, SOCK_STREAM, 0)) != 0);

  Z_CLEAR(addr);
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(PORT);

  ASSERT(bind(server_fd,
              (struct sockaddr*) &addr,
                sizeof(addr)) >= 0);

  for(;;){
    Z_CLEAR(buffer);
    ASSERT(listen(server_fd, 3) >= 0);
    ASSERT((new_socket = accept(
      server_fd,
      (struct sockaddr*)&addr,
      (socklen_t*)&addr_len)) >= 0);

    read(new_socket, buffer, 1024);
    buffer[4] = '\0';
    printf("%s\n", buffer);
    send(new_socket, hello, strlen(hello), 0);

    close(new_socket);
    if(strcmp(buffer, "quit") == 0){
      break;
    }
  }

  shutdown(server_fd, SHUT_RDWR);

  return 0;
}