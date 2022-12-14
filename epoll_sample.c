#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include "lib/tlpi_hdr.h"
#include "lib/read_line.h"

#define EVENTS 16
#define BUF_SIZE 1024

enum my_state{
  READ = 0, WRITE
};

struct client_info{
  char buf[1024];
  ssize_t n;
  enum my_state state;
  int fd;
};

int main(){
  struct epoll_event ev_ret[EVENTS];
  int dic[1000];
  int sock0 = socket(AF_INET, SOCK_STREAM, 0);
  if(sock0 == -1)
    errExit("socket");

  if(setsockopt(sock0, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) == -1)
    errExit("socket");
  if(setsockopt(sock0, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int)) == -1)
    errExit("socket");

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(8080);
  addr.sin_addr.s_addr = INADDR_ANY;
  if(bind(sock0, (struct sockaddr*) &addr, sizeof(addr)) == -1)
    errExit("bind");

  if(listen(sock0, 5)== -1)
    errExit("listen");

  int ep_fd = epoll_create(EVENTS);
  if(ep_fd < 0)
    errExit("epoll_create");

  struct epoll_event ev0;
  memset(&ev0, 0 ,sizeof(ev0));
  ev0.events = EPOLLIN;
  ev0.data.ptr = malloc(sizeof(struct client_info));
  if(ev0.data.ptr == NULL)
    errExit("malloc");

  memset(ev0.data.ptr, 0, sizeof(struct client_info));
  ((struct client_info*)ev0.data.ptr)->fd = sock0;

  if(ioctl(sock0, FIONBIO, &(int){1}) == -1)
    errExit("ioctl");

  if(epoll_ctl(ep_fd, EPOLL_CTL_ADD, sock0, &ev0) != 0)
    errExit("epoll_ctl");

  for(;;){
    int n_fds = epoll_wait(ep_fd, ev_ret, EVENTS, -1);
    if(n_fds < 0)
      errExit("epoll_wait");

    printf("after epoll_wait : n_fds=%d\n", n_fds);

    for(ssize_t i=0; i<n_fds; i++){
      struct client_info *ci = ev_ret[i].data.ptr;
      printf("fd=%d\n", ci->fd);

      if(ci->fd == sock0){
        struct sockaddr_in client;
        socklen_t len = sizeof(client);
        int sock = accept(sock0, (struct sockaddr*)&client, &len);
        if(sock < 0)
          errExit("accept");

        // SUSv3 does not define about the option inheritance of accept().
        if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) == -1)
          errExit("socket");
        if(setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int)) == -1)
          errExit("socket");

        printf("accept sock=%d\n", sock);
        struct epoll_event ev;
        memset(&ev, 0, sizeof(ev));
        ev.events = EPOLLIN | EPOLLONESHOT;
        ev.data.ptr = malloc(sizeof(struct client_info));
        if(ev.data.ptr == NULL)
          errExit("malloc");

        memset(ev.data.ptr, 0, sizeof(struct client_info));
        ((struct client_info*)ev.data.ptr)->fd = sock;

        if(ioctl(sock, FIONBIO, &(int){1}) == -1)
          errExit("ioctl");

        if(epoll_ctl(ep_fd, EPOLL_CTL_ADD, sock, &ev) != 0)
          errExit("epoll_ctl");
      }else{
        if(ev_ret[i].events & EPOLLIN){
          ci->n = readLine(ci->fd , ci->buf, BUF_SIZE);
          if(ci->n < 0)
            errExit("read");

          char *tmp;
          char *tmp2;
          char *end;
          int key;
          int val;
          switch (ci->buf[0]) {
            case 'r':
              tmp = strtok(ci->buf, " ");
              tmp = strtok(NULL, " ");
              key = strtol(tmp, &end, 10);
              sprintf(ci->buf, "READ Key: %ld Value: %d\n", key, dic[key]);
              ci->n = strlen(ci->buf);
              break;
            case 'w':
              tmp = strtok(ci->buf, " ");
              tmp = strtok(NULL, " ");
              tmp2 = strtok(NULL, " ");
              key = strtol(tmp, &end, 10);
              val = strtol(tmp2, &end, 10);
              dic[key] = val;
              sprintf(ci->buf, "WRITE Key: %ld Value: %d\n", key, dic[key]);
              ci->n = strlen(ci->buf);
              break;
            default:
              strcpy(ci->buf, "Invalid command\n");
              ci->n = strlen(ci->buf);
          }

          // format: r 1   w 2 3

          ci->state = WRITE;
          ev_ret[i].events = EPOLLOUT;

          if(epoll_ctl(ep_fd, EPOLL_CTL_MOD, ci->fd, &ev_ret[i]) != 0)
            errExit("epoll_ctl");
        }else if(ev_ret[i].events & EPOLLOUT){
          ssize_t n = write(ci->fd , ci->buf, ci->n);
          if(n < 0)
            errExit("write");

          if(epoll_ctl(ep_fd, EPOLL_CTL_DEL, ci->fd, &ev_ret[i]) != 0)
            errExit("epoll_ctl");

          close(ci->fd);
          free(ev_ret[i].data.ptr);
        }
      }
    }
  }

  close(sock0);
  return 0;
}