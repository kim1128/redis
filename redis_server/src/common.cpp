#include "common.h"
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <unistd.h>
using namespace std;

#define BUF_SIZE 1024
void sendMsgToClient(int sClient) {
  string sendStr = string("OK!");
  stringstream stream;
  stream << "$" << sendStr.size() << "\r\n" << sendStr << "\r\n";
  // cout << "客户端发送的数据: " << sendStr << endl;
  send(sClient, stream.str().c_str(), stream.str().size(), 0);
}

int process(int sClient) {
  char buf[BUF_SIZE];
  int retVal = SEND_SUC;
  memset(buf, BUF_SIZE, 0);

  retVal = recv(sClient, buf, BUF_SIZE, 0);
  // cout << "recv:" << retVal << endl;
  if (SOCKET_ERROR == retVal) {
    cout << "recv failed!" << retVal << endl;
    goto end;
  }
  if ("" != buf) {
    sendMsgToClient(sClient);
  }

end:
  close(sClient);
  return retVal;
}
