#include "common.h"
#include <arpa/inet.h>
#include <cstring>
#include <errno.h>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <sstream>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
using namespace std;

#define BUF_SIZE 4096
class ClientInfo {
public:
  string readBuf;
  string writeBuf;
  int clientId;
  ClientInfo(int id);
  int read();
  int write();
  ClientInfo &operator=(const ClientInfo &c);
};

#define MAX_EVENTS 10000

ClientInfo::ClientInfo(int id) { clientId = id; }

ClientInfo &ClientInfo::operator=(const ClientInfo &c) {
  this->clientId = c.clientId;
  this->readBuf = c.readBuf;
  this->writeBuf = c.writeBuf;
}
int ClientInfo::read() {
  char buf[BUF_SIZE];
  memset(buf, BUF_SIZE, 0);

  int retVal = recv(clientId, buf, BUF_SIZE, 0);
  if (SOCKET_ERROR == retVal) {
    cout << "recv failed!" << retVal << endl;
    return -1;
  }
  readBuf = string(buf);

  string sendStr = string("OK!");
  stringstream stream;
  stream << "$" << sendStr.size() << "\r\n" << sendStr << "\r\n";
  writeBuf = stream.str();

  return 1;
}
int ClientInfo::write() {
  // cout << clientId << writeBuf << endl;
  send(clientId, writeBuf.c_str(), writeBuf.size(), 0);
  return 1;
}

void do_server(int sfd, int efd, map<int, ClientInfo> *mymap) {
  sockaddr_in addrClient;
  int addrClientlen = sizeof(addrClient);
  int client =
      accept(sfd, (struct sockaddr *)&addrClient, (socklen_t *)&addrClientlen);
  if (-1 == client) {
    cout << "accept fail!" << endl;
    return;
  }

  epoll_event ev;
  memset(&ev, 0, sizeof(ev));
  ev.data.fd = client;
  ev.events = EPOLLIN;
  if (-1 == epoll_ctl(efd, EPOLL_CTL_ADD, client, &ev)) {
    cout << "sfd:" << sfd << ",epoll_ctl fail!" << endl;
    return;
  }

  ClientInfo clientInfo(client);
  map<int, ClientInfo>::iterator it = mymap->begin();
  mymap->insert(it, pair<int, ClientInfo>(client, clientInfo));
}

void epoll_in(int efd, epoll_event *ev, map<int, ClientInfo> *mymap) {
  map<int, ClientInfo>::iterator it;
  int clientId = ev->data.fd;
  it = mymap->find(clientId);
  if (it == mymap->end()) {
    cout << "clientId=" << clientId << "not exist!" << endl;
    return;
  }

  ClientInfo *clientInfo = &it->second;
  clientInfo->read();
  // cout << clientInfo->writeBuf << endl << clientInfo->readBuf << endl;
  ev->events = ev->events & ~EPOLLIN;
  ev->events = ev->events | EPOLLOUT;

  epoll_ctl(efd, EPOLL_CTL_MOD, clientId, ev);
}

int epoll_out(int efd, epoll_event *ev, map<int, ClientInfo> *mymap) {
  map<int, ClientInfo>::iterator it;
  int clientId = ev->data.fd;
  it = mymap->find(clientId);
  if (it == mymap->end()) {
    cout << "clientId=" << clientId << "not exist!" << endl;
    return -1;
  }

  ClientInfo *clientInfo = &it->second;
  clientInfo->write();

  ev->events = ev->events & ~EPOLLOUT;

  if (0 == ev->events) {
    epoll_ctl(efd, EPOLL_CTL_DEL, clientId, ev);
    close(clientId);
    mymap->erase(it);
  }

  return 1;
}

int epoll_pro_ex(int sfd) {
  int efd;
  struct epoll_event ev;
  struct epoll_event *events = NULL;
  map<int, ClientInfo> clientMap;

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
      if (sfd == events[n].data.fd) {
        do_server(sfd, efd, &clientMap);
      } else if (EPOLLIN == events[n].events) {
        epoll_in(efd, &events[n], &clientMap);
      } else if (EPOLLOUT == events[n].events) {
        epoll_out(efd, &events[n], &clientMap);
      }
    }
  }
  close(efd);
  free(events);
}
