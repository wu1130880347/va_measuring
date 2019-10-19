#ifndef __BSP_SPI_H__
#define __BSP_SPI_H__
#include "bsp_head.h"
extern void spi_init(void);
extern uint8_t spi_read_write(uint8_t write_dat);
extern void SPI_CS_ENABLE(uint8_t channel);
extern void SPI_CS_DISABLE(uint8_t channel);
#endif