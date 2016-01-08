#ifndef HAL_H
#define HAL_H

#include "rtos.h"
#include "socket_rtos.h"

int hal_l2_send(int encrypt_enable, const u8 *dst_addr, u16 proto, const u8 *buf, size_t len);
int hal_get_ip_addr(struct sockaddr_in *saddr);

void trigger_to_scan(void);
void trigger_to_join(void);
void trigger_to_connect(void);
void trigger_to_disconnect(void);
int trigger_to_start_DHCP(int state);

int rtos_get_bssid(unsigned char *bssid, int len);
int rtos_get_ssid(unsigned char *ssid, int len);
struct wpa_scan_results * get_scan_results(unsigned char* ssid, int len);
void wpa_set_key_to_hal(int alg,
	       const unsigned char *addr, int key_idx, int set_tx,
	       const unsigned char *seq, size_t seq_len,
	       const unsigned char *key, size_t key_len);


/**
 * ************************************************************************************************
 * TODO: implementation in wpa_supplicant
 */
int hal_send_msg_to_wpa_supplicant(char *ifname, char *msg, int len, int l2_or_driver);
int hal_send_msg_to_wpa_supplicant_driver(void *priv, char *cmd, int len);
int hal_send_msg_to_wpa_supplicant_l2_packet(void *ctx, char *msg, int len);

int TASK_WPA_SUPPLICANT(int argc, char *argv[]);

#endif
