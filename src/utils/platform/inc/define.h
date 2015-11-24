#ifndef	DEFINE_H
#define DEFINE_H

#include "common.h"

#ifdef		_ZSP
typedef		int					int16_t;
typedef 	long 				int32_t;
typedef 	unsigned int 		uint16_t;
typedef 	unsigned long 		uint32_t;
typedef		volatile uint32_t 	REGISTER;
typedef 	int16_t 			boolean;
#define		OFFSET 				2
#endif

#ifdef		_ARM
#if 0
typedef		char				int8_t;
typedef		short 				int16_t;
typedef		int 				int32_t;
typedef		long long 			int64_t;
typedef		unsigned char 		uint8_t;
typedef 	unsigned short 		uint16_t;
typedef 	unsigned int 		uint32_t;
typedef 	unsigned long long 	uint64_t;
typedef		volatile uint32_t 	REGISTER;
typedef 	int16_t 			boolean;
#endif
#define		OFFSET 				4
#endif


#ifdef	_GLOBAL_DEF

#ifndef	ERROR
#define	ERROR               ((int16_t)(-1))            
#endif

#ifndef	OK
#define	OK                  ((boolean)(1))                
#endif

#ifndef	FALSE
#define	FALSE               ((boolean)(0)) 
#endif

#ifndef	TRUE
#define	TRUE                ((boolean)(1)) 
#endif

#ifndef	NULL
#define	NULL                0L
#endif

#ifndef	ENABLE
#define	ENABLE              ((boolean)(1)) 
#endif

#ifndef	DISABLE
#define	DISABLE             ((boolean)(0)) 
#endif

#ifndef	CLR	
#define	CLR                 ((boolean)(1))
#endif
#endif

#endif
