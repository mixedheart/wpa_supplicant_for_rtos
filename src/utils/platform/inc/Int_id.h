#ifndef	    INT_ID_H
#define     INT_ID_H
extern	void	GIC_IntCfg(unsigned	int IntID, unsigned	int IntCfg, unsigned int secCfg, unsigned int IntCPU0, unsigned int IntCPU1, unsigned int secFIQ);
//- 文件定义了LC1810 Cortex-A9各中断ID的编号。

#ifndef		__INT_ID__
#define		__INT_ID__

#define     IntID_CTL				32
#define     IntID_AP_DMAG			33
#define     IntID_AUDIO_DMAS		34
#define     IntID_AP_DMAS			35
#define     IntID_SECURITY			36
#define     IntID_KBS				37
#define     IntID_NA				38
#define     IntID_TRACKBALL			39
#define     IntID_WDT				40
#define     IntID_Timer0			41
#define     IntID_I2C0				42
#define     IntID_I2C1				43
#define     IntID_I2C2				44
#define     IntID_I2C3				45
#define     IntID_COM_I2C			46
#define     IntID_AP_PWR			47
#define     IntID_GPIO				48
#define     IntID_DSI				49
#define     IntID_HDMI				50
#define     IntID_GPU_GP			51
#define     IntID_GPU_PP0			52
#define     IntID_GPU_PP1			53
#define     IntID_GPU_GPMMU			54
#define     IntID_GPU_PP0MMU		55
#define     IntID_GPU_PP1MMU		56
#define     IntID_GPU_PMU			57
#define     IntID_I2S0				58
#define     IntID_I2S1				59
#define     IntID_SDMMC0			60
#define     IntID_SDMMC1			61
#define     IntID_SDMMC2			62
#define     IntID_SSI0				63
#define     IntID_SSI1				64
#define     IntID_SSI2				65
#define     IntID_COM_PCM			66
#define     IntID_UART0				67
#define     IntID_UART1				68
#define     IntID_UART2				69
#define     IntID_DDR_INT			70
#define     IntID_COM_UART			71
#define     IntID_SDIO3				72
#define     IntID_ON2_Decoder		73
#define     IntID_ON2_Encoder0		74
#define     IntID_ON2_Encoder1		75
#define     IntID_USB_OTG			76
#define     IntID_USB_HOST			77
#define     IntID_USB_HSIC			78
#define     IntID_LCDC				79
//#define     IntID_LCDC1				80
#define     IntID_NFC				81
#define     IntID_ISP				82
#define     IntID_CIPHER			83
//#define     IntID_COM_UART			84
#define     IntID_TPZCTL			85
#define     IntID_SSI3				86
#define     IntID_Timer1			87
#define     IntID_Timer2			88
#define     IntID_Timer3			89
#define     IntID_COM_PCM1			90
#define     IntID_DDR_PWR			91
#define     IntID_GPIO1				92
#define     IntID_a9_L2C_L2CCINTR	93
#define     IntID_CTL_SEC			94

#endif
#endif
