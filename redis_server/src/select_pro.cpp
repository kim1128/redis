#include "select_pro.h"
#include "common.h"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
using namespace std;

void select_pro(int serFd) {
  int ret = 0;
  int sClient;
  vector<int> clientId;

  sockaddr_in addrClient;
  int addrClientlen = sizeof(addrClient);

  while (1) {
    int maxId = serFd;
    fd_set fdRead;
    FD_ZERO(&fdRead);
    FD_SET(serFd, &fdRead);
    for (int i = 0; i < clientId.size(); i++) {
      int id = clientId.at(i);
      FD_SET(id, &fdRead);
      maxId = (maxId > id) ? maxId : id;
    }

    ret = select(maxId + 1, &fdRead, NULL, NULL, NULL);
    if (ret <= 0) {
      cout << "serverFd:" << serFd << "select fail!" << ret << endl;
      continue;
    }

    if (FD_ISSET(serFd, &fdRead)) {
      sClient = accept(serFd, (struct sockaddr *)&addrClient,
                       (socklen_t *)&addrClientlen);
      if (SOCKET_ERROR == sClient) {
        cout << "serverFd:" << serFd << "accept failed!" << endl;
        continue;
      }
      clientId.push_back(sClient);
    }

    for (int i = 0; i < clientId.size(); i++) {
      int id = clientId.at(i);
      if (FD_ISSET(id, &fdRead)) {
        process(id);
        clientId.erase(clientId.begin() + i);
        close(id);
        FD_CLR(id, &fdRead);
      }
    }
  }
}
