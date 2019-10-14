#ifndef __BSP_SPI_H__
#define __BSP_SPI_H__
#include "bsp_head.h"
typedef enum
{
    BSP_SPI_FLASH = 0,
    BSP_SPI_CARD,
    BSP_SPI_LORA
}BSP_Spi_Sel_t;

extern void spi_deinit(BSP_Spi_Sel_t init_fg);
extern void spi_init(BSP_Spi_Sel_t init_fg);
extern uint8_t spi_read_write(uint8_t write_dat);
extern uint8_t MCU_SPI_Read(void);
extern void MCU_SPI_Write(uint8_t dat);
void SPI_CS_ENABLE(void);
void SPI_CS_DISABLE(void);
#endif