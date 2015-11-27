commit 22de1db9c48a22fb06d98691075c8715b0b480ce
Author: lisen <lisen@bj.leadcoretech.com>
Date:   Wed Nov 25 16:44:12 2015 +0800

    (1) because platform.h has existed in src\utils, modify RTOS
    	platform.h --> platform_def.h in src\platform
    (2) compared with wpa_supplicant_2.4, statistical information for
    	modifying, maybe incomplete:
    	01>> src/common/wpa_ctrl.c
    	02>> src/crypto/tommath.h		(+)
    	03>> src/l2_packet/l2_packet_rtos.c	(+)
    	04>> src/l2_packet/l2_packet_rtos.h	(+)
    	05>> src/utils/common.h			(+)
    	06>> src/utils/eloop_rtos.c		(+)
    	07>> src/utils/include.h
    	08>> src/utils/os.h
    	09>> src/utils/os_rtos.c		(+)
    	10>> src/utils/socket_rtos.c		(+)
    	11>> src/utils/socket_rtos.h		(+)
    	12>> wpa_supplicant/cross.mk		(+)
    	13>> wpa_supplicant/ctrl_iface_named_pipe.c
    	14>> wpa_supplicant/ctrl_iface_rtos.c	(+)
    	15>> wpa_supplicant/main_rtos.c		(+)
    	16>> wpa_supplicant/Makefile
	(3) add <platform> directory for RTOS API