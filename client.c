#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "./lib/error_functions.h"
#include "lib/read_line.h"

int main(){
  char recv_buff[1024];
  memset(recv_buff, 0, sizeof(recv_buff));
  int socket_fd;
  if((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    errExit("socket");

  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(8080);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  if(connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    errExit("connect");


  for(size_t i = 0; i < 10000; i++){
    int r = rand() % 1;
    char *str;
    if(r == 0){
      str = "r 2\n";
    }else{
      str = "w 2 3\n";
    }
    int p = write(socket_fd, str, strlen(str));
    if(p == -1)
      errExit("write");

    readLine(socket_fd, recv_buff, sizeof(recv_buff));
    printf("Result: %s\n", recv_buff);
  }

  close(socket_fd);
  return 0;
}