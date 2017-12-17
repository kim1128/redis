#include "fork_pro.h"
#include "common.h"
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

int create_fork(int sClient) {
  pid_t pid;
  int bRet = 0;

  if (0 == (pid = fork())) {
    pid_t ppid;
    if (0 == (ppid = fork())) {
      bRet = process(sClient);
      close(sClient);
      exit(0);
    }
    exit(0);
  } else {
    waitpid(pid, &bRet, WUNTRACED);
  }
  close(sClient);
  return bRet;
}

void fork_pro(int sfd) {
  sockaddr_in addrClient;
  int addrClientlen = sizeof(addrClient);
  while (true) {
    int sClient = accept(sfd, (struct sockaddr *)&addrClient,
                         (socklen_t *)&addrClientlen);
    if (SOCKET_ERROR == sClient) {
      cout << "sfd:" << sfd << "accept failed!" << endl;
      continue;
    }
    create_fork(sClient);
  }
}
