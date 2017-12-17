#include "common.h"
#include <arpa/inet.h>
#include <cstring>
#include <errno.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
using namespace std;

#define MAX_EVENTS 10000
void do_server(int sfd, int efd, struct epoll_event *ev) {
  sockaddr_in addrClient;
  int addrClientlen = sizeof(addrClient);
  int client =
      accept(sfd, (struct sockaddr *)&addrClient, (socklen_t *)&addrClientlen);
  if (-1 == client) {
    cout << "accept fail!" << endl;
    return;
  }
  ev->data.fd = client;
  ev->events = EPOLLIN;
  if (-1 == epoll_ctl(efd, EPOLL_CTL_ADD, client, ev)) {
    cout << "sfd:" << sfd << ",epoll_ctl fail!" << endl;
    return;
  }
}

int epoll_pro(int sfd) {
  int efd;
  struct epoll_event ev;
  struct epoll_event *events = NULL;

  efd = epoll_create1(0);
  if (-1 == efd) {
    cout << "epoll_create1 fail!" << endl;
    return -1;
  }

  memset(&ev, 0, sizeof(ev));
  ev.data.fd = sfd;
  ev.events = EPOLLIN;
  if (-1 == epoll_ctl(efd, EPOLL_CTL_ADD, sfd, &ev)) {
    cout << "sfd:" << sfd << ",epoll_ctl fail!" << endl;
    return -1;
  }
  events =
      (struct epoll_event *)malloc(sizeof(struct epoll_event) * MAX_EVENTS);

  while (1) {
    int n, nfds;
    nfds = epoll_wait(efd, events, MAX_EVENTS, -1);
    if (-1 == nfds) {
      cout << "epoll_wait fail!" << endl;
      continue;
    }
    for (n = 0; n < nfds; n++) {
      if (sfd == events[n].data.fd) { // server socket
        do_server(sfd, efd, &ev);
      } else {
        process(events[n].data.fd);
        epoll_ctl(efd, EPOLL_CTL_DEL, events[n].data.fd, &ev);
        // close(events[n].data.fd);
      }
    }
  }
  close(efd);
  free(events);
}
