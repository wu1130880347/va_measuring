#include "bsp_spi.h"
#include "bsp_system.h"

#define SPI_CS_PIN(x) 
#define SPI_CS_GPIO(x) 
#define SPI_CS_EN  
#define SPI_CS_DIS

void spi_init(void);
uint8_t spi_read_write(uint8_t write_dat);
void SPI_CS_ENABLE(uint8_t channel);
void SPI_CS_DISABLE(uint8_t channel);

void spi_init(void)
{
    SPI_InitTypeDef SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable peripheral clocks --------------------------------------------------*/
    /* Enable SPIy clock and GPIO clock for SPIy */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    //GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); //disable JTAG
    /* GPIO configuration ------------------------------------------------------*/
    /* Configure SPIy pins: SCK, MISO and MOSI ---------------------------------*/
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* SPI_INTERFACE Config -------------------------------------------------------------*/
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256; // 5 MHz
    //SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; // 10 MHz
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);
    SPI_Cmd(SPI1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA,GPIO_Pin_3);
    //reset ic
    // GPIO_ResetBits(GPIOA,GPIO_Pin_4);
    // bsp_delay_nms(100);
    GPIO_SetBits(GPIOA,GPIO_Pin_4);
}
uint8_t spi_read_write(uint8_t write_dat)
{
    static uint16_t i = 0;
    /* Send SPIy data */
    SPI_I2S_SendData(SPI1, write_dat);
    i = 0;
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
    {
        if (i++ > 10000)
        {
            return 0xff;
        }
    }
    return SPI_I2S_ReceiveData(SPI1);
}
void spi_write(uint8_t write_dat)
{
    SPI_I2S_SendData(SPI1, write_dat);
}
uint8_t spi_read(void)
{
    return SPI_I2S_ReceiveData(SPI1);
}
void SPI_CS_ENABLE(uint8_t channel)
{
    GPIO_ResetBits(GPIOA,GPIO_Pin_3);
}
void SPI_CS_DISABLE(uint8_t channel)
{
    GPIO_SetBits(GPIOA,GPIO_Pin_3);
}