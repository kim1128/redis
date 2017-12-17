#include <cstring>

#include <iostream>
#include <string>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "resp.h"

using namespace std;

int main(int argc, char **argv) {
  struct sockaddr_in sin; //服务器的地址
  int sfd;
  int port = 6379;
  string serverIP = "127.0.0.1";

  memset(&sin, sizeof(sin), 0);
  sin.sin_family = AF_INET; //使用IPV4地址族
  inet_pton(AF_INET, serverIP.c_str(), (void *)&(sin.sin_addr));
  sin.sin_port = htons(port);

  sfd = socket(AF_INET, SOCK_STREAM, 0);
  connect(sfd, (struct sockaddr *)&(sin), sizeof(sin));

  string getcmd[] = {"LRANGE", "hk", "0", "10"};
  string buf2 = respSendMsg(getcmd, 4);
  write(sfd, buf2.c_str(), buf2.size());

  RESPMsg *debase = RESPMsg::decode(sfd);
  if (NULL == debase) {
    cout << "debase is null!" << endl;
    return 0;
  }
  debase->show();

  close(sfd);

  return 0;
}
