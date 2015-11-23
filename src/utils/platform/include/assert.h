#ifndef __ASSERT_H__
#define __ASSERT_H__
//#include "uart.h"

#ifdef DBUG

#define assert(assertion) do { \
                                if(!(assertion)) \
                                {	printf("assertion failed:(%s), file: %s, line: %d, func: %s\n", \
                                        #assertion, __FILE__, __LINE__, __func__); \
                                    while(1); \
                                } \
	                        } while(0)


    
#else

#define assert(assertion) do { \
                                if(!(assertion)) \
                                {	 while(1); } \
	                        } while(0)

#endif
	                        

#endif
