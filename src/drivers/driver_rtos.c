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


static const int frequency_list[] = { 
	2412, 2417, 2422, 2427, 2432, 2437, 2442,
	2447, 2452, 2457, 2462, 2467, 2472, 2484 
};

struct bss_ie_hdr {
	u8 elem_id;
	u8 len;
	u8 oui[3];
	/* u8 oui_type; */
	/* u16 version; */
} __attribute__ ((packed));


struct wpa_driver_rtos_data {
	void *ctx;
	unsigned int event;
	int sock;
	char ifname[IFNAMSIZ + 1];
};


static int wpa_driver_rtos_get_bssid(void *priv, u8 *bssid)
{
	struct wpa_driver_rtos_data *drv = priv;
	wpa_printf(MSG_DEBUG, "calling wpa_driver_rtos_get_bssid()...");
//	if (broadcom_ioctl(drv, WLC_GET_BSSID, bssid, ETH_ALEN) == 0)
		return 0;
	
	os_memset(bssid, 0, ETH_ALEN);
	return -1;
}

static int wpa_driver_rtos_get_ssid(void *priv, u8 *ssid)
{
	struct wpa_driver_rtos_data *drv = priv;
	wpa_printf(MSG_DEBUG, "calling wpa_driver_rtos_get_ss()...");
//	wlc_ssid_t s;id
//
//	if (broadcom_ioctl(drv, WLC_GET_SSID, &s, sizeof(s)) == -1)
//		return -1;
//
//	os_memcpy(ssid, s.SSID, s.SSID_len);
//	return s.SSID_len;
	return 0;
}

static int wpa_driver_rtos_set_key(const char *ifname, void *priv,
				       enum wpa_alg alg,
				       const u8 *addr, int key_idx, int set_tx,
				       const u8 *seq, size_t seq_len,
				       const u8 *key, size_t key_len)
{
	wpa_printf(MSG_DEBUG, "calling wpa_driver_rtos_set_key()...");
	return 0;
}

enum WLC_Enum{
	WLC_ASSOC_MSG = (0x01UL<<0),
	WLC_DISASSOC_MSG = (0x01UL<<1),
	WLC_PTK_MIC_MSG = (0x01UL<<2),
	WLC_GTK_MIC_MSG = (0x01UL<<3),
};
static void wpa_driver_rtos_event_receive(int sock, void *ctx,
					      void *sock_ctx)
{
	char buf[8192];
	unsigned int real_event;
	int left;
	union wpa_event_data data;
	u8 *resp_ies = NULL;

	wpa_printf(MSG_DEBUG, "RECEIVE EVENT %0.8x", sock);
	if ((left = recv(sock, buf, sizeof(buf), 0)) < 0)
		return;

	wpa_hexdump(MSG_DEBUG, "RECEIVE EVENT", (u8 *) buf, left);

	os_memset(&data, 0, sizeof(data));

	switch (real_event) {
	case WLC_ASSOC_MSG:
		wpa_printf(MSG_DEBUG, "BROADCOM: ASSOC MESSAGE (left: %d)",
			   left);
		if (left > 0) {
			resp_ies = os_malloc(left);
			if (resp_ies == NULL)
				return;
			data.assoc_info.resp_ies = resp_ies;
			data.assoc_info.resp_ies_len = left;
		}

		wpa_supplicant_event(ctx, EVENT_ASSOC, &data);
		os_free(resp_ies);
		break;
	case WLC_DISASSOC_MSG:
		wpa_printf(MSG_DEBUG, "RTOS: DISASSOC MESSAGE");
		wpa_supplicant_event(ctx, EVENT_DISASSOC, NULL);
		break;
	case WLC_PTK_MIC_MSG:
		wpa_printf(MSG_DEBUG, "RTOS: PTK MIC MSG MESSAGE");
		data.michael_mic_failure.unicast = 1;
		wpa_supplicant_event(ctx, EVENT_MICHAEL_MIC_FAILURE, &data);
		break;
	case WLC_GTK_MIC_MSG:
		wpa_printf(MSG_DEBUG, "RTOS: GTK MIC MSG MESSAGE");
		data.michael_mic_failure.unicast = 0;
		wpa_supplicant_event(ctx, EVENT_MICHAEL_MIC_FAILURE, &data);
		break;
	default:
		wpa_printf(MSG_DEBUG, "RTOS: UNKNOWN MESSAGE");
		break;
	}
}

static void * wpa_driver_rtos_init(void *ctx, const char *ifname)
{
	struct wpa_driver_rtos_data *drv;

	drv = os_zalloc(sizeof(*drv));
	if (drv == NULL)
		return NULL;

	drv->sock = socket(PF_INET, SOCK_DGRAM, 0);
	if (drv->sock < 0)
		goto fail;

	if(eloop_register_event(&drv->event, sizeof(drv->event),
			NULL, ctx, NULL) < 0)
	{
		wpa_printf(MSG_ERROR, "failed to create drv->event...");
		goto fail;
	}
	else
	{
		wpa_printf(MSG_DEBUG, "success to create drv->event: %0.8x", drv->event);
	}

	drv->ctx = ctx;
	os_strlcpy(drv->ifname, ifname, sizeof(drv->ifname));

	wpa_printf(MSG_DEBUG, "calling wpa_driver_rtos_init()...:%s", drv->ifname);
	return drv;

fail:
	os_free(drv);
	return NULL;
}

static void wpa_driver_rtos_deinit(void *priv)
{
	struct wpa_driver_rtos_data *drv = priv;

	eloop_unregister_event(&drv->event, sizeof(drv->event));

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
};

