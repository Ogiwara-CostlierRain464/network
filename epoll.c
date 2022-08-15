#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include "lib/tlpi_hdr.h"

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
  int sock0 = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(8080);
  addr.sin_addr.s_addr = INADDR_ANY;
  if(bind(sock0, (struct sockaddr*) &addr, sizeof(addr)) == -1)
    errExit("bind");

  listen(sock0, 5);
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

        printf("accept sock=%d\n", sock);
        struct epoll_event ev;
        memset(&ev, 0, sizeof(ev));
        ev.events = EPOLLIN | EPOLLONESHOT;
        ev.data.ptr = malloc(sizeof(struct client_info));
        if(ev.data.ptr == NULL)
          errExit("malloc");

        memset(ev.data.ptr, 0, sizeof(struct client_info));
        ((struct client_info*)ev.data.ptr)->fd = sock;

        if(epoll_ctl(ep_fd, EPOLL_CTL_ADD, sock, &ev) != 0)
          errExit("epoll_ctl");
      }else{
        if(ev_ret[i].events & EPOLLIN){
          ci->n = read(ci->fd , ci->buf, BUF_SIZE);
          if(ci->n < 0)
            errExit("read");

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