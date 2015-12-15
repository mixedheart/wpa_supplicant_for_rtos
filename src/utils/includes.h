/*
 * wpa_supplicant/hostapd - Default include files
 * Copyright (c) 2005-2006, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 *
 * This header file is included into all C files so that commonly used header
 * files can be selected with OS specific ifdef blocks in one place instead of
 * having to have OS/C library specific selection in many files.
 */

#ifndef INCLUDES_H
#define INCLUDES_H

#define WPA_RTOS_FS		0

/* Include possible build time configuration before including anything else */
#include "build_config.h"
#include "utils/os.h"
#include "ff.h"
#include "file.h"
#include "inc/com_uart.h"

#include <errno.h>
#include "inc/errno_base.h"
#include "utils/os.h"
#include "utils/list.h"
#include "utils/eloop.h"

#include "rtos.h"
#include "socket_rtos.h"
#include "inc/macro.h"
#include "inc/com_uart.h"
#include "hal.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

typedef u32 gid_t;
typedef u32 uid_t;
typedef u32 pid_t;

/**
 * TODO: should be known
 * in C language, if you want to override (either overload or rewrite)
 * variable, structure, function, you must include yourself header file(.h)
 * before including system library.
 * Especially, you should provide exactly, clearly, explicitly declaration of function,
 * and don't use extern key word.
 */
#define printf		uart_printf
#define FILE FIL

int wpa_supplicant_rename(const char * old, const char *new);
#define rename	wpa_supplicant_rename

#define	fopen		_fopen
#define	fclose		_fclose
#define	fread		_fread
#define	fwrite		_fwrite
#define fputs		_fputs
#define ftell		_ftell
#define	fseek		_flseek
#define perror		_perror
#define	exit		_exit

#define fprintf		f_printf
#define fputc		f_putc
#define	fgets		f_gets

#endif /* INCLUDES_H */
