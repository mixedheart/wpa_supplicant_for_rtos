#ifndef HAL_H
#define HAL_H

#include "rtos.h"

int hal_l2_send(const u8 *dst_addr, u16 proto, const u8 *buf, size_t len);

#endif
