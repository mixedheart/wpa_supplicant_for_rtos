/***************************************************************************
Leadcore Technology Co.Ltd. Copyright@2010
All rights reserved.
	
Filename:               macro.h
File description:   This file contains all functions statement and general 
                                macro defination for LC760 FPGA chip test.
Author:                   niuyue
Date:                      2010-8-4
Version:                v1.0
Version history:	v1.0 created by niuyue
            
****************************************************************************/

#ifndef     MACRO_H
#define     MACRO_H
#include    "common.h"
/*basic bit or reg operation*/
#define     REG_VALUE(reg)                      (*(REGISTER*)(reg))
#define     BIT_FIELD(len)                      (((uint32_t)1 << (len)) - 1)
#define     SETBITFIELD(reg, op, bit, len)      REG_VALUE(reg) = (REG_VALUE(reg))&(~(BIT_FIELD(len)<<(bit)))|((uint32_t)(op)<<(bit))
#define     SETBITVALUE(reg, op, bit, len)      SETBITFIELD(reg, op, bit, len)

#define     IOW(reg, op, bit)                   REG_VALUE(reg) = ((uint32_t)(op)<<(bit))

#define     SETBIT(reg, bit)                    SETBITVALUE(reg, 1, bit, 1)
#define     CLRBIT(reg, bit)                    SETBITVALUE(reg, 0, bit, 1)

#define     GETBIT(reg, bit, len)               (REG_VALUE(reg)>>bit)&(BIT_FIELD(len))
#define     TESTBIT(reg, bit)                   GETBIT(reg, bit, 1)

#define     GETSTATUS(status, reg, bit, len)    status = GETBIT(reg, bit, len)
#define     GET_REVERSE_BIT(reg, bit, len)      ((~REG_VALUE(reg))&(BIT_FIELD(len)<<(bit)))
//#define     REVERSE_BIT(reg, bit, len)          (SETBITVALUE(reg, 0, bit, len)|GET_RESVERSE_BIT(reg, bit, len))
#define     REVERSE_BIT(reg, bit, len)          REG_VALUE(reg) = (SETBITVALUE(reg, 0, bit, len)|GET_REVERSE_BIT(reg, bit, len))
#define     TOGGLEBIT(reg, bit, len)            REVERSE_BIT(reg, bit, len)

/*#define     SETBITSET_ATOM(reg, val, bit_en, we_begin)          REG_VALUE(reg) = (val)+((bit_en)<<(we_begin))
#define     SETBITFIELD_ATOM_BIT(reg, val, pos, we)             REG_VALUE(reg) = ((val)<<(pos))+((1L)<<(we))
#define     SETBITVALUE_ATOM_BIT(reg, val, we)                  REG_VALUE(reg) = (val)+((bit_en)<<(we_begin))
#define     SETBIT_ATOM(reg, bit, we_begin)                     REG_VALUE(reg) = (1L+(1L<<(we_begin)))<<(bit)
#define     CLRBIT_ATOM(reg, bit, we_begin)                     REG_VALUE(reg) = (0L+(1L<<(we_begin)))<<(bit)
#define     SETBIT_ATOM_BIT(reg, bit, we)                       REG_VALUE(reg) = (1L<<(bit))+(1L<<(we))
#define     CLRBIT_ATOM_BIT(reg, bit, we)                       REG_VALUE(reg) = (0L<<(bit))+(1L<<(we))
*/

/*atom bit set with write enable*/
#define     SETBITGROUP_ATOM(reg, val, bit_en, we_begin)        REG_VALUE(reg) = (val)+((uint32_t)(bit_en)<<(we_begin))
#define     SETBITFIELD_ATOM(reg, val, we)                      REG_VALUE(reg) = ((val) + (1L<<(we)))
#define     SETMULTIBITFIELD_ATOM(reg, val, we_grp, we_beg)     REG_VALUE(reg) = ((val) + (((uint32_t)(we_grp))<<(we_beg)))
#define     SETBIT_ATOM(reg, bit, we)                           REG_VALUE(reg) = (1L<<(bit))+(1L<<(we))
#define     CLRBIT_ATOM(reg, bit, we)                           REG_VALUE(reg) = (0L<<(bit))+(1L<<(we))

/*widely used macro*/
#define     GPIO_SET_FLAG()     {\
                                *GPIO0_PORTA_DDR |= 0x01;\
                                *GPIO0_PORTA_DR  |= 0x01 ;\
                                }
                                

#define     SET_FLAG(flag)      flag = 1
#define	    CLR_FLAG(flag)      flag = 0

#ifndef     CTL_ZSP_INTEN
#define     CTL_ZSP_INTEN()                 SETBITFIELD(CTL_ZSP_CTRL, 1, 28, 1)

#define     XY(x, y)            ((x) + ((y)<<16))

#endif
#endif


