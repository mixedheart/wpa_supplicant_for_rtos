/*
 * WPA Supplicant - Layer2 packet handling with Linux packet sockets
 * Copyright (c) 2003-2015, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "includes.h"
#include "common.h"
#include "eloop.h"
#include "crypto/sha1.h"
#include "crypto/crypto.h"
#include "l2_packet.h"

struct l2_packet_data {
	int encrypt_enable;
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

#define inet_ntoa(x) wpa_supplicant_inet_ntoa(x)
char *wpa_supplicant_inet_ntoa (struct in_addr __in)
{
#define BUFLEN_INET_NTOA	(16)
	static char str[BUFLEN_INET_NTOA];
	uint32_t s_addr = __in.s_addr;
	char inv[3];
	char *rp;
	uint8_t *ap;
	uint8_t rem;
	uint8_t n;
	uint8_t i;
	int len = 0;

	rp = str;
	ap = (uint8_t *) &s_addr;
	for (n = 0; n < 4; n++) {
		i = 0;
		do {
			rem = *ap % (uint8_t) 10;
			*ap /= (uint8_t) 10;
			inv[i++] = '0' + rem;
		} while (*ap);
		while (i--) {
			if (len++ >= BUFLEN_INET_NTOA) {
				return NULL;
			}
			*rp++ = inv[i];
		}
		if (len++ >= BUFLEN_INET_NTOA) {
			return NULL;
		}
		*rp++ = '.';
		ap++;
	}
	*--rp = 0;
	return str;
}

#if 0
/* Generated by 'sudo tcpdump -s 3000 -dd greater 278 and ip and udp and
 * src port bootps and dst port bootpc'
 */
static struct sock_filter dhcp_sock_filter_insns[] = {
	{ 0x80, 0, 0, 0x00000000 },
	{ 0x35, 0, 12, 0x00000116 },
	{ 0x28, 0, 0, 0x0000000c },
	{ 0x15, 0, 10, 0x00000800 },
	{ 0x30, 0, 0, 0x00000017 },
	{ 0x15, 0, 8, 0x00000011 },
	{ 0x28, 0, 0, 0x00000014 },
	{ 0x45, 6, 0, 0x00001fff },
	{ 0xb1, 0, 0, 0x0000000e },
	{ 0x48, 0, 0, 0x0000000e },
	{ 0x15, 0, 3, 0x00000043 },
	{ 0x48, 0, 0, 0x00000010 },
	{ 0x15, 0, 1, 0x00000044 },
	{ 0x6, 0, 0, 0x00000bb8 },
	{ 0x6, 0, 0, 0x00000000 },
};

static const struct sock_fprog dhcp_sock_filter = {
	.len = ARRAY_SIZE(dhcp_sock_filter_insns),
	.filter = dhcp_sock_filter_insns,
};


/* Generated by 'sudo tcpdump -dd -s 1500 multicast and ip6[6]=58' */
static struct sock_filter ndisc_sock_filter_insns[] = {
	{ 0x30, 0, 0, 0x00000000 },
	{ 0x45, 0, 5, 0x00000001 },
	{ 0x28, 0, 0, 0x0000000c },
	{ 0x15, 0, 3, 0x000086dd },
	{ 0x30, 0, 0, 0x00000014 },
	{ 0x15, 0, 1, 0x0000003a },
	{ 0x6, 0, 0, 0x000005dc },
	{ 0x6, 0, 0, 0x00000000 },
};

static const struct sock_fprog ndisc_sock_filter = {
	.len = ARRAY_SIZE(ndisc_sock_filter_insns),
	.filter = ndisc_sock_filter_insns,
};
#endif

int l2_packet_get_own_addr(struct l2_packet_data *l2, u8 *addr)
{
	os_memcpy(addr, l2->own_addr, ETH_ALEN);
	return 0;
}

/* TODO: send data directly. */
int l2_packet_send(struct l2_packet_data *l2, const u8 *dst_addr, u16 proto,
		   const u8 *buf, size_t len)
{
	int ret;
	if (l2 == NULL)
		return -1;

	if (l2->l2_hdr) {
#if 0
		ret = send(l2->fd, buf, len, 0);
		if (ret < 0)
			wpa_printf(MSG_ERROR, "l2_packet_send - send: %s",
				   strerror(errno));
#endif
	} else {
		wpa_printf(MSG_DEBUG, "l2_packet_send - sendto hal:%d", l2->encrypt_enable);
		ret = hal_l2_send(l2->encrypt_enable, dst_addr, proto, buf, len);
		if (ret < 0) {
			wpa_printf(MSG_ERROR, "l2_packet_send - sendto: %s",
				   strerror(errno));
		}
	}
	return ret;
}

int hal_send_msg_to_wpa_supplicant_l2_packet(void *ctx, char *msg, int len){
	struct l2_packet_data *l2 = ctx;
	int real_send_len;
	if(l2 == NULL){
		wpa_printf(MSG_ERROR, "l2 instance is not exist");
		return -2;
	}else{
		if((real_send_len = send(l2->fd, msg, len, 0)) < 0){
			wpa_printf(MSG_ERROR, "hal_send_msg_to_l2_packet failed");
		}else{
			wpa_printf(MSG_DEBUG, "hal_send_msg_to_l2_packet OK");
		}
		return real_send_len;
	}
}

static void l2_packet_receive(int sock, void *eloop_ctx, void *sock_ctx)
{
	struct l2_packet_data *l2 = eloop_ctx;
	u8 buf[2300];
	int res, pos;
	struct sockaddr_ll ll;

	l2->num_rx++;
	os_memset(&ll, 0, sizeof(ll));
	res = recvfrom(sock, buf, sizeof(buf), 0, NULL, NULL);
	if (res < 0) {
		wpa_printf(MSG_DEBUG, "l2_packet_receive - recvfrom: %s",
			   strerror(errno));
		return;
	}else{
		pos = 6;
		memcpy(ll.sll_addr, buf+6, 8);
	}

	//wpa_hexdump(MSG_DEBUG, "EAPOL:", buf, res);

	wpa_printf(MSG_DEBUG, "%s: src=" MACSTR " len=%d",
		   __func__, MAC2STR(ll.sll_addr), (int) res);

	if (l2->fd_br_rx >= 0) {
		u8 hash[SHA1_MAC_LEN];
		const u8 *addr[1];
		size_t len[1];

		/*
		 * Close the workaround socket if the kernel version seems to be
		 * able to deliver packets through the packet socket before
		 * authorization has been completed (in dormant state).
		 */
		if (l2->num_rx_br <= 1) {
			wpa_printf(MSG_DEBUG,
				   "l2_packet_receive: Main packet socket for %s seems to have working RX - close workaround bridge socket",
				   l2->ifname);
			eloop_unregister_read_sock(l2->fd_br_rx);
			close(l2->fd_br_rx);
			l2->fd_br_rx = -1;
		}

		addr[0] = buf;
		len[0] = res;
		sha1_vector(1, addr, len, hash);
		if (l2->last_from_br &&
		    os_memcmp(hash, l2->last_hash, SHA1_MAC_LEN) == 0) {
			wpa_printf(MSG_DEBUG, "%s: Drop duplicate RX",
				   __func__);
			return;
		}
		os_memcpy(l2->last_hash, hash, SHA1_MAC_LEN);
	}

	l2->last_from_br = 0;
	pos = 14;
	l2->rx_callback(l2->rx_callback_ctx, ll.sll_addr, buf+pos, res - pos);
}


static void l2_packet_receive_br(int sock, void *eloop_ctx, void *sock_ctx)
{
	struct l2_packet_data *l2 = eloop_ctx;
	u8 buf[2300];
	int res;
	struct sockaddr_ll ll;
	u8 hash[SHA1_MAC_LEN];
	const u8 *addr[1];
	size_t len[1];

	l2->num_rx_br++;
	res = recvfrom(sock, buf, sizeof(buf), 0, NULL, NULL);
	if (res < 0) {
		wpa_printf(MSG_DEBUG, "l2_packet_receive_br - recvfrom: %s",
			   strerror(errno));
		return;
	}

	wpa_printf(MSG_DEBUG, "%s: src=" MACSTR " len=%d",
		   __func__, MAC2STR(ll.sll_addr), (int) res);

	addr[0] = buf;
	len[0] = res;
	sha1_vector(1, addr, len, hash);
	if (!l2->last_from_br &&
	    os_memcmp(hash, l2->last_hash, SHA1_MAC_LEN) == 0) {
		wpa_printf(MSG_DEBUG, "%s: Drop duplicate RX", __func__);
		return;
	}
	l2->last_from_br = 1;
	os_memcpy(l2->last_hash, hash, SHA1_MAC_LEN);
	l2->rx_callback(l2->rx_callback_ctx, ll.sll_addr, buf, res);
}


struct l2_packet_data * l2_packet_init(
	const char *ifname, const u8 *own_addr, unsigned short protocol,
	void (*rx_callback)(void *ctx, const u8 *src_addr,
			    const u8 *buf, size_t len),
	void *rx_callback_ctx, int l2_hdr)
{
	struct l2_packet_data *l2;
//	struct ifreq ifr;
//	const u8 local_addr[ETH_ALEN] = {0xF4, 0x95, 0x55, 0x23, 0x03, 0x66};

	l2 = os_zalloc(sizeof(struct l2_packet_data));
	if (l2 == NULL)
		return NULL;
	os_strlcpy(l2->ifname, ifname, sizeof(l2->ifname));
	l2->rx_callback = rx_callback;
	l2->rx_callback_ctx = rx_callback_ctx;
	l2->l2_hdr = l2_hdr;
	l2->fd_br_rx = -1;

	l2->fd = socket(PF_PACKET, l2_hdr ? SOCK_RAW : SOCK_DGRAM,
			htons(protocol));
	if (l2->fd < 0) {
		wpa_printf(MSG_ERROR, "%s: socket(PF_PACKET): %s",
			   __func__, strerror(errno));
		os_free(l2);
		return NULL;
	}
//	os_memset(&ifr, 0, sizeof(ifr));
//	os_strlcpy(ifr.ifr_name, l2->ifname, sizeof(ifr.ifr_name));
#if 0
	if (ioctl(l2->fd, SIOCGIFINDEX, &ifr) < 0) {
		wpa_printf(MSG_ERROR, "%s: ioctl[SIOCGIFINDEX]: %s",
			   __func__, strerror(errno));
		close(l2->fd);
		os_free(l2);
		return NULL;
	}
	l2->ifindex = ifr.ifr_ifindex;

	os_memset(&ll, 0, sizeof(ll));
	ll.sll_family = PF_PACKET;
	ll.sll_ifindex = ifr.ifr_ifindex;
	ll.sll_protocol = htons(protocol);
	if (bind(l2->fd, (struct sockaddr *) &ll, sizeof(ll)) < 0) {
		wpa_printf(MSG_ERROR, "%s: bind[PF_PACKET]: %s",
			   __func__, strerror(errno));
		close(l2->fd);
		os_free(l2);
		return NULL;
	}

	if (ioctl(l2->fd, SIOCGIFHWADDR, &ifr) < 0) {
		wpa_printf(MSG_ERROR, "%s: ioctl[SIOCGIFHWADDR]: %s",
			   __func__, strerror(errno));
		close(l2->fd);
		os_free(l2);
		return NULL;
	}
#endif
	/**
	 * TODO Attention: own_addr should be checked, i don't know what's the own_addr value
	 */
	os_memcpy(l2->own_addr, own_addr, ETH_ALEN);
	wpa_hexdump(MSG_DEBUG, "mac_addr", l2->own_addr, ETH_ALEN);
	//os_memcpy(l2->own_addr, local_addr, ETH_ALEN);

	eloop_register_read_sock(l2->fd, l2_packet_receive, l2, NULL);

	return l2;
}


struct l2_packet_data * l2_packet_init_bridge(
	const char *br_ifname, const char *ifname, const u8 *own_addr,
	unsigned short protocol,
	void (*rx_callback)(void *ctx, const u8 *src_addr,
			    const u8 *buf, size_t len),
	void *rx_callback_ctx, int l2_hdr)
{
	struct l2_packet_data *l2;

	l2 = l2_packet_init(br_ifname, own_addr, protocol, rx_callback,
			    rx_callback_ctx, l2_hdr);
	if (!l2)
		return NULL;

	/*
	 * The Linux packet socket behavior has changed over the years and there
	 * is an inconvenient regression in it that breaks RX for a specific
	 * protocol from interfaces in a bridge when that interface is not in
	 * fully operation state (i.e., when in station mode and not completed
	 * authorization). To work around this, register ETH_P_ALL version of
	 * the packet socket bound to the real netdev and use socket filter to
	 * match the ethertype value. This version is less efficient, but
	 * required for functionality with many kernel version. If the main
	 * packet socket is found to be working, this less efficient version
	 * gets closed automatically.
	 */

	l2->fd_br_rx = socket(PF_PACKET, l2_hdr ? SOCK_RAW : SOCK_DGRAM,
			      htons(ETH_P_ALL));
	if (l2->fd_br_rx < 0) {
		wpa_printf(MSG_DEBUG, "%s: socket(PF_PACKET-fd_br_rx): %s",
			   __func__, strerror(errno));
		/* try to continue without the workaround RX socket */
		return l2;
	}
#if 0
	os_memset(&ll, 0, sizeof(ll));
	ll.sll_family = PF_PACKET;
	ll.sll_ifindex = if_nametoindex(ifname);
	ll.sll_protocol = htons(ETH_P_ALL);
	if (bind(l2->fd_br_rx, (struct sockaddr *) &ll, sizeof(ll)) < 0) {
		wpa_printf(MSG_DEBUG, "%s: bind[PF_PACKET-fd_br_rx]: %s",
			   __func__, strerror(errno));
		/* try to continue without the workaround RX socket */
		close(l2->fd_br_rx);
		l2->fd_br_rx = -1;
		return l2;
	}

	if (setsockopt(l2->fd_br_rx, SOL_SOCKET, SO_ATTACH_FILTER,
		       &ethertype_sock_filter, sizeof(struct sock_fprog))) {
		wpa_printf(MSG_DEBUG,
			   "l2_packet_linux: setsockopt(SO_ATTACH_FILTER) failed: %s",
			   strerror(errno));
		/* try to continue without the workaround RX socket */
		close(l2->fd_br_rx);
		l2->fd_br_rx = -1;
		return l2;
	}
#endif
	eloop_register_read_sock(l2->fd_br_rx, l2_packet_receive_br, l2, NULL);

	return l2;
}


void l2_packet_deinit(struct l2_packet_data *l2)
{
	if (l2 == NULL)
		return;

	if (l2->fd >= 0) {
		eloop_unregister_read_sock(l2->fd);
		close(l2->fd);
	}

	if (l2->fd_br_rx >= 0) {
		eloop_unregister_read_sock(l2->fd_br_rx);
		close(l2->fd_br_rx);
	}

	os_free(l2);
}


int l2_packet_get_ip_addr(struct l2_packet_data *l2, char *buf, size_t len)
{
//	int s;
//	struct ifreq ifr;
	struct sockaddr_in saddr;
	size_t res;

//	s = socket(PF_INET, SOCK_DGRAM, 0);
//	if (s < 0) {
//		wpa_printf(MSG_ERROR, "%s: socket: %s",
//			   __func__, strerror(errno));
//		return -1;
//	}
//	os_memset(&ifr, 0, sizeof(ifr));
//	os_strlcpy(ifr.ifr_name, l2->ifname, sizeof(ifr.ifr_name));
//	if (ioctl(s, SIOCGIFADDR, &ifr) < 0) {
//		if (errno != EADDRNOTAVAIL)
//			wpa_printf(MSG_ERROR, "%s: ioctl[SIOCGIFADDR]: %s",
//				   __func__, strerror(errno));
//		close(s);
//		return -1;
//	}
//	close(s);
//	saddr = aliasing_hide_typecast(&ifr.ifr_addr, struct sockaddr_in);
//
	/**
	 * TODO
	 * from hal sock to get ip addr
	 */
	hal_get_ip_addr(&saddr);
	if (saddr.sin_family != AF_INET)
		return -1;
	res = os_strlcpy(buf, inet_ntoa(saddr.sin_addr), len);
	if (res >= len)
		return -1;
	return 0;
}


void l2_packet_notify_auth_start(struct l2_packet_data *l2)
{
}

int l2_packet_set_packet_filter(struct l2_packet_data *l2,
				enum l2_packet_filter_type type)
{
#if 0
	const struct sock_fprog *sock_filter;

	switch (type) {
	case L2_PACKET_FILTER_DHCP:
		sock_filter = &dhcp_sock_filter;
		break;
	case L2_PACKET_FILTER_NDISC:
		sock_filter = &ndisc_sock_filter;
		break;
	default:
		return -1;
	}

	if (setsockopt(l2->fd, SOL_SOCKET, SO_ATTACH_FILTER,
		       sock_filter, sizeof(struct sock_fprog))) {
		wpa_printf(MSG_ERROR,
			   "l2_packet_linux: setsockopt(SO_ATTACH_FILTER) failed: %s",
			   strerror(errno));
		return -1;
	}

	return 0;
#else
	return 0;
#endif
}

