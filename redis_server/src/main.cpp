#include <cstring>

#include <iostream>
#include <string>

#include "common.h"
#include "epoll_pro.h"
#include "epoll_pro_ex.h"
#include "fork_pro.h"
#include "select_pro.h"
#include "thread_pro.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

int main(int argc, char **argv) {
  struct sockaddr_in sin;
  int sfd, sClient;
  int port = 6379;
  int retVal = 0;
  string serverIP = "127.0.0.1";

  memset(&sin, sizeof(sin), 0);
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);

  sfd = socket(AF_INET, SOCK_STREAM, 0);
  retVal = bind(sfd, (struct sockaddr *)&sin, sizeof(sin));
  if (SOCKET_ERROR == retVal) {
    cout << "bind fail!" << endl;
    goto end;
  }

  retVal = listen(sfd, 1);
  if (SOCKET_ERROR == retVal) {
    cout << "listen failed!" << endl;
    goto end;
  }
  //  epoll_pro(sfd);
  epoll_pro_ex(sfd);
//  select_pro(sfd);
// fork_pro(sfd);
// thread_pro(sfd);

end:
  close(sfd);
  return retVal;
}
