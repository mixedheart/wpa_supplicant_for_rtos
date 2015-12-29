#ifndef HAL_H
#define HAL_H

#include "rtos.h"
#include "socket_rtos.h"

int hal_l2_send(const u8 *dst_addr, u16 proto, const u8 *buf, size_t len);
int hal_get_ip_addr(struct sockaddr_in *saddr);

void trigger_to_scan(void);
void trigger_to_join(void);
void trigger_to_connect(void);
void trigger_to_disconnect(void);

#endif
