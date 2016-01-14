/*
 * WPA Supplicant / Windows Named Pipe -based control interface
 * Copyright (c) 2004-2006, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "includes.h"

#include "common.h"
#include "eloop.h"
#include "config.h"
#include "eapol_supp/eapol_supp_sm.h"
#include "wpa_supplicant_i.h"
#include "ctrl_iface.h"
#include "common/wpa_ctrl.h"

/* Per-interface ctrl_iface */

#define REQUEST_BUFSIZE 256
#define REPLY_BUFSIZE 4096
#define MAX_CTRL_QUEUE_LEN	4

//typedef uint32_t os_event;
//
//struct ctrl_iface_tab_item
//{
//	char ifname[16];
//	OS_QUEUE* up_queue;
//	OS_QUEUE* down_queue;
//	os_event  e;
//	uint32_t  registered;
//};
//typedef struct ctrl_iface_tab_item ctrl_if_data;

struct ctrl_iface_tab_item ctrl_if_tab[8];

void ctrl_if_register(char* name, OS_QUEUE* up, OS_QUEUE* down, os_event e)
{
	int i;
	for(i = 0; i < 8; i++)
	{
		if(ctrl_if_tab[i].registered == 0) break;
	}
	if(i == 8) return;

	strncpy(ctrl_if_tab[i].ifname, name, 15);
	ctrl_if_tab[i].ifname[15] = '\0';
	ctrl_if_tab[i].up_queue = up;
	ctrl_if_tab[i].down_queue = down;
	ctrl_if_tab[i].e = e;
	ctrl_if_tab[i].registered = 1;
}

void ctrl_if_unregister(char* name)
{
	int i;
	for(i = 0; i < 8; i++)
	{
		if(strncmp(ctrl_if_tab[i].ifname, name, 15) == 0) break;
	}
	if(i == 8) return;
	
	os_memset((void*)(&ctrl_if_tab[i]), 0, sizeof(struct ctrl_iface_tab_item));
}

struct ctrl_iface_tab_item* ctrl_if_get(const char* name)
{
	int i;
	for(i = 0; i < 8; i++)
	{
		if(strncmp(ctrl_if_tab[i].ifname, name, 15) == 0) break;
	}
	if(i == 8)
		return NULL;
	
	return (struct ctrl_iface_tab_item* )&ctrl_if_tab[i];
}

void ctrl_if_list(void)
{
	int i;
	for(i = 0; i < 8; i++)
	{
		if(strlen(ctrl_if_tab[i].ifname) != 0)
			wpa_printf(MSG_DEBUG, "%d>> %s\n", i, ctrl_if_tab[i].ifname);
	}
}

struct ctrl_iface_priv;

/**
 * struct wpa_ctrl_dst - Internal data structure of control interface clients
 *
 * This structure is used to store information about registered control
 * interface monitors into struct wpa_supplicant. This data is private to
 * ctrl_iface_named_pipe.c and should not be touched directly from other files.
 */

struct wpa_ctrl_dst {
	os_event event;
	struct wpa_ctrl_dst *next, *prev;
	struct ctrl_iface_priv *priv;
	OS_QUEUE* up_queue;
	OS_QUEUE* down_queue;
	int attached;
	int debug_level;
	int errors;
	char req_buf[REQUEST_BUFSIZE];
	char *rsp_buf;
	int used;
};


struct ctrl_iface_priv {
	struct wpa_supplicant *wpa_s;
	struct wpa_ctrl_dst *ctrl_dst;
};


static void wpa_supplicant_ctrl_iface_send(struct ctrl_iface_priv *priv,
					   int level, const char *buf,
					   size_t len);

static void ctrl_close(struct wpa_ctrl_dst *dst, char* ctrl_name);
static void wpa_supplicant_ctrl_iface_receive(void *, void *);
static void ctrl_iface_read_completed(uint32_t err, uint32_t bytes);

struct wpa_global_dst;
static void global_close(struct wpa_global_dst *dst, char* name);
static void wpa_supplicant_global_iface_receive(void *eloop_data,
						void *user_ctx);
static void global_iface_read_completed(uint32_t err, uint32_t bytes);


static int ctrl_open(struct ctrl_iface_priv *priv, char* ctrl_name)
{
	struct wpa_ctrl_dst *dst;
	OS_STATUS ret;
	char name_array[20];

	dst = os_zalloc(sizeof(*dst));
	if (dst == NULL)
		return -1;
	wpa_printf(MSG_DEBUG, "CTRL: Open up and down queues %p", dst);

	dst->priv = priv;
	dst->debug_level = MSG_DEBUG;
	dst->up_queue = NULL;
	dst->down_queue = NULL;

	eloop_register_event(&dst->event,
			     sizeof(dst->event),
			     wpa_supplicant_ctrl_iface_receive, dst, NULL);

	strcpy(name_array, "up-\0");
	ret = OS_Create_Queue(&dst->up_queue, strncat(name_array, ctrl_name, 10), MAX_CTRL_QUEUE_LEN, 1, NULL);
	strcpy(name_array, "down-\0");
	ret = OS_Create_Queue(&dst->down_queue, strncat(name_array, ctrl_name, 10), MAX_CTRL_QUEUE_LEN, 1, NULL);
 
	if (ret != 0) {
		wpa_printf(MSG_ERROR, "CTRL: Create up and down queues failed: %d", ret);
		goto fail;
	}

	ctrl_if_register(ctrl_name, dst->up_queue, dst->down_queue, dst->event);	

	dst->next = priv->ctrl_dst;
	if (dst->next)
		dst->next->prev = dst;
	priv->ctrl_dst = dst;

	return 0;

fail:
	ctrl_close(dst, ctrl_name);
	return -1;
}


static void ctrl_close(struct wpa_ctrl_dst *dst, char* ctrl_name)
{
	wpa_printf(MSG_DEBUG, "CTRL: close ctrl interface%p", dst);

	ctrl_if_unregister(ctrl_name);

	if (dst->event) {
		eloop_unregister_event(&dst->event, sizeof(dst->event));
		dst->event = 0;
	}

	if (dst->up_queue != NULL) {
		int i, ret;
		unsigned int bytes;
		void *_buf;
		for(i = 0; i < MAX_CTRL_QUEUE_LEN; i++){
			ret = OS_Receive_From_Queue(dst->up_queue, &_buf, 1,
					&bytes,  OS_NO_SUSPEND, NULL);
			if(ret != 0)
				break;
			os_free(_buf);
		}
		OS_Delete_Queue(dst->up_queue, NULL);
	}
	if (dst->down_queue != NULL) {
		int i, ret;
		unsigned int bytes;
		void *_buf;
		for(i = 0; i < MAX_CTRL_QUEUE_LEN; i++){
			ret = OS_Receive_From_Queue(dst->down_queue, &_buf, 1,
					&bytes,  OS_NO_SUSPEND, NULL);
			if(ret != 0)
				break;
			os_free(_buf);
		}
		OS_Delete_Queue(dst->down_queue, NULL);
	}

	if (dst->prev)
		dst->prev->next = dst->next;
	else
		dst->priv->ctrl_dst = dst->next;
	if (dst->next)
		dst->next->prev = dst->prev;

	
	if(dst->rsp_buf) {
		os_free(dst->rsp_buf);
	}
	os_free(dst);
}


static void wpa_supplicant_ctrl_iface_rx(struct wpa_ctrl_dst *dst, size_t len)
{
	struct wpa_supplicant *wpa_s = dst->priv->wpa_s;
	char *reply = NULL, *send_buf;
	size_t reply_len = 0, send_len;
	int new_attached = 0;
	char *buf = dst->req_buf;
	int ret;

	dst->used = 1;
	if (len >= REQUEST_BUFSIZE)
		len = REQUEST_BUFSIZE - 1;
	buf[len] = '\0';

	if (os_strcmp(buf, "ATTACH") == 0) {
		dst->attached = 1;
		wpa_printf(MSG_DEBUG, "CTRL_IFACE monitor attached");
		new_attached = 1;
		reply_len = 2;
	} else if (os_strcmp(buf, "DETACH") == 0) {
		dst->attached = 0;
		wpa_printf(MSG_DEBUG, "CTRL_IFACE monitor detached");
		reply_len = 2;
	} else if (os_strncmp(buf, "LEVEL ", 6) == 0) {
		wpa_printf(MSG_DEBUG, "CTRL_IFACE LEVEL %s", buf + 6);
		dst->debug_level = atoi(buf + 6);
		reply_len = 2;
	} else {
		reply = wpa_supplicant_ctrl_iface_process(wpa_s, buf,
							  &reply_len);
	}

	if (reply) {
		send_buf = reply;
		send_len = reply_len;
	} else if (reply_len == 2) {
		send_buf = "OK\n\0";
		send_len = 3;
	} else {
		send_buf = "FAIL\n\0";
		send_len = 5;
	}

	os_free(dst->rsp_buf);
	dst->rsp_buf = os_malloc(send_len + 1);
	if (dst->rsp_buf == NULL) {
		ctrl_close(dst, wpa_s->conf->ctrl_interface);
		os_free(reply);
		return;
	}
	os_memcpy(dst->rsp_buf, send_buf, send_len);
	dst->rsp_buf[send_len] = '\0';
	os_free(reply);

	if ((ret = OS_Send_To_Queue(dst->down_queue, &dst->rsp_buf, 1,  MS_TO_TICK_COUNT(3000), NULL)) != 0) {
		wpa_printf(MSG_DEBUG, "CTRL: WriteFileEx failed: %d",
			   (int) ret);
	} else {
		wpa_printf(MSG_DEBUG, "CTRL: Overlapped write started for %p",
				dst);
	}

	if (new_attached)
		eapol_sm_notify_ctrl_attached(wpa_s->eapol);
}


static void wpa_supplicant_ctrl_iface_receive(void *eloop_data, void *user_ctx)
{
	struct wpa_ctrl_dst *dst = eloop_data;
	void* _buf;
	uint32_t bytes;
	OS_STATUS ret;

	if ((ret = OS_Receive_From_Queue(dst->up_queue, &_buf, 1, &bytes,  MS_TO_TICK_COUNT(3000), NULL)) != 0) {
		wpa_printf(MSG_DEBUG, "CTRL: No Message Read. %d",
			   (int)(ret));
		return;
	}

	/*copy message data to request_buf*/
	os_memcpy(dst->req_buf, _buf, REQUEST_BUFSIZE);
	os_free(_buf);

	/* Use write completion function to start reading a command */
	wpa_supplicant_ctrl_iface_rx(dst, REQUEST_BUFSIZE);
}


static void wpa_supplicant_ctrl_iface_msg_cb(void *ctx, int level, int global,
					     const char *txt, size_t len)
{
	struct wpa_supplicant *wpa_s = ctx;
	if (wpa_s == NULL || wpa_s->ctrl_iface == NULL)
		return;
	/**
	 * TODO: I think that this function need a pair of new queues: up and down;
	 * wpa_supplicant_ctrl_iface_wait() and wpa_supplicant_ctrl_iface_send should
	 * be couple of functions;
 	 */
	//wpa_supplicant_ctrl_iface_send(wpa_s->ctrl_iface, level, txt, len);
}


struct ctrl_iface_priv *
wpa_supplicant_ctrl_iface_init(struct wpa_supplicant *wpa_s)
{
	struct ctrl_iface_priv *priv;

	priv = os_zalloc(sizeof(*priv));
	if (priv == NULL)
		return NULL;
	priv->wpa_s = wpa_s;

	if (wpa_s->conf->ctrl_interface == NULL)
		return priv;

	if (ctrl_open(priv, wpa_s->conf->ctrl_interface) < 0) {
		os_free(priv);
		return NULL;
	}
	else{
		wpa_printf(MSG_DEBUG, "ctrl_open name: %s\n", wpa_s->conf->ctrl_interface);
	}

	wpa_msg_register_cb(wpa_supplicant_ctrl_iface_msg_cb);

	return priv;
}


void wpa_supplicant_ctrl_iface_deinit(struct ctrl_iface_priv *priv)
{
	while (priv->ctrl_dst)
		//ctrl_close(priv->ctrl_dst, priv->global->param.ctrl_interface);
		ctrl_close(priv->ctrl_dst, priv->wpa_s->conf->ctrl_interface);
	os_free(priv);
}


static void wpa_supplicant_ctrl_iface_send(struct ctrl_iface_priv *priv,
					   int level, const char *buf,
					   size_t len)
{
	struct wpa_ctrl_dst *dst, *next;
	char levelstr[10];
	char *sbuf;
	int llen;

	dst = priv->ctrl_dst;
	if (dst == NULL)
		return;

	os_snprintf(levelstr, sizeof(levelstr), "<%d>", level);

	llen = os_strlen(levelstr);
	sbuf = os_malloc(llen + len);
	if (sbuf == NULL)
		return;

	os_memcpy(sbuf, levelstr, llen);
	os_memcpy(sbuf + llen, buf, len);

	if(dst)
	{
		uint32_t msg;
		msg = (uint32_t)(sbuf);
		OS_Send_To_Queue(dst->down_queue, &msg, 1, 1, NULL);
		//os_free(sbuf);
	}
	os_free(sbuf);
}

/*TODO: need to finish*/
void wpa_supplicant_ctrl_iface_wait(struct ctrl_iface_priv *priv)
{
	uint32_t msg;
	uint32_t actual;
	struct wpa_ctrl_dst *dst = priv->ctrl_dst;

	wpa_printf(MSG_DEBUG, "CTRL_IFACE - %s - wait for monitor",
		   priv->wpa_s->ifname);
	if (priv->ctrl_dst == NULL)
		return;
	OS_Receive_From_Queue(dst->up_queue, &msg, 1, &actual, OS_SUSPEND_NO_TIMEOUT, NULL);
}


/* Global ctrl_iface */

struct ctrl_iface_global_priv;

struct wpa_global_dst {
	/* Note: OVERLAPPED must be the first member of struct wpa_global_dst
	 */
	os_event event;
	struct wpa_global_dst *next, *prev;
	struct ctrl_iface_global_priv *priv;
	OS_QUEUE* up_queue;
	OS_QUEUE* down_queue;
	int attached;
	int debug_level;
	int errors;
	char req_buf[REQUEST_BUFSIZE];
	char *rsp_buf;
	int used;
};

struct ctrl_iface_global_priv {
	struct wpa_global *global;
	struct wpa_global_dst *ctrl_dst;
};


static int global_open(struct ctrl_iface_global_priv *priv, char* name)
{
	struct wpa_global_dst *dst;
	OS_STATUS ret;
	char name_array[20];

	if(name == NULL) return -1;

	dst = os_zalloc(sizeof(*dst));
	if (dst == NULL)
		return -1;
	wpa_printf(MSG_DEBUG, "Global CTRL: Open pipe %p", dst);

	dst->priv = priv;
	//dst->debug_level = MSG_INFO;
	dst->up_queue = NULL;
	dst->down_queue = NULL;

	eloop_register_event(&dst->event,
			     sizeof(dst->event),
				 wpa_supplicant_global_iface_receive, dst, NULL);

	strcpy(name_array, "up-\0");
	ret = OS_Create_Queue(&dst->up_queue, strncat(name_array, name, 10), MAX_CTRL_QUEUE_LEN, 1, NULL);
	strcpy(name_array, "down-\0");
	ret = OS_Create_Queue(&dst->down_queue, strncat(name_array, name, 10), MAX_CTRL_QUEUE_LEN, 1, NULL);
 
	if (ret) {
		wpa_printf(MSG_ERROR, "Global CTRL: CreateNamedPipe failed: %d", ret);
		goto fail;
	}

	ctrl_if_register(name, dst->up_queue, dst->down_queue, dst->event);	

	dst->next = priv->ctrl_dst;
	if (dst->next)
		dst->next->prev = dst;
	priv->ctrl_dst = dst;

	return 0;

fail:
	global_close(dst, name);
	return -1;
}


static void global_close(struct wpa_global_dst *dst, char* name)
{
	wpa_printf(MSG_DEBUG, "Global CTRL: close ctrl interface %p", dst);

	ctrl_if_unregister(name);

	if (dst->event) {
		eloop_unregister_event(&dst->event, sizeof(dst->event));
		dst->event = 0;
	}

	if (dst->up_queue != NULL) {
		OS_Delete_Queue(dst->up_queue, NULL);
	}
	if (dst->down_queue != NULL) {
		OS_Delete_Queue(dst->down_queue, NULL);
	}

	if (dst->prev)
		dst->prev->next = dst->next;
	else
		dst->priv->ctrl_dst = dst->next;
	if (dst->next)
		dst->next->prev = dst->prev;

	
	if(dst->rsp_buf) {
		os_free(dst->rsp_buf);
	}
	os_free(dst);
}


static void wpa_supplicant_global_iface_rx(struct wpa_global_dst *dst,
					   size_t len)
{
	struct wpa_global *global = dst->priv->global;
	char *reply = NULL, *send_buf;
	size_t reply_len = 0, send_len;
	char *buf = dst->req_buf;
	OS_STATUS ret;

	dst->used = 1;
	if (len >= REQUEST_BUFSIZE)
		len = REQUEST_BUFSIZE - 1;
	buf[len] = '\0';

	wpa_printf(MSG_DEBUG,"Global wpa received cmd: %s", buf);

	reply = wpa_supplicant_global_ctrl_iface_process(global, buf,
							 &reply_len);
	if (reply) {
		send_buf = reply;
		send_len = reply_len;
	} else if (reply_len) {
		send_buf = "FAIL\n";
		send_len = 5;
	} else {
		os_free(dst->rsp_buf);
		dst->rsp_buf = NULL;
		return;
	}

	os_free(dst->rsp_buf);
	dst->rsp_buf = os_malloc(send_len + 1);
	if (dst->rsp_buf == NULL) {
		global_close(dst, global->params.ctrl_interface);
		os_free(reply);
		return;
	}
	os_memcpy(dst->rsp_buf, send_buf, send_len);
	dst->rsp_buf[send_len] = '\0';
	os_free(reply);

	if ((ret = OS_Send_To_Queue(dst->down_queue, &dst->rsp_buf, 1, OS_SUSPEND_NO_TIMEOUT, NULL)) != 0) {
		wpa_printf(MSG_DEBUG, "Global CTRL: WriteFileEx failed: %d",
			   (int) ret);
	} else {
		wpa_printf(MSG_DEBUG, "Global CTRL: Overlapped write started for %p",
			   dst);
	}
}

static void wpa_supplicant_global_iface_receive(void *eloop_data,
						void *user_ctx)
{
	struct wpa_global_dst *dst = eloop_data;
	struct ctrl_iface_global_priv *priv = dst->priv;
	void* _buf;
	uint32_t bytes;
	OS_STATUS ret;

	if ((ret = OS_Receive_From_Queue(dst->up_queue, &_buf, 1, &bytes, OS_NO_SUSPEND, NULL)) != 0) {
		wpa_printf(MSG_DEBUG, "Global CTRL: No Message Read. %d",
			   (int)(ret));
		return;
	}
	/*copy message data to request_buf*/
	os_memcpy(dst->req_buf, _buf, REQUEST_BUFSIZE);
	os_free(_buf);

	wpa_printf(MSG_DEBUG, "Global CTRL: GetOverlappedResult: New client "
		   "connected");

	/* Use write completion function to start reading a command */
	wpa_supplicant_global_iface_rx(dst, REQUEST_BUFSIZE);
}


struct ctrl_iface_global_priv *
wpa_supplicant_global_ctrl_iface_init(struct wpa_global *global)
{
	struct ctrl_iface_global_priv *priv;

	priv = os_zalloc(sizeof(*priv));
	if (priv == NULL)
		return NULL;
	priv->global = global;

	/* no global interface need configuring */
	if(global->params.ctrl_interface == NULL)
		return priv;

	if (global_open(priv, global->params.ctrl_interface) < 0) {
		os_free(priv);
		return NULL;
	}

	return priv;
}


void
wpa_supplicant_global_ctrl_iface_deinit(struct ctrl_iface_global_priv *priv)
{
	while (priv->ctrl_dst)
		global_close(priv->ctrl_dst, priv->global->params.ctrl_interface);
	os_free(priv);
}
