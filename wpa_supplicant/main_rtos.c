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

struct l2_packet_data* get_wpa_l2_packet(void)
{
	return global->ifaces->l2;
}

int get_wpa_supplicant_socket_fd(const char *ifname)
{
	struct wpa_supplicant *wpa_s;
	if(global == NULL){
		// global is not exist
		printf("global is not exist.....\n");
		return -2;
	}else if(global->ifaces == NULL){
		//no available interface
		printf("no available wlan interface.....\n");
		return -3;
	}

	//traverse iface list, and find interface of ifname
	for(wpa_s = global->ifaces; wpa_s; wpa_s = wpa_s->next)
	{
		if(strncmp(wpa_s->ifname, ifname, sizeof(wpa_s->ifname)) == 0)
		{
			return global->ifaces->l2->fd;
		}
	}

	//not found
	printf("interface: %s not found....\n", ifname);
	return -1;
}
