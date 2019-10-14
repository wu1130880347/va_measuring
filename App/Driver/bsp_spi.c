#include "bsp_spi.h"
#include "bsp_system.h"

#define SPI_CS_PIN(x) ((x == BSP_SPI_FLASH)?SPI_CS_FLASH_Pin:(x == BSP_SPI_LORA)?SPI_CS_LORA_Pin:(x == BSP_SPI_CARD)?SPI_CS_CARD_Pin:SPI_CS_CARD_Pin)
#define SPI_CS_GPIO(x) ((x == BSP_SPI_FLASH)?SPI_CS_FLASH_GPIO:(x == BSP_SPI_LORA)?SPI_CS_LORA_GPIO:(x == BSP_SPI_CARD)?SPI_CS_CARD_GPIO:SPI_CS_CARD_GPIO)
#define SPI_CS_EN   HAL_GPIO_WritePin(spi_cs_gpio,spi_cs_pin,GPIO_PIN_RESET);
#define SPI_CS_DIS  HAL_GPIO_WritePin(spi_cs_gpio,spi_cs_pin,GPIO_PIN_SET);

void spi_deinit(BSP_Spi_Sel_t init_fg);
void spi_init(BSP_Spi_Sel_t init_fg);
uint8_t spi_read_write(uint8_t write_dat);

SPI_HandleTypeDef SpiHandle;
static GPIO_TypeDef *spi_cs_gpio;
static uint16_t spi_cs_pin;

void spi_init(BSP_Spi_Sel_t init_fg)
{
    spi_cs_gpio = SPI_CS_GPIO(init_fg);
    spi_cs_pin = SPI_CS_PIN(init_fg);
    GPIO_InitTypeDef GPIO_InitStruct;
    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* Enable GPIO TX/RX clock */
    SPIx_SCK_GPIO_CLK_ENABLE();
    SPIx_MISO_GPIO_CLK_ENABLE();
    SPIx_MOSI_GPIO_CLK_ENABLE();
    /* Enable SPI clock */
     SPIx_CLK_ENABLE();

    GPIO_InitStruct.Pin = SPI_CS_FLASH_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(SPI_CS_FLASH_GPIO, &GPIO_InitStruct);
    HAL_GPIO_WritePin(SPI_CS_FLASH_GPIO,SPI_CS_FLASH_Pin,GPIO_PIN_SET);

    GPIO_InitStruct.Pin = SPI_CS_LORA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(SPI_CS_LORA_GPIO, &GPIO_InitStruct);
    HAL_GPIO_WritePin(SPI_CS_LORA_GPIO,SPI_CS_LORA_Pin,GPIO_PIN_SET);

    GPIO_InitStruct.Pin = SPI_CS_CARD_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(SPI_CS_CARD_GPIO, &GPIO_InitStruct);
    HAL_GPIO_WritePin(SPI_CS_CARD_GPIO,SPI_CS_CARD_Pin,GPIO_PIN_SET);

    /*##-2- Configure peripheral GPIO ##########################################*/
    /* SPI SCK GPIO pin configuration  */
    GPIO_InitStruct.Pin = SPIx_SCK_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = SPIx_SCK_AF;

    HAL_GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_InitStruct);

    /* SPI MOSI GPIO pin configuration  */
    GPIO_InitStruct.Pin = SPIx_MOSI_PIN;
    GPIO_InitStruct.Alternate = SPIx_MOSI_AF;

    HAL_GPIO_Init(SPIx_MOSI_GPIO_PORT, &GPIO_InitStruct);

    /* SPI MISO GPIO pin configuration  */
    GPIO_InitStruct.Pin = SPIx_MISO_PIN;
    GPIO_InitStruct.Alternate = SPIx_MISO_AF;

    HAL_GPIO_Init(SPIx_MISO_GPIO_PORT, &GPIO_InitStruct);

    /*##-3- Configure the NVIC for SPI #########################################*/
    /* NVIC for SPI */
    HAL_NVIC_SetPriority(SPIx_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(SPIx_IRQn);

    /*##-1- Configure the SPI peripheral #######################################*/
    /* Set the SPI parameters */
    SpiHandle.Instance = SPIx;
    SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
    SpiHandle.Init.Direction = SPI_DIRECTION_2LINES;
    SpiHandle.Init.CLKPhase = (init_fg == BSP_SPI_CARD)?SPI_PHASE_1EDGE:SPI_PHASE_2EDGE;
    SpiHandle.Init.CLKPolarity = (init_fg == BSP_SPI_CARD)?SPI_POLARITY_LOW:SPI_POLARITY_HIGH;
    SpiHandle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    SpiHandle.Init.CRCPolynomial = 7;
    SpiHandle.Init.DataSize = SPI_DATASIZE_8BIT;
    SpiHandle.Init.FirstBit = SPI_FIRSTBIT_MSB;
    SpiHandle.Init.NSS = SPI_NSS_SOFT;
    SpiHandle.Init.TIMode = SPI_TIMODE_DISABLE;
    SpiHandle.Init.Mode = SPI_MODE_MASTER;
    if (HAL_SPI_Init(&SpiHandle) != HAL_OK)
    {
        /* Initialization Error */
        //Error_Handler();
    }
}

void spi_deinit(BSP_Spi_Sel_t init_fg)
{
    /*##-1- Reset peripherals ##################################################*/
    SPIx_FORCE_RESET();
    SPIx_RELEASE_RESET();

    // HAL_GPIO_DeInit(SPI_CS_FLASH_GPIO, SPI_CS_FLASH_Pin);
    // HAL_GPIO_DeInit(SPI_CS_LORA_GPIO, SPI_CS_LORA_Pin);
    // HAL_GPIO_DeInit(SPI_CS_CARD_GPIO, SPI_CS_CARD_Pin);
    HAL_GPIO_WritePin(SPI_CS_FLASH_GPIO,SPI_CS_FLASH_Pin,GPIO_PIN_SET);
    HAL_GPIO_WritePin(SPI_CS_LORA_GPIO,SPI_CS_LORA_Pin,GPIO_PIN_SET);
    HAL_GPIO_WritePin(SPI_CS_CARD_GPIO,SPI_CS_CARD_Pin,GPIO_PIN_SET);
    /*##-2- Disable peripherals and GPIO Clocks ################################*/
    /* Configure SPI SCK as alternate function  */
    HAL_GPIO_DeInit(SPIx_SCK_GPIO_PORT, SPIx_SCK_PIN);
    /* Configure SPI MISO as alternate function  */
    HAL_GPIO_DeInit(SPIx_MISO_GPIO_PORT, SPIx_MISO_PIN);
    /* Configure SPI MOSI as alternate function  */
    HAL_GPIO_DeInit(SPIx_MOSI_GPIO_PORT, SPIx_MOSI_PIN);

    /*##-3- Disable the NVIC for SPI ###########################################*/
     HAL_NVIC_DisableIRQ(SPIx_IRQn);
}

uint8_t MCU_SPI_Read(void)
{
    uint8_t Rxdata = 0;
    HAL_SPI_Receive(&SpiHandle, &Rxdata, 1, 10);
    return Rxdata;
}

void MCU_SPI_Write(uint8_t dat)
{
    HAL_SPI_Transmit(&SpiHandle, &dat, 1, 10);
    return;
}

void SPI_CS_ENABLE(void)
{
    SPI_CS_EN;
}
void SPI_CS_DISABLE(void)
{
    SPI_CS_DIS;
}

uint8_t spi_read_write(uint8_t write_dat)
{
    uint8_t read_dat = 0;
    HAL_SPI_TransmitReceive(&SpiHandle,&write_dat,&read_dat,1,10);
    // HAL_SPI_Transmit(&SpiHandle, &write_dat, 1, 10);
    // HAL_SPI_Receive(&SpiHandle, &read_dat, 1, 10);
    return read_dat;
}
