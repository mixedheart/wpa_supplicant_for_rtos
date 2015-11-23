
//- 文件定义了LC1810 A9/arm926ej-s中的数据类型。

#ifndef	__DEFINE__H
#define __DEFINE__H

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
#define		OFFSET 				4

//you can add other data tpye from here.

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
//#define	ENABLE              ((boolean)(1))
#define	ENABLE              ((1L))
#endif

#ifndef	DISABLE
#define	DISABLE             ((0L))
#endif

#ifndef	CLR	
#define	CLR                 ((boolean)(1))
#endif
#endif

#endif


/* data type macro */
/*typedef void * VOID_PTR;
typedef unsigned char uint8_t ;
typedef unsigned char * UINT8_PTR;
typedef unsigned short UINT16;
typedef unsigned short * UINT16_PTR;
typedef unsigned long int UINT32;
typedef unsigned long int * UINT32_PTR;
typedef signed char SINT8;
typedef signed char * SINT8_PTR;
typedef signed short SINT16;
typedef signed short * SINT16_PTR;
typedef signed int SINT32;
typedef signed int * SINT32_PTR;
typedef unsigned char BOOLEAN;*/

