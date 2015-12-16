/*
 * WPA Supplicant / Example program entrypoint
 * Copyright (c) 2003-2005, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "includes.h"

#include "common.h"
#include "wpa_supplicant_i.h"
#include "l2_packet/l2_packet.h"
#include "main_rtos_wpa_interface.h"
#include "wpa_supplicant_cli.h"

FIL	wpa_std_inouterr;

int wpa_supplicant_rename(const char * old, const char *new)
{
	return 0;
}

struct wpa_global *global;

static OS_QUEUE* Wpa_Global;

int TASK_WPA_SUPPLICANT(int argc, char *argv[])
{
	struct wpa_interface iface;
	int exitcode = 0;
	struct wpa_params params;
//	struct wpa_global *global;

	memset(&params, 0, sizeof(params));
	params.daemonize = 0;
	params.wait_for_monitor = 0;
	params.wpa_debug_level = MSG_DEBUG;
	//params.ctrl_interface = "Global";
	params.wpa_debug_show_keys = 1;
	params.wpa_debug_timestamp = 1;

	global = wpa_supplicant_init(&params);
	if (global == NULL)
		return -1;

	memset(&iface, 0, sizeof(iface));
	iface.ifname = "leadcore";
	iface.ctrl_interface = "wlan0";
	/* TODO: set interface parameters */

	if (wpa_supplicant_add_iface(global, &iface, NULL) == NULL)
	{
		wpa_printf(MSG_DEBUG, "failed to add interface ....\n");
		exitcode = -1;
	}

#if 0
	OS_Send_To_Queue(Wpa_Global, &global, 1, OS_SUSPEND_NO_TIMEOUT, NULL);
#endif
	wpa_supplicant_cli_init();

	if (exitcode == 0)
		exitcode = wpa_supplicant_run(global);
	wpa_printf(MSG_DEBUG, "wpa_supplicant_run exit, exitcode=%d....\n", exitcode);

	wpa_supplicant_deinit(global);
	wpa_supplicant_cli_deinit();

	//delete self
	//OS_Delete_Task(NULL, NULL);
	return exitcode;
}
/**
 * should be called before creating task: TASK_WPA_SUPPLICANT
 */
OS_STATUS Create_Wpa_Gloabl_Queue(void)
{
#if 0
	OS_STATUS ret = OS_Create_Queue(&Wpa_Global, "Get_WPA", 4, 1, NULL);
	if(ret != 0)
	{
		printf("create Get_Wpa_Global failed....\n");
	}
	return ret;
#else
	return 0;
#endif
}

OS_STATUS Delete_Wpa_Gloabl_Queue(void)
{
#if 0
	OS_Delete_Queue(Wpa_Global, NULL);
	return OS_SUCCESS;
#else
	return OS_SUCCESS;
#endif
}

struct l2_packet_data* get_wpa_l2_packet(void)
{
#if 0
	void *msg;
	uint32_t actual_size;
	OS_Receive_From_Queue(Wpa_Global, &msg, 1, &actual_size, OS_SUSPEND_NO_TIMEOUT, NULL);
	return ((struct wpa_global*)msg)->ifaces->l2;
#else
	return global->ifaces->l2;
#endif
}
