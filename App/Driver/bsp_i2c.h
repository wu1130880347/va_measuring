#ifndef __BSP_I2C_H__
#define __BSP_I2C_H__
#include "bsp_head.h"

extern void I2C_Init(uint8_t i2c_ch);
extern void I2C_DeInit(uint8_t i2c_ch);
extern uint8_t I2C_Write_One_Byte(uint8_t daddr, uint8_t addr, uint8_t data);
extern uint8_t I2C_Read_One_Byte(uint8_t daddr, uint8_t addr);
extern void Rtc_Read_n_Byte(uint8_t daddr, uint8_t *dat,uint8_t len);
extern void Rtc_Wirte_n_Byte(uint8_t daddr, uint8_t *dat,uint8_t len);

#endif