#ifndef __MIC_OPTS_H__
#define __MIC_OPTS_H__

//#define FONT_LIB_SUPPORT

#define		DDR_RAM_START		0x40000000
#define		DDR_RAM_LENGTH		0x1000000

#define SYS_THREAD_STACK			(volatile uint32 *)0x4000000
#define SYS_THREAD__STACK_LENTH		(0x4100000 - 0x4000000) //
#define TCPIP_THREAD_STACKSIZE  	SYS_THREAD__STACK_LENTH
/*****************************************************************/
/* Task Priority Define begin */
/*
 * decription of priority defination
 * IO USE HIGHEST PRIOTITY for BH(SDIO) and WS(SD)
 * HIGH CPU LOAD TASK USE LESS THAN BASE+5 for TCP, DISPATCH
 * LESS USED TASK USE BASE+5--BASE+15 for MLME, AUTH, LWIP, DHCPD...
 * DAMEON USE AFTER BASE+15, for IDLE use BASE+20
 *
 * */

#ifndef THREAD_BASE_PRIO
#define THREAD_BASE_PRIO			7
#endif

#ifndef SD_THREAD_PRIO
#define SD_THREAD_PRIO				(THREAD_BASE_PRIO + 0)
#endif

#ifndef BH_THREAD_PRIO
#define BH_THREAD_PRIO				(THREAD_BASE_PRIO + 0)
#endif

#ifndef TXRX_THREAD_PRIO
#define TXRX_THREAD_PRIO			(THREAD_BASE_PRIO + 1)
#endif

#ifndef PM_THREAD_PRIO
#define PM_THREAD_PRIO				(THREAD_BASE_PRIO + 1)
#endif

#ifndef PROXY_THREAD_PRIO
#define PROXY_THREAD_PRIO			(THREAD_BASE_PRIO + 5)
#endif

#ifndef WHBOX_TEST_THREAD_PRIO
#define WHBOX_TEST_THREAD_PRIO		(THREAD_BASE_PRIO + 5)
#endif

#ifndef TCPIP_THREAD_PRIO
#define TCPIP_THREAD_PRIO			(THREAD_BASE_PRIO + 2)
#endif

#ifndef LWIP_THREAD_PRIO
#define LWIP_THREAD_PRIO			(THREAD_BASE_PRIO + 1)
#endif

#ifndef AUTH_THREAD_PRIO
#define AUTH_THREAD_PRIO			(THREAD_BASE_PRIO + 5)
#endif

#ifndef HTC_THREAD_PRIO
#define HTC_THREAD_PRIO				(THREAD_BASE_PRIO + 3)
#endif

#ifndef MLME_THREAD_PRIO
#define MLME_THREAD_PRIO			(THREAD_BASE_PRIO + 5)
#endif

#ifndef LWIPD_THREAD_PRIO
#define LWIPD_THREAD_PRIO			(THREAD_BASE_PRIO + 6)
#endif

#ifndef WS_RECV_THREAD_PRIO
#define WS_RECV_THREAD_PRIO         (THREAD_BASE_PRIO + 8) // 7
#endif

#ifndef WS_WORK_THREAD_PRIO
#define WS_WORK_THREAD_PRIO         (THREAD_BASE_PRIO + 0)//9
#endif

#ifndef WS_DOWNLOAD_THREAD_PRIO
#define WS_DOWNLOAD_THREAD_PRIO     (THREAD_BASE_PRIO + 11)//9
#endif

#ifndef DHCPSERVER_THREAD_PRIO
#define DHCPSERVER_THREAD_PRIO      (THREAD_BASE_PRIO + 7)
#endif

#ifndef DNSSERVER_THREAD_PRIO
#define DNSSERVER_THREAD_PRIO       (THREAD_BASE_PRIO + 7)
#endif

#ifndef FS_UPDATE_THREAD_PRIO
#define FS_UPDATE_THREAD_PRIO      	(THREAD_BASE_PRIO + 6)
#endif

#ifndef SM_THREAD_PRIO
#define SM_THREAD_PRIO	         	(THREAD_BASE_PRIO + 12)
#endif

#ifndef DEAMON_THREAD_PRIO
#define DEAMON_THREAD_PRIO         	(THREAD_BASE_PRIO + 13)
#endif
/* Task Priority Define end */

/* Task Stack Define begin */

#ifndef TCPIP_THREAD_STACKSIZE
#define TCPIP_THREAD_STACKSIZE  (1024 * 4)      //15000
#endif

#ifndef LWIP_STACK_SIZE
#define LWIP_STACK_SIZE			0x4000
#endif

#ifndef DHCPSERVER_STACK_SIZE
#define DHCPSERVER_STACK_SIZE	3000
#endif

#ifndef DNSSERVER_STACK_SIZE
#define DNSSERVER_STACK_SIZE	2000
#endif


/* Task Stack Define end */

/* Function define begin */

/* Net function define */
#define MIC_LWIP				1
#define LWIP_TEST				0

/* For BIN update DEMO */
#define BIN_UPDATE_DEMO			1

/* White Box Test */
#define MIC_WHITEBOX_TEST		1
#define MIC_WHITEBOX_LWIP_SEND	1

#define MIC_WEBSERV                     (1 & MIC_LWIP)

#define MIC_DHCPSERVER                  (1 & MIC_LWIP)

#define MIC_DNS							(1 & MIC_LWIP)

#define MIC_LHTTPD                  	(1 & MIC_LWIP)

#define MIC_DNSSERVER                   (1 & MIC_LWIP)

#define AUTO_PUSH                       (0 & MIC_WEBSERV)

/* svc_host define */
#define MIC_HOST				1

/* svc_Proxy define */
#define MIC_PROXY				1

/* SD function define */
#define MIC_SD					1
#define MIC_APDU				(1 & MIC_SD)
#define MIC_PIT2_ENABLE			1

#ifndef MIC_SD
#define MIC_SD					0
#endif

#ifndef MIC_KISS
#define MIC_KISS				MIC_SD
#endif

#ifndef MIC_FTL
#define MIC_FTL					MIC_SD
#endif

#ifndef MIC_APDU
#define MIC_APDU				(1 &MIC_SD)
#endif

#ifndef MIC_LWIP
#define MIC_LWIP				1
#endif

#ifndef LWIP_TEST
#define LWIP_TEST				(1 & MIC_LWIP)
#endif

#ifndef MIC_WHITEBOX_TEST
#define MIC_WHITEBOX_TEST		0
#endif

#ifndef MIC_PIT2_ENABLE
#define MIC_PIT2_ENABLE			0
#endif

/*
#if MIC_APDU
#undef MIC_APDU
#define MIC_APDU				(1 & MIC_SD)
#endif
*/

/* Function define end */

#endif /* __TASK_OPTS_H__*/
