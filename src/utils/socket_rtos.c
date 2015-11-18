#include "macro.h"
#include "rtos.h"

#define MAX_SOCK_NUM		8
#define MAX_SOCK_DATA_LEN	4096
#define MSG_DONTWAIT		1

struct _socket_tab_item
{
	int sock;
	OS_QUEUE* _queue;
}
typedef struct _socket_tab_item SOCKET_ITEM;

struct _socket_tab
{
	unsigned int bitmap;
	SOCKET_ITEM _sock[MAX_SOCK_NUM];
	int cnt;
	OS_EVENTGROUP* _eloop_events;
	
}
static struct _socket_tab sk_tab;

static int _alloc_sock_id(void)
{
	int i;
	for(i = 0; i < MAX_SOCK_NUM; i++)
	{
		if(!TESTBIT(&sk_tab.bitmap, i))
		{
			return i;
		}
	}
	return -1;
}

static void rtos_socket_register_events(OS_EVENTFROUP* e)
{
	sk_tab._eloop_events = e;
}

void rtos_socket_init(OS_EVENTFROUP *e)
{
	memset(&sk_tab, sizeof(sk_tab), 0);
	rtos_socket_register_events(e);
}

int rtos_socket(int nf, int type, int protocol)
{
	unsigned int id;
	id = _alloc_sock_id();
	if(id < 0)
	{
		printf("reach max socket, cannot alloc anymore.\n");
		return -1;
	}
	
	SETBIT(&sk_tab.bitmap, id);
	sk_tab._sock[id].sock = BIT(id);
	OS_Create_Queue(&sk_tab._sock[id]._queue, 4, 1, NULL);
	return id;
}

#if 1
int rtos_sendto(int sock, char* buf, int len, int flags, void* to, void* tolen)
{
	void* _msg = os_malloc(len);
	os_memcpy(_msg, buf, len);
	OS_Send_To_Queue(sk_tab._sock[sock]._queue, &_msg, OS_SUSPEND_NO_TIMEOUT, NULL);
	OS_Set_Events(sk_tab._eloop_events, BIT(sock), NULL);
}
#endif

int rtos_recvfrom(int sock, void* buf, int len, unsigned int flags, void* from, void* fromlen)
{
	unsigned int msg, size;
	unsigned char opt;

	if(len > MAX_SOCK_DATA_LEN) len = MAX_SOCK_DATA_LEN;
	if(flags & MSG_DONTWAIT)
	{
		opt = OS_NO_SUSPEND;
	}
	else
	{
		opt = OS_SUSPEND_NO_TIMEOUT;
	}
	OS_Receive_From_Queue(sk_tab._sock[sock],_queue, &msg, 1, &size, opt, NULL);

	os_memcpy(buf, (void*)(msg), len);
}

int rtos_close(int sock)
{
	if(sock > 0 && sock < MAX_SOCK_NUM)
	{
		CLRBIT(&sk_tab.bitmap, sock);
		return 0;
	}
	return -1;
}
