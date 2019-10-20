#include "bsp_spi.h"
#include "bsp_system.h"

#define SPI_CS_PIN(x) 
#define SPI_CS_GPIO(x) 
#define SPI_CS_EN  
#define SPI_CS_DIS

#define LED_SCL_H do{PBout(13) = 1;bsp_delay_nus(1);}while(0);
#define LED_SCL_L do{PBout(13) = 0;bsp_delay_nus(1);}while(0);
#define LED_SDA_H do{PBout(15) = 1;bsp_delay_nus(1);}while(0);
#define LED_SDA_L do{PBout(15) = 0;bsp_delay_nus(1);}while(0);

#define LED_EN_IC PAout(8) = 0;
#define LED_DIS_IC PAout(8) = 1;

#define LED595_LATCH_RESET PBout(12) = 0;
#define LED595_LATCH_SET PBout(12) = 1;
#define LED_UPDATE_IC do{LED595_LATCH_RESET;bsp_delay_nus(100); LED595_LATCH_SET;}while(0);

void spi_init(void);
uint8_t spi_read_write(uint8_t write_dat);
void SPI_CS_ENABLE(uint8_t channel);
void SPI_CS_DISABLE(uint8_t channel);
void init_hc595(void);

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

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
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
void init_hc595(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);
    //init hc595 EN
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOA,GPIO_Pin_8);//EN_IC
    //init hc595 SDA
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB,GPIO_Pin_15);
    //init hc595 SCL
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB,GPIO_Pin_13);
    //init hc595 updata
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_12);
    LED_EN_IC;
    LED_SCL_L;
    LED_SDA_L;
}
static void he595_send_update(uint8_t dat)
{
    uint8_t i = 0;
    for (i = 0; i < 8; i++)
    {
        if ((dat & 0x80) == 0x80)
        {
            LED_SDA_H;
        }
        else
        {
            LED_SDA_L;
        }

        dat = dat << 1;
        LED_SCL_L;
        bsp_delay_nus(1);
        LED_SCL_H;
    }
    //updata data to ic pin
    LED_UPDATE_IC;
}
void SPI_CS_ENABLE(uint8_t channel)
{
    he595_send_update(0xFD);
}
void SPI_CS_DISABLE(uint8_t channel)
{
    he595_send_update(0xFF);
}