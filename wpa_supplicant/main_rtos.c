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
//#include "main_rtos_wpa_interface.h"
#include "wpa_supplicant_cli.h"

FIL	wpa_std_inouterr;

int wpa_supplicant_rename(const char * old, const char *new)
{
	return 0;
}

struct wpa_global *global;
struct wpa_supplicant *wpa_s_const = NULL;

int TASK_WPA_SUPPLICANT(int argc, char *argv[])
{
	struct wpa_interface iface;
	int exitcode = 0;
	struct wpa_params params;
//	struct wpa_global *global;
	wpa_s_const = NULL;

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
	iface.ifname = "wlan0";
	iface.ctrl_interface = "ctrl_wlan0";
	iface.confname = "wpa_s.txt";
	iface.driver = "rtos";

	/* TODO: set interface parameters */

	if (wpa_supplicant_add_iface(global, &iface, NULL) == NULL)
	{
		wpa_printf(MSG_DEBUG, "failed to add interface ....\n");
		exitcode = -1;
	}

	//wpa_supplicant_cli_init();

	if (exitcode == 0)
		exitcode = wpa_supplicant_run(global);
	wpa_printf(MSG_DEBUG, "wpa_supplicant_run exit, exitcode=%d....\n", exitcode);

	wpa_supplicant_deinit(global);
	//wpa_supplicant_cli_deinit();

	//delete self
	//OS_Delete_Task(NULL, NULL);
	return exitcode;
}

/**
 * try to protect wpa_supplicant integrity
 */
void* get_wpa_supplicant_interface(char *ifname, int waiting_sec, int waiting_usec){
	struct wpa_supplicant *wpa_s;
	char local_ifname[sizeof(wpa_s->ifname)];
	int i = 5;
	if(ifname == NULL){
		memcpy(local_ifname, "wlan0\0\0", 10);
	}else{
		memcpy(local_ifname, ifname, sizeof(local_ifname));
	}
	for(i=0; i < 5; i++){
		if(global == NULL){
			// global is not exist
			printf("global is not exist.....\n");
			goto Failed;
		}else if(global->ifaces == NULL){
			//no available interface
			printf("no available wlan interface.....\n");
			goto Failed;
		}
		//traverse iface list, and find interface of ifname
		for(wpa_s = global->ifaces; wpa_s; wpa_s = wpa_s->next){
			if(strncmp(wpa_s->ifname, local_ifname, sizeof(wpa_s->ifname)) == 0){
				wpa_printf(MSG_DEBUG, "get_wpa_supplicant_interface success, counter = %d", i);
				return global->ifaces;
			}
		}
Failed:
		wpa_printf(MSG_DEBUG, "get_wpa_supplicant_interface failed, waiting time: %d ms...",
				waiting_sec*1000 + waiting_usec/1000);
		os_sleep(waiting_sec, waiting_usec);
	}

	return NULL;
}

int hal_send_msg_to_wpa_supplicant(char *ifname, char *msg, int len, int l2_or_driver){
	if(wpa_s_const == NULL){
		wpa_s_const = get_wpa_supplicant_interface(ifname, 0,500000);
		if(wpa_s_const == NULL){
			return -1;
		}
	}

	if(l2_or_driver == 0){
		return hal_send_msg_to_wpa_supplicant_l2_packet(wpa_s_const->l2, msg, len);
	}else if(l2_or_driver){
		return hal_send_msg_to_wpa_supplicant_driver(wpa_s_const->drv_priv, msg, len);
	}else{
		wpa_printf(MSG_DEBUG, "unknown data packet from hal");
	}
	return -1;
}


