/*
 * platform.h
 *
 *  Created on: 2015Äê9ÔÂ2ÈÕ
 *      Author: niuyue
 */

#ifndef PLATFORM_DEF_H_
#define PLATFORM_DEF_H_


#if defined(OS_NUCLEUS)
#include "nucleus/plat_def/plat_nucleus.h"
#elif defined(OS_FREERTOS)
#include "freertos/plat_def/plat_freertos.h"
#endif
//#include "rtos.h"
#endif /* PLATFORM_PLATFORM_H_ */
