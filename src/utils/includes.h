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

/* Include possible build time configuration before including anything else */
#include "build_config.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include <errno.h>
#include "inc/errno_base.h"
#include "utils/os.h"
#include "utils/list.h"

#include "rtos.h"
#include "socket_rtos.h"
#include "inc/macro.h"
#include "hal.h"
typedef u32 gid_t;
typedef u32 uid_t;
typedef u32 pid_t;

#endif /* INCLUDES_H */
