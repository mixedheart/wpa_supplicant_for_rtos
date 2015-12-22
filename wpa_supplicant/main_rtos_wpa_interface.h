#ifndef WPA_INTERFACE_H
#define WPA_INTERFACE_H

#include "common.h"
#define SHA1_MAC_LEN 20

struct l2_packet_data {
	int fd; /* packet socket for EAPOL frames */
	char ifname[IFNAMSIZ + 1];
	int ifindex;
	u8 own_addr[ETH_ALEN];
	void (*rx_callback)(void *ctx, const u8 *src_addr,
			    const u8 *buf, size_t len);
	void *rx_callback_ctx;
	int l2_hdr; /* whether to include layer 2 (Ethernet) header data
		     * buffers */

	/* For working around Linux packet socket behavior and regression. */
	int fd_br_rx;
	int last_from_br;
	u8 last_hash[SHA1_MAC_LEN];
	unsigned int num_rx, num_rx_br;
};

int TASK_WPA_SUPPLICANT(int argc, char *argv[]);

struct l2_packet_data*  get_wpa_l2_packet(void);
int get_wpa_supplicant_socket_fd(const char *ifname);

#endif

