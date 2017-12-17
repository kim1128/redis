#ifndef __THREAD_PRO_H__
#define __THREAD_PRO_H__

typedef struct { int clientId; } THREAD_INFO;
// int create_thread(int sClient);
void thread_pro(int sfd);
#endif
