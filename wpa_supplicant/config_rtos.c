/*
 * WPA Supplicant / Configuration backend: empty starting point
 * Copyright (c) 2003-2005, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 *
 * This file implements dummy example of a configuration backend. None of the
 * functions are actually implemented so this can be used as a simple
 * compilation test or a starting point for a new configuration backend.
 */

#include "includes.h"

#include "common.h"
#include "config.h"
#include "base64.h"

static int wpa_check_validate_network(struct wpa_ssid *ssid, int line)
{
	int errors = 0;

	if (ssid->passphrase) {
		if (ssid->psk_set) {
			wpa_printf(MSG_ERROR, "both PSK and passphrase configured.");
			errors++;
		}
		wpa_config_update_psk(ssid);
	}

	if ((ssid->group_cipher & WPA_CIPHER_CCMP) &&
	    !(ssid->pairwise_cipher & WPA_CIPHER_CCMP) &&
	    !(ssid->pairwise_cipher & WPA_CIPHER_NONE)) {
		/* Group cipher cannot be stronger than the pairwise cipher. */
		wpa_printf(MSG_DEBUG, "removed CCMP from group cipher"
			   " list since it was not allowed for pairwise cipher");
		ssid->group_cipher &= ~WPA_CIPHER_CCMP;
	}

	if (ssid->mode == WPAS_MODE_MESH &&
	    (ssid->key_mgmt != WPA_KEY_MGMT_NONE &&
	    ssid->key_mgmt != WPA_KEY_MGMT_SAE)) {
		wpa_printf(MSG_ERROR,
			   "key_mgmt for mesh network should be open or SAE");
		errors++;
	}

	return errors;
}

static char * wpa_parse_string(const char *value, size_t *len)
{
	if (*value == '"') {
		const char *pos;
		char *str;
		value++;
		pos = os_strrchr(value, '"');
		if (pos == NULL || pos[1] != '\0')
			return NULL;
		*len = pos - value;
		str = dup_binstr(value, *len);
		if (str == NULL)
			return NULL;
		return str;
	}
	return NULL;
}

static int wpa_parse_key_mgmt(struct wpa_ssid *ssid, char* key_mgmt)
{
	int val = 0, errors = 0;
	char *start = key_mgmt;

	if (os_strcmp(start, "WPA-PSK") == 0)
		val |= WPA_KEY_MGMT_PSK;
	else if (os_strcmp(start, "WPA-EAP") == 0)
		val |= WPA_KEY_MGMT_IEEE8021X;
	else if (os_strcmp(start, "IEEE8021X") == 0)
		val |= WPA_KEY_MGMT_IEEE8021X_NO_WPA;
	else if (os_strcmp(start, "NONE") == 0)
		val |= WPA_KEY_MGMT_NONE;
	else if (os_strcmp(start, "WPA-NONE") == 0)
		val |= WPA_KEY_MGMT_WPA_NONE;
	else {
		wpa_printf(MSG_ERROR, "invalid key_mgmt '%s'", start);
		errors++;
	}

	if (val == 0) {
		wpa_printf(MSG_ERROR, "no key_mgmt values configured.");
		errors++;
	}

	wpa_printf(MSG_MSGDUMP, "key_mgmt: 0x%x", val);
	ssid->key_mgmt = val;
	return errors ? -1 : 0;
}

static int wpa_parse_psk(struct wpa_ssid *ssid, const char *value)
{
	if (*value == '"') {
#ifndef CONFIG_NO_PBKDF2
		const char *pos;
		size_t len;

		value++;
		pos = os_strrchr(value, '"');
		if (pos)
			len = pos - value;
		else
			len = os_strlen(value);
		if (len < 8 || len > 63) {
			wpa_printf(MSG_ERROR, "Invalid passphrase "
				   "length %lu (expected: 8..63) '%s'.",
				    (unsigned long) len, value);
			return -1;
		}
		wpa_hexdump_ascii_key(MSG_MSGDUMP, "PSK (ASCII passphrase)",
				      (u8 *) value, len);
		if (ssid->passphrase && os_strlen(ssid->passphrase) == len &&
		    os_memcmp(ssid->passphrase, value, len) == 0)
			return 0;
		ssid->psk_set = 0;
		str_clear_free(ssid->passphrase);
		ssid->passphrase = dup_binstr(value, len);
		if (ssid->passphrase == NULL)
			return -1;
		return 0;
#else /* CONFIG_NO_PBKDF2 */
		wpa_printf(MSG_ERROR, "Line %d: ASCII passphrase not "
			   "supported.", line);
		return -1;
#endif /* CONFIG_NO_PBKDF2 */
	}

	wpa_printf(MSG_ERROR, "Invalid passphrase from conf file");
	return -1;
}

static struct wpa_ssid * wpa_config_read_network(const char *f, int *line, int id)
{
	struct wpa_ssid *ssid;
	int errors = 0;
	struct conf *info = NULL;

	wpa_printf(MSG_MSGDUMP, "start of a new network block");
	ssid = os_zalloc(sizeof(*ssid));
	if (ssid == NULL)
		return NULL;
	dl_list_init(&ssid->psk_list);
	ssid->id = id;

	wpa_config_set_network_defaults(ssid);

	info = read_configration_from_user(f);
	if(info == NULL)
		errors++;
	else{
		ssid->ssid = (unsigned char *)wpa_parse_string(info->ssid, &ssid->ssid_len);
		if (ssid->ssid == NULL)
			errors++;

		ssid->scan_ssid = info->scan_ssid;

		if(wpa_parse_key_mgmt(ssid, info->key_mgmt) != 0)
			errors++;

		if(wpa_parse_psk(ssid, info->passphrare_or_psk) != 0)
			errors++;

		errors += wpa_check_validate_network(ssid, *line);
	}
	if (errors) {
		wpa_config_free_ssid(ssid);
		ssid = NULL;
	}

	return ssid;
}

struct wpa_config * wpa_config_read(const char *name, struct wpa_config *cfgp)
{
	int errors = 0, line = 0;
	struct wpa_ssid *ssid, *tail, *head;
	struct wpa_cred *cred_tail, *cred_head;
	struct wpa_config *config;
	int id = 0;

	if (name == NULL)
		return NULL;
	if (cfgp)
		config = cfgp;
	else
		config = wpa_config_alloc_empty(NULL, NULL);
	if (config == NULL) {
		wpa_printf(MSG_ERROR, "Failed to allocate config file "
			   "structure");
		return NULL;
	}
	tail = head = config->ssid;
	while (tail && tail->next)
		tail = tail->next;
	cred_tail = cred_head = config->cred;
	while (cred_tail && cred_tail->next)
		cred_tail = cred_tail->next;

	wpa_printf(MSG_DEBUG, "Reading configuration file '%s'", name);
	if (name == NULL) {
		wpa_printf(MSG_ERROR, "Failed to open config file '%s'", name);
		os_free(config);
		return NULL;
	}

	if (os_strcmp(name, "network") == 0) {
		ssid = wpa_config_read_network(name, &line, id++);
		if (ssid == NULL) {
			wpa_printf(MSG_ERROR, "Line %d: failed to "
				   "parse network block.", line);
			errors++;
		}else{
			if (head == NULL) {
				head = tail = ssid;
			} else {
				tail->next = ssid;
				tail = ssid;
			}
			if (wpa_config_add_prio_network(config, ssid)) {
				wpa_printf(MSG_ERROR, "Line %d: failed to add "
					   "network block to priority list.",
					   line);
				errors++;
			}
		}
	}

	config->ssid = head;
	wpa_config_debug_dump_networks(config);
	config->cred = cred_head;

#ifndef WPA_IGNORE_CONFIG_ERRORS
	if (errors) {
		wpa_config_free(config);
		config = NULL;
		head = NULL;
	}
#endif /* WPA_IGNORE_CONFIG_ERRORS */

	/* TODO: fill in configuration data */
	return config;
}


int wpa_config_write(const char *name, struct wpa_config *config)
{
	struct wpa_ssid *ssid;
	struct wpa_config_blob *blob;

	wpa_printf(MSG_DEBUG, "Writing configuration file '%s'", name);

	/* TODO: write global config parameters */


	for (ssid = config->ssid; ssid; ssid = ssid->next) {
		/* TODO: write networks */
	}

	for (blob = config->blobs; blob; blob = blob->next) {
		/* TODO: write blobs */
	}

	return 0;
}
