#include "includes.h"
#include "common.h"
#include "eloop.h"
#include "driver.h"
#include "common/qca-vendor.h"
#include "common/qca-vendor-attr.h"
#include "common/ieee802_11_defs.h"
#include "common/ieee802_11_common.h"
#include "l2_packet/l2_packet.h"

#include "hal.h"


//static const int frequency_list[] = {
//	2412, 2417, 2422, 2427, 2432, 2437, 2442,
//	2447, 2452, 2457, 2462, 2467, 2472, 2484
//};

struct bss_ie_hdr {
	u8 elem_id;
	u8 len;
	u8 oui[3];
	/* u8 oui_type; */
	/* u16 version; */
} __attribute__ ((packed));


struct wpa_driver_rtos_data {
	void *ctx;
//	unsigned int event;
	int sock;
	int operstate;
	char ifname[IFNAMSIZ + 1];
	char own_addr[ETH_ALEN];
};


static int wpa_driver_rtos_get_bssid(void *priv, u8 *bssid)
{
//	struct wpa_driver_rtos_data *drv = priv;
	if (rtos_get_bssid(bssid, ETH_ALEN) == 0)
	{
		wpa_printf(MSG_DEBUG, "calling wpa_driver_rtos_get_bssid(): "MACSTR, MAC2STR(bssid));
		return 0;
	}
	
	os_memset(bssid, 0, ETH_ALEN);
	return -1;
}

static int wpa_driver_rtos_get_ssid(void *priv, u8 *ssid)
{
//	struct wpa_driver_rtos_data *drv = priv;
	int len;

	if((len = rtos_get_ssid(ssid, 32)) <= 0){
		wpa_printf(MSG_DEBUG, "rtos_get_ssid failed...");
	}
	wpa_printf(MSG_DEBUG, "calling wpa_driver_rtos_get_ssid(): %s", ssid);
	return len;
}

static int wpa_driver_rtos_set_key(const char *ifname, void *priv,
				       enum wpa_alg alg,
				       const u8 *addr, int key_idx, int set_tx,
				       const u8 *seq, size_t seq_len,
				       const u8 *key, size_t key_len)
{
	wpa_printf(MSG_DEBUG, "calling wpa_driver_rtos_set_key()...");
	wpa_set_key_to_hal(alg, addr, key_idx, set_tx, seq, seq_len,
		       key, key_len);
	return 0;
}

int hal_send_msg_to_wpa_supplicant_driver(void *priv, char *cmd, int len){
	struct wpa_driver_rtos_data *drv = priv;
	int real_send_len;
	if(drv == NULL){
		wpa_printf(MSG_ERROR, "driver instance is not exist");
		return -2;
	}else{
		if((real_send_len = send(drv->sock, cmd, len, 0)) < 0){
			wpa_printf(MSG_ERROR, "hal_send_msg_to_wpa_supplicant_driver failed");
		}else{
			wpa_printf(MSG_DEBUG, "hal_send_msg_to_wpa_supplicant_driver OK");
		}
		return real_send_len;
	}
}

enum WLC_Enum{
	LC_JOIN_MSG = 0,
	LC_ASSOC_MSG,
	LC_DISASSOC_MSG,
	LC_PTK_MIC_MSG,
	LC_GTK_MIC_MSG,
};
struct drvmsg{
	int cmd;
	int len;
	char msg[8192];
};

static void wpa_driver_rtos_sock_receive(int sock, void *ctx,
					      void *sock_ctx)
{
	struct drvmsg buf;
	int left;
	union wpa_event_data data;
	u8 *resp_ies = NULL;

	//wpa_printf(MSG_DEBUG, "RECEIVE EVENT %0.8x", sock);
	memset(&buf, 0, sizeof(struct drvmsg));
	if ((left = recv(sock, &buf, sizeof(buf), 0)) < 0)
		return;
	//wpa_hexdump(MSG_DEBUG, "RECEIVE MSG", &buf, left);

	left = buf.len;
	os_memset(&data, 0, sizeof(data));

	switch (buf.cmd) {
	case LC_JOIN_MSG:
		trigger_to_connect();
		break;
	case LC_ASSOC_MSG:
		wpa_printf(MSG_DEBUG, "RTOS: ASSOC MESSAGE (left: %d)",
			   left);
		if (left > 0) {
			resp_ies = os_malloc(left);
			if (resp_ies == NULL)
				return;
			os_memcpy(resp_ies, buf.msg, left);
			data.assoc_info.resp_ies = resp_ies;
			data.assoc_info.resp_ies_len = left;
		}

		wpa_supplicant_event(ctx, EVENT_ASSOC, &data);
		os_free(resp_ies);
		break;
	case LC_DISASSOC_MSG:
		wpa_printf(MSG_DEBUG, "RTOS: DISASSOC MESSAGE");
		wpa_supplicant_event(ctx, EVENT_DISASSOC, NULL);
		break;
	case LC_PTK_MIC_MSG:
		wpa_printf(MSG_DEBUG, "RTOS: PTK MIC MSG MESSAGE");
		data.michael_mic_failure.unicast = 1;
		wpa_supplicant_event(ctx, EVENT_MICHAEL_MIC_FAILURE, &data);
		break;
	case LC_GTK_MIC_MSG:
		wpa_printf(MSG_DEBUG, "RTOS: GTK MIC MSG MESSAGE");
		data.michael_mic_failure.unicast = 0;
		wpa_supplicant_event(ctx, EVENT_MICHAEL_MIC_FAILURE, &data);
		break;
	default:
		wpa_printf(MSG_DEBUG, "RTOS: UNKNOWN MESSAGE");
		break;
	}
}

//static void wpa_driver_rtos_sock_event_cb(void *eloop_data, void *user_ctx)
//{
//	struct wpa_driver_rtos_data *drv = user_ctx;
//	struct wpa_supplicant *wpa_s = eloop_data;
//	wpa_printf(MSG_DEBUG, "calling wpa_driver_rtos_sock_event_cb()...");
//	wpa_driver_rtos_sock_receive(drv->sock, wpa_s, NULL);
//}

static void * wpa_driver_rtos_init(void *ctx, const char *ifname)
{
	struct wpa_driver_rtos_data *drv;

	drv = os_zalloc(sizeof(*drv));
	if (drv == NULL)
		return NULL;

	drv->ctx = ctx;
	os_strlcpy(drv->ifname, ifname, sizeof(drv->ifname));

	drv->sock = socket(PF_INET, SOCK_DGRAM, 0);
	if (drv->sock < 0)
		goto fail;

	if(eloop_register_read_sock(drv->sock, wpa_driver_rtos_sock_receive,
			drv->ctx, NULL) < 0){
		wpa_printf(MSG_ERROR, "failed to create eloop_register_read_sock...");
		close(drv->sock);
		goto fail;
	}else{
		wpa_printf(MSG_DEBUG, "success to create eloop_register_read_sock");
	}
//	if(eloop_register_event(&drv->event, sizeof(drv->event),
//			wpa_driver_rtos_sock_event_cb, drv->ctx, drv) < 0)
//	{
//		wpa_printf(MSG_ERROR, "failed to create drv->event...");
//		goto fail;
//	}
//	else
//	{
//		wpa_printf(MSG_DEBUG, "success to create drv->event: %0.8x", drv->event);
//	}

	wpa_printf(MSG_DEBUG, "calling wpa_driver_rtos_init()...:%s", drv->ifname);

	return drv;

fail:
	os_free(drv);
	return NULL;
}

static void wpa_driver_rtos_deinit(void *priv)
{
	struct wpa_driver_rtos_data *drv = priv;

//	eloop_unregister_event(&drv->event, sizeof(drv->event));
	eloop_unregister_read_sock(drv->sock);
	close(drv->sock);
	os_free(drv);
	wpa_printf(MSG_DEBUG, "calling wpa_driver_rtos_deinit()...");
}

static int wpa_driver_rtos_set_countermeasures(void *priv,
						   int enabled)
{
	wpa_printf(MSG_DEBUG, "calling wpa_driver_rtos_set_countermeasures()...:");
	return 0;
}

static void wpa_driver_rtos_scan_timeout(void *eloop_ctx,
					     void *timeout_ctx)
{
	wpa_printf(MSG_DEBUG, "Scan timeout - try to get results");
	wpa_supplicant_event(timeout_ctx, EVENT_SCAN_RESULTS, NULL);
}

static int wpa_driver_rtos_scan(void *priv,
					struct wpa_driver_scan_params *params)
{
	struct wpa_driver_rtos_data *drv = priv;
	wpa_printf(MSG_DEBUG, "calling wpa_driver_rtos_scan()...:");
	trigger_to_scan();

	eloop_cancel_timeout(wpa_driver_rtos_scan_timeout, drv, drv->ctx);
	eloop_register_timeout(3, 0, wpa_driver_rtos_scan_timeout, drv,
			       drv->ctx);
	return 0;
}

static struct wpa_scan_results * wpa_driver_rtos_get_scan_results(void *priv)
{
	wpa_printf(MSG_DEBUG, "calling wpa_driver_rtos_get_scan_results()...:");
	return get_scan_results(NULL, 0);
}

static int wpa_driver_rtos_deauthenticate(void *priv, const u8 *addr,
					      int reason_code)
{
	wpa_printf(MSG_DEBUG, "calling wpa_driver_rtos_deauthenticate()...:");
	trigger_to_disconnect();
	return 0;
}

//static int wpa_driver_rtos_disassociate(void *priv, const u8 *addr,
//					    int reason_code)
//{
//	wpa_printf(MSG_DEBUG, "calling wpa_driver_rtos_scan()...:");
//	trigger_to_disconnect();
//	return 0;
//}

static int wpa_driver_rtos_associate(void *priv,
			      struct wpa_driver_associate_params *params)
{
	wpa_printf(MSG_DEBUG, "calling wpa_driver_rtos_associate()...:");
	trigger_to_join();
	return 0;
}

int wpa_driver_rtos_set_operstate(void *priv, int state)
{
	struct wpa_driver_rtos_data *drv = priv;

	wpa_printf(MSG_DEBUG, "%s: operstate %d->%d (%s)",
		   __func__, drv->operstate, state, state ? "UP" : "DORMANT");
	drv->operstate = state;
	return trigger_to_start_DHCP(state);
}

static const u8 * wpa_driver_rtos_get_mac_addr(void *priv)
{
	struct wpa_driver_rtos_data *drv = priv;
	wpa_printf(MSG_DEBUG, "%s", __func__);
	get_mac_addr_from_hal(drv->own_addr);
	return (u8*)drv->own_addr;
}

const struct wpa_driver_ops wpa_driver_rtos_ops = {
	.name = "rtos",
	.desc = "rtos wireless driver",
	.get_bssid = wpa_driver_rtos_get_bssid,
	.get_ssid = wpa_driver_rtos_get_ssid,
	.set_key = wpa_driver_rtos_set_key,
	.init = wpa_driver_rtos_init,
	.deinit = wpa_driver_rtos_deinit,
	.set_countermeasures = wpa_driver_rtos_set_countermeasures,
	.scan2 = wpa_driver_rtos_scan,
	.get_scan_results2 = wpa_driver_rtos_get_scan_results,
	.deauthenticate = wpa_driver_rtos_deauthenticate,
	.associate = wpa_driver_rtos_associate,
	.set_operstate = wpa_driver_rtos_set_operstate,
	.get_mac_addr = wpa_driver_rtos_get_mac_addr,
};

