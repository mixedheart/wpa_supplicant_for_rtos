/*
 * wpa_cli.h
 *
 *  Created on: 2015��12��4��
 *      Author: lisen
 */

#ifndef WPA_SUPPLICANT_CLI_H_
#define WPA_SUPPLICANT_CLI_H_

int wpa_supplicant_cli_task(int argc, char *argv[]);
void wpa_supplicant_cli_init(void);
void wpa_supplicant_cli_deinit(void);

#endif /* WPA_SUPPLICANT_CLI_H_ */
