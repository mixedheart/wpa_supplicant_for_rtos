#ifndef	_SOCKET_RTOS_H_
#define	_SOCKET_RTOS_H_

#include "rtos.h"

typedef int socklen_t;

/* Types of sockets.  */
enum __socket_type
{
  SOCK_STREAM = 1,		/* Sequenced, reliable, connection-based
				   byte streams.  */
#define SOCK_STREAM SOCK_STREAM
  SOCK_DGRAM = 2,		/* Connectionless, unreliable datagrams
				   of fixed maximum length.  */
#define SOCK_DGRAM SOCK_DGRAM
  SOCK_RAW = 3,			/* Raw protocol interface.  */
#define SOCK_RAW SOCK_RAW
  SOCK_RDM = 4,			/* Reliably-delivered messages.  */
#define SOCK_RDM SOCK_RDM
  SOCK_SEQPACKET = 5,		/* Sequenced, reliable, connection-based,
				   datagrams of fixed maximum length.  */
#define SOCK_SEQPACKET SOCK_SEQPACKET
  SOCK_DCCP = 6,		/* Datagram Congestion Control Protocol.  */
#define SOCK_DCCP SOCK_DCCP
  SOCK_PACKET = 10,		/* Linux specific way of getting packets
				   at the dev level.  For writing rarp and
				   other similar things on the user level. */
#define SOCK_PACKET SOCK_PACKET

  /* Flags to be ORed into the type parameter of socket and socketpair and
     used for the flags parameter of paccept.  */

  SOCK_CLOEXEC = 02000000,	/* Atomically set close-on-exec flag for the
				   new descriptor(s).  */
#define SOCK_CLOEXEC SOCK_CLOEXEC
  SOCK_NONBLOCK = 04000		/* Atomically mark descriptor(s) as
				   non-blocking.  */
#define SOCK_NONBLOCK SOCK_NONBLOCK
};

/* Protocol families.  */
#define	PF_UNSPEC	0	/* Unspecified.  */
#define	PF_LOCAL	1	/* Local to host (pipes and file-domain).  */
#define	PF_UNIX		PF_LOCAL /* POSIX name for PF_LOCAL.  */
#define	PF_FILE		PF_LOCAL /* Another non-standard name for PF_LOCAL.  */
#define	PF_INET		2	/* IP protocol family.  */
#define	PF_INET6	10	/* IP version 6.  */
#define	PF_PACKET	17	/* Packet family.  */
#define PF_LLC		26	/* Linux LLC.  */

/* Address families.  */
#define	AF_UNSPEC	PF_UNSPEC
#define	AF_LOCAL	PF_LOCAL
#define	AF_UNIX		PF_UNIX
#define	AF_FILE		PF_FILE
#define	AF_INET		PF_INET
#define	AF_INET6	PF_INET6
#define	AF_PACKET	PF_PACKET
#define AF_LLC		PF_LLC

/* POSIX.1g specifies this type name for the `sa_family' member.  */
typedef unsigned short int sa_family_t;
/* Type to represent a port.  */
typedef uint16_t in_port_t;
/* Internet address.  */
typedef uint32_t in_addr_t;

struct in_addr
  {
    in_addr_t s_addr;
  };

/* Structure describing a generic socket address.  */
struct sockaddr
  {
	sa_family_t sa_family;	/* Common data: address family and length.  */
    char sa_data[14];		/* Address data.  */
  };

/* Structure describing an Internet socket address.  */
struct sockaddr_in
{
	sa_family_t sin_family;
    in_port_t sin_port;			/* Port number.  */
    struct in_addr sin_addr;		/* Internet address.  */

    /* Pad to size of `struct sockaddr'.  */
    unsigned char sin_zero[sizeof (struct sockaddr) -(sizeof (unsigned short int))
				-sizeof (in_port_t) - sizeof (struct in_addr)];
};

struct sockaddr_ll
{
    unsigned short int sll_family;
    unsigned short int sll_protocol;
    int sll_ifindex;
    unsigned short int sll_hatype;
    unsigned char sll_pkttype;
    unsigned char sll_halen;
    unsigned char sll_addr[8];
};

void rtos_socket_init(OS_EVENTGROUP *e);
int rtos_socket(int nf, int type, int protocol);
int rtos_sendto(int sock, char* buf, unsigned int len, int flags, void* to, void* tolen);
int rtos_recvfrom(int sock, void* buf, int len, unsigned int flags, void* from, void* fromlen);
int rtos_close(int sock);
#endif

