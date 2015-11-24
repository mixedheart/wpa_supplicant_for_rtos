#ifndef     GPIO_H
#define     GPIO_H
#include "../inc/macro.h"
#define     NA      0xFF

#define    GPIO_HIGH_LEVEL_INTR              0
#define    GPIO_LOW_LEVEL_INTR               2
#define    GPIO_CLK_RISING_INTR              4
#define    GPIO_CLK_FALLING_INTR             6
#define    GPIO_CLK_EDGE_INTR                8
#define    GPIO_PCLK_RISING_INTR             10
#define    GPIO_PCLK_FALLING_INTR            12
#define    GPIO_PCLK_EDGE_INTR               14
#define    GPIO_INTR_EN                      1
#define    GPIO_INTR_DIS                     0
extern int32_t gpio_set_mux_pin(uint8_t u8_gpio_id);
extern int32_t gpio_set_pin(uint8_t u8_gpio_id, uint8_t u8_dir, uint8_t u8_level);
#define     MUX_PIN_REG_ID(id)          ((id)>>5)
#define     MUX_PIN_PIN_ID(id)          ((id)&0x1F)
#define     MUX_PIN_REG(id)             (CTL_MUX0+MUX_PIN_REG_ID(id))


#define     GPIO_DIR_INPUT              0
#define     GPIO_DIR_OUTPUT             1
#define     GPIO_ID(id)                 ((id)>>7)
#define     GPIO_REG_ID(id)             (((id)>>4)&0x7)
#define     GPIO_EXT_REG_ID(id)         (((id)>>5)&0x3)
#define     GPIO_PIN_ID(id)             ((id)&0xF)
#define     GPIO_EXT_PIN_ID(id)         ((id)&0x1F)
#define     GPIO_INTR_REG_ID(id)        (((id)>>2)&0x1F) 
#define     GPIO_INTR_PIN_ID(id)        ((id)&0x3)
#define     GPIO_INTR_CLR_REG_ID(id)    (((id)>>5)&0x3)
#define     GPIO_INTR_CLR_PIN_ID(id)    ((id)&0x1F)

#define     GPIO_DIR_ADDR(id)           (GPIO0_PORT_DDR0 + GPIO_ID(id)*0x200 + GPIO_REG_ID(id))
#define     GPIO_DATA_ADDR(id)          (GPIO0_PORT_DR0 + GPIO_ID(id)*0x200 + GPIO_REG_ID(id))
#define     GPIO_EXT_DATA_ADDR(id)      (GPIO0_EXT_PORT0 + GPIO_ID(id)*0x200 + GPIO_EXT_REG_ID(id))
#define     GPIO_CLR_INTR_ADDR(id)      (GPIO0_INTR_CLR0 + GPIO_ID(id)*0x200 + GPIO_INTR_CLR_REG_ID(id))
#define     GPIO_INTR_ADDR(id)          (GPIO0_INTR_CTRL0 + GPIO_ID(id)*0x200 + GPIO_INTR_REG_ID(id))
#define     GPIO_DEBOUNCE_ADDR(id)      (GPIO0_DEBOUNCE0 + GPIO_ID(id)*0x200 + GPIO_REG_ID(id))
#define     GPIO_INTR_MSK_ADDR(id)      (GPIO0_INTR_MASK_C00 + GPIO_ID(id)*0x200 + GPIO_REG_ID(id))

//#define     GPIO_SET_PIN_DIR(id, dir)   SETBITFIELD_ATOM(GPIO_DIR_ADDR(id), ((dir)<<GPIO_PIN_ID(id)), ((1<<GPIO_PIN_ID(id)), 16)
//#define     GPIO_SET_PIN_DIR(id, dir)   *GPIO_DIR_ADDR(id) = (0x10000+(dir))<<GPIO_PIN_ID(id)
//#define     GPIO_SET_PIN_DAT(id, dat)   *GPIO_DATA_ADDR(id) = (0x10000+(dat))<<GPIO_PIN_ID(id)

#define     GPIO_SET_PIN_DIR(id, dir)   *GPIO_DIR_ADDR(id) = (0x10000L+(uint32_t)(dir))<<GPIO_PIN_ID(id)
#define     GPIO_SET_PIN_DAT(id, dat)   *GPIO_DATA_ADDR(id) = (0x10000L+(uint32_t)(dat))<<GPIO_PIN_ID(id)
#define     GPIO_GET_EXT_DATA(id)       TESTBIT(GPIO_EXT_DATA_ADDR(id), GPIO_EXT_PIN_ID(id))
#define     GPIO_SET_INTR_MOD(id, mod)  SETBITFIELD_ATOM(GPIO_INTR_ADDR(id), ((mod)<<(GPIO_INTR_PIN_ID(id)<<2)), (16+GPIO_INTR_PIN_ID(id)))
//
#define     GPIO_SET_PIN_OUTPUT(id)     SETBIT_ATOM(GPIO_DIR_ADDR(id), GPIO_PIN_ID(id), (16+GPIO_PIN_ID(id)))
#define     GPIO_SET_PIN_INPUT(id)      CLRBIT_ATOM(GPIO_DIR_ADDR(id), GPIO_PIN_ID(id), (16+GPIO_PIN_ID(id)))
#define     GPIO_SET_PIN_DR(id)         SETBIT_ATOM(GPIO_DATA_ADDR(id), GPIO_PIN_ID(id), (16+GPIO_PIN_ID(id)))
#define     GPIO_CLR_PIN_DR(id)         CLRBIT_ATOM(GPIO_DATA_ADDR(id), GPIO_PIN_ID(id), (16+GPIO_PIN_ID(id)))
#define     GPIO_MSK_PIN_INTR(id)       SETBIT_ATOM(GPIO_INTR_MSK_ADDR(id), GPIO_PIN_ID(id), (16+GPIO_PIN_ID(id)))
#define     GPIO_UNMSK_PIN_INTR(id)     CLRBIT_ATOM(GPIO_INTR_MSK_ADDR(id), GPIO_PIN_ID(id), (16+GPIO_PIN_ID(id)))
#define     GPIO_EN_DEBOUNCE(id)        SETBIT_ATOM(GPIO_DEBOUNCE_ADDR(id), GPIO_PIN_ID(id), (16+GPIO_PIN_ID(id)))
#define     GPIO_DIS_DEBOUNCE(id)       CLRBIT_ATOM(GPIO_DEBOUNCE_ADDR(id), GPIO_PIN_ID(id), (16+GPIO_PIN_ID(id)))
#define     GPIO_CLR_INTR(id)           *GPIO_CLR_INTR_ADDR(id) = (1L<<GPIO_INTR_CLR_PIN_ID(id))

#define     GPIO_SET_HIGH_OUTPUT(id)    gpio_set_pin(id, 1, 1)
#define     GPIO_SET_LOW_OUTPUT(id)     gpio_set_pin(id, 1, 0)
#define     GPIO_SET_INPUT(id)          gpio_set_pin(id, 0, 0)

#define     ENABLE_GPIO_INTR()          GIC_IntCfg(IntID_GPIO, 1, 1, 1, 0, 0)
struct gpio_map_table
{
    uint8_t  u8_mux_pin;
    volatile uint32_t* u32p_reg;
    uint8_t  u8_bit_pos;
    uint8_t  u8_val;
};
typedef struct gpio_map_table GPIO_MAP_TABLE;
extern GPIO_MAP_TABLE map_table [];
#endif

