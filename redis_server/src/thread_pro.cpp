#include "common.h"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>

#include "thread_pro.h"

using namespace std;

int process(int sClient);

void *thread_for_client(void *args) {
  THREAD_INFO *threadInfo = (THREAD_INFO *)args;
  if (NULL == args) {
    cout << "input is NULL!" << endl;
    return NULL;
  }
  process(threadInfo->clientId);
  close(threadInfo->clientId);
  return NULL;
}

int create_thread(int sClient) {
  pthread_t thread;
  void *retVal;
  THREAD_INFO threadInfo;
  memset(&thread, 0, sizeof(thread));
  memset(&threadInfo, 0, sizeof(threadInfo));
  threadInfo.clientId = sClient;
  int errCode = pthread_create(&thread, NULL, thread_for_client, &threadInfo);
  if (0 != errCode) {
    cout << "pthread_create err,err code is " << errCode << endl;
    return false;
  }
  pthread_join(thread, &retVal);
  return true;
}

void thread_pro(int sfd) {
  sockaddr_in addrClient;
  int addrClientlen = sizeof(addrClient);
  while (true) {
    int sClient = accept(sfd, (struct sockaddr *)&addrClient,
                         (socklen_t *)&addrClientlen);
    if (SOCKET_ERROR == sClient) {
      cout << "sfd:" << sfd << "accept failed!" << endl;
      continue;
    }
    create_thread(sClient);
    close(sClient);
  }
}
