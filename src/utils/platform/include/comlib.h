#ifndef _COMLIB_FUN_H
#define _COMLIB_FUN_H

//#include "datatype.h"
//#include "uart.h"
#include <stddef.h>

#include "platform/rtos.h"                        /* RTOS API defines */
#include "assert.h"
//#include "micopts.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void *linmalloc(size_t size);
extern void *linmalloc_lwip(size_t size);
extern void linfree(void *memblock);

//#ifndef malloc
//#define malloc  linmalloc
//#endif
//
//#ifndef free
//#define free  linfree
//#endif

//#define memcpy lz_memcpy
#if 0
struct hostent
{
	char *h_name;
	char **h_aliases;
	int h_addrtype;
	int h_length;
	char **h_addr_list;
};
#endif
#define CheckStack
#ifndef CheckStack
#define CheckStack   printf("%s stack remain=%d\n\n\n", __FUNCTION__, NU_Check_Stack())
#endif

#ifdef __cplusplus
}
#endif

#endif
