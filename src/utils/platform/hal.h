#ifndef HAL_H
#define HAL_H

#include "rtos.h"
#include "socket_rtos.h"

enum LC_Enum{
	LC_JOIN_MSG = 0,
	LC_ASSOC_MSG,
	LC_DEAUTH_MSG,
	LC_DISASSOC_MSG,
	LC_PTK_MIC_MSG,
	LC_GTK_MIC_MSG,
};
struct drvmsg{
	int cmd;
	int len;
	char msg[8192];
};

int hal_l2_send(int encrypt_enable, const u8 *dst_addr, u16 proto, const u8 *buf, size_t len);
int hal_get_ip_addr(struct sockaddr_in *saddr);

void trigger_to_scan(void);
void trigger_to_join(void);
void trigger_to_connect(void);
void trigger_to_disconnect(const unsigned char *addr, int reason_code);
int trigger_to_start_DHCP(int state);

int rtos_get_bssid(unsigned char *bssid, int len);
int rtos_get_ssid(unsigned char *ssid, int len);
struct wpa_scan_results * get_scan_results(unsigned char* ssid, int len);
void wpa_set_key_to_hal(int alg,
	       const unsigned char *addr, int key_idx, int set_tx,
	       const unsigned char *seq, size_t seq_len,
	       const unsigned char *key, size_t key_len);

void get_mac_addr_from_hal(char* mac_addr);

/**
 * ************************************************************************************************
 * TODO: implementation in wpa_supplicant
 */
struct network_param{
	char ifname[20];
	char ctrl_interface[20];
	char confname[20];
	char drivername[20];
	int daemonize;
	int wait_for_monitor;
	int wpa_debug_level;
	int wpa_debug_show_keys;
	int wpa_debug_timestamp;
};
int hal_send_msg_to_wpa_supplicant(char *ifname, char *msg, int len, int l2_or_driver);
int hal_send_msg_to_wpa_supplicant_driver(void *priv, char *cmd, int len);
int hal_send_msg_to_wpa_supplicant_l2_packet(void *ctx, char *msg, int len);

int TASK_WPA_SUPPLICANT(void *argv);

#endif
