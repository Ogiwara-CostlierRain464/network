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

int loop_sec = 10;
bool read_only = true;

int main(int argc, char* argv[]){
  int c;
  while ((c = getopt(argc, argv, "t:w")) != -1){
    switch (c) {
      case 't':
        loop_sec = atoi(optarg);
        break;
      case 'w':
        read_only = false;
        break;
      default:
        fprintf(stderr, "Usage: %s [-t secs] [-w] \n", argv[0]);
        exit(EXIT_FAILURE);
    }
  }

  printf("Loop sec: %d, Read/Write: %s\n", loop_sec, read_only ? "read only" : "read & write");

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
  ssize_t iter_count = 0;
  long latency_acc=0;

  for(;;){
    int r_w = read_only ? 0 : rand() % 2;
    int key = rand() % TUPLE_NUM;
    int value = rand() & 100;

    char str[100];
    if(r_w == 0){
      sprintf(str, "r %d\n", key);
    }else{
      sprintf(str, "w %d %d\n", key, value);
    }
    clock_t before = clock();
    int p = write(socket_fd, str, strlen(str));
    if(p == -1)
      errExit("write");
    readLine(socket_fd, recv_buff, sizeof(recv_buff));

    clock_t after = clock();
    latency_acc += after - before;

    iter_count++;

    seconds = (double)(after - start) / CLOCKS_PER_SEC;
    if(seconds > loop_sec){
      break;
    }
  }

  double throughput = ((double) iter_count) / seconds;
  double latency_ave = ((double)latency_acc / CLOCKS_PER_SEC) / iter_count;
  printf("Throughput: %lf Latency: %lf micro sec\n", throughput, latency_ave * 1000 * 1000);

  close(socket_fd);
  return 0;
}