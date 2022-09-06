#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "./lib/error_functions.h"
#include "lib/read_line.h"
#include "silo/tuple.h"

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

  clock_t start = clock();
  double seconds = 0;
  ssize_t i = 0;

  for(;;i++){
    int r_w = rand() % 2;
    int key = rand() % TUPLE_NUM;
    int value = rand() & 100;

    char str[100];
    if(r_w == 0){
      sprintf(str, "r %d\n", key);
    }else{
      sprintf(str, "w %d %d\n", key, value);
    }
    int p = write(socket_fd, str, strlen(str));
    if(p == -1)
      errExit("write");

    readLine(socket_fd, recv_buff, sizeof(recv_buff));

    clock_t end = clock();
    seconds = (double)(end - start) / CLOCKS_PER_SEC;
    if(seconds > 10){
      break;
    }
  }

  double throughput = ((double) i) / seconds;
  printf("Throughput: %lf seconds: %lf\n", throughput, seconds);

  close(socket_fd);
  return 0;
}