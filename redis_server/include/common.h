#ifndef COMMON_H
#define COMMON_H

enum {
  SOCKET_ERROR = -1,
  SELECT_ERROR,
  ACCEPT_ERROR,
  SELECT_SUC,
  SEND_SUC
} SOCKET_RETURN_CODE;

int process(int sClient);

#endif
