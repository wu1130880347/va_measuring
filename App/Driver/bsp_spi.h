#ifndef __BSP_SPI_H__
#define __BSP_SPI_H__
#include "bsp_head.h"
extern void spi_init(void);
extern uint8_t spi_read_write(uint8_t write_dat);
extern void SPI_CS_ENABLE(uint8_t channel);
extern void SPI_CS_DISABLE(uint8_t channel);
extern void init_hc595(void);
extern void enable_ch_led(uint8_t ch);
extern void disable_ch_led(uint8_t ch);
extern void update_led_state(void);
#endif