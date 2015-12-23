#include "includes.h"
#include "rtos.h"
#include "socket_rtos.h"
#include "inc/macro.h"

#define MAX_SOCK_NUM		8
#define MAX_SOCK_DATA_LEN	4096
#define MSG_DONTWAIT		1

struct _socket_tab_item
{
	int sock;
	OS_QUEUE* _queue;
};
typedef struct _socket_tab_item SOCKET_ITEM;

struct _socket_tab
{
	unsigned int bitmap;
	SOCKET_ITEM _sock[MAX_SOCK_NUM];
	int cnt;
	OS_EVENTGROUP* _eloop_events;
	
};
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

static void rtos_socket_register_events(OS_EVENTGROUP* e)
{
	sk_tab._eloop_events = e;
}

void rtos_socket_init(OS_EVENTGROUP *e)
{
	memset(&sk_tab, sizeof(sk_tab), 0);
	rtos_socket_register_events(e);
}

int rtos_socket(int nf, int type, int protocol)
{
	int id;
	id = _alloc_sock_id();
	if(id < 0)
	{
		wpa_printf(MSG_DEBUG, "reach max socket, cannot alloc anymore.\n");
		return -1;
	}
	
	(SETBIT(&sk_tab.bitmap, id));
	sk_tab._sock[id].sock = BIT(id);
	OS_Create_Queue(&sk_tab._sock[id]._queue, "new_sock", 4, 1, NULL);
	return id;
}

#if 1
int rtos_sendto(int sock, char* buf, unsigned int len, int flags, void* to, void* tolen)
{
	int acutal_len;

	if(len > MAX_SOCK_DATA_LEN) len = MAX_SOCK_DATA_LEN;
	char* _msg = os_malloc(len + sizeof(short));
	if(_msg == NULL)
	{
		wpa_printf(MSG_DEBUG, "socket failed to send to malloc\n");
	}
	//header for len: sizeof(short) bytes
	*((short*)_msg) = len;
	os_memcpy(_msg + sizeof(short), buf, len);
	if(sk_tab._sock[sock]._queue != NULL)
	{
		OS_Send_To_Queue(sk_tab._sock[sock]._queue, &_msg, 1, OS_SUSPEND_NO_TIMEOUT, NULL);
		//when flags equals to 0xFF, test mode for socket' sending and receiving
		if(flags != 0xFF)
		{
			OS_Set_Events(sk_tab._eloop_events, BIT(sock), NULL);
		}
		acutal_len = len;
	}
	else
	{
		acutal_len = 0;
		os_free(_msg);
		wpa_printf(MSG_DEBUG, "socket failed to send to <OS_Send_To_Queue>\n");
	}
	return acutal_len;
}
#endif

int rtos_recvfrom(int sock, void* buf, int len, unsigned int flags, void* from, void* fromlen)
{
	void *msg;
	int actual_len;
	unsigned int size;
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
	OS_Receive_From_Queue(sk_tab._sock[sock]._queue, &msg, 1, &size, opt, NULL);

	actual_len = *((short*)msg);
	os_memcpy(buf, ((char*)msg + sizeof(short)), len);
	os_free(msg);
	return actual_len;
}

int rtos_close(int sock)
{
	if(sock > 0 && sock < MAX_SOCK_NUM)
	{
		int counter = sk_tab._sock[sock]._queue->uxMessagesWaiting;
		if(counter > 0)
		{
			unsigned int i, size;
			void *msg;
			wpa_printf(MSG_DEBUG, "socket queue still hold %d data", counter);
			for(i = 0; (i < counter + 2)&&(sk_tab._sock[sock]._queue->uxMessagesWaiting > 0); i++)
			{
				OS_Receive_From_Queue(sk_tab._sock[sock]._queue, &msg, 1, &size, OS_SUSPEND_NO_TIMEOUT, NULL);
				os_free(msg);
				wpa_printf(MSG_DEBUG, "delete one");
			}
		}
		OS_Delete_Queue(sk_tab._sock[sock]._queue, NULL);
		sk_tab._sock[sock]._queue = NULL;
		sk_tab._sock[sock].sock = -1;
		CLRBIT(&sk_tab.bitmap, sock);
		return 0;
	}
	else
	{
		wpa_printf(MSG_DEBUG, "socket exceeds the regular range\n");
		return -1;
	}
}
