#ifndef	_SOCKET_RTOS_H_
#define	_SOCKET_RTOS_H_

void rtos_socket_init(OS_EVENTFROUP *e);
int rtos_socket(int nf, int type, int protocol);
int rtos_sendto(int sock, char* buf, int len, int flags, void* to, void* tolen);
int rtos_recvfrom(int sock, void* buf, int len, unsigned int flags, void* from, void* fromlen);
int rtos_close(int sock);
#endif
