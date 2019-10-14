#ifndef __BSP_HW_CONFIG_H__
#define __BSP_HW_CONFIG_H__

//±‡“Îøÿ÷∆
#define __ON__ 1
#define __OFF__ 0
//µ˜ ‘–≈œ¢◊‹ø™πÿ
#define DEBUG_TEST __OFF__
#define USART_DEBUG __OFF__
#define UNIT_TEST __OFF__

//RTC I2C3
#define I2C3_SCL_Pin    GPIO_PIN_0
#define I2C3_SDA_Pin    GPIO_PIN_1
#define I2C3_GPIO       GPIOC

//µÁ»›∞¥º¸≈‰÷√I2C1
#define I2C1_SCL_Pin    GPIO_PIN_8
#define I2C1_SDA_Pin    GPIO_PIN_9
#define I2C1_GPIO       GPIOB


#define CAP_EN_Pin      GPIO_PIN_12
#define CAP_EN_GPIO     GPIOC

#define CAP_INT_Pin     GPIO_PIN_0
#define CAP_INT_GPIO    GPIOA

//LEDœ‘ æ≈‰÷√
#define LED_SCL_Pin     GPIO_PIN_8
#define LED_SDA_Pin     GPIO_PIN_9
#define LED_DAT_GPIO    GPIOB

#define LED_EN_Pin      GPIO_PIN_10
#define LED_EN_GPIO     GPIOA

#define LED_UPDATE_Pin  GPIO_PIN_9
#define LED_UPDATE_GPIO GPIOA

#define LED_PWR_LED_Pin GPIO_PIN_7
#define LED_PWR_LED_GPIO GPIOC

//LoraÕ®–≈≈‰÷√
#define LORA_IRQ_Pin    GPIO_PIN_13
#define LORA_IRQ_GPIO   GPIOC

#define LORA_RESET_Pin  GPIO_PIN_9
#define LORA_RESET_GPIO GPIOC

//¥Æø⁄≈‰÷√
#define USARTx_TX_Pin   GPIO_PIN_3
#define USARTx_TX_GPIO  GPIOB

#define USARTx_RX_Pin   GPIO_PIN_4
#define USARTx_RX_GPIO  GPIOB

#define USARTx              USART5
#define USARTx_TX_AF        GPIO_AF6_USART5
#define USARTx_RX_AF        GPIO_AF6_USART5
#define USARTx_IRQn         USART4_5_IRQn
#define USARTx_IRQHandler   USART4_5_IRQHandler
#define USARTx_CLK_ENABLE()              __HAL_RCC_USART5_CLK_ENABLE();
#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE() 
#define USARTx_FORCE_RESET()             __HAL_RCC_USART5_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __HAL_RCC_USART5_RELEASE_RESET()

//À¯–≈∫≈≈‰÷√
#define LOCK_PWR_Pin       GPIO_PIN_8
#define LOCK_PWR_GPIO      GPIOC

#define LOCK_A_Pin         GPIO_PIN_12
#define LOCK_A_GPIO        GPIOA

#define LOCK_B_Pin         GPIO_PIN_11
#define LOCK_B_GPIO        GPIOA

#define LOCK_S1_Pin        GPIO_PIN_6 
#define LOCK_S1_GPIO        GPIOC

#define LOCK_S2_Pin        GPIO_PIN_7
#define LOCK_S2_GPIO       GPIOA

#define LOCK_S3_Pin        GPIO_PIN_6
#define LOCK_S3_GPIO       GPIOA

//∑‰√˘∆˜≈‰÷√
#define BEEP_FREQ_Pin        GPIO_PIN_2
#define BEEP_FREQ_GPIO       GPIOA

//SPI≈‰÷√
#define SPI_CS_FLASH_Pin        GPIO_PIN_2
#define SPI_CS_FLASH_GPIO       GPIOC
#define SPI_CS_LORA_Pin         GPIO_PIN_10
#define SPI_CS_LORA_GPIO        GPIOC
#define SPI_CS_CARD_Pin         GPIO_PIN_6
#define SPI_CS_CARD_GPIO        GPIOB

#define SPIx                             SPI2
#define SPIx_CLK_ENABLE()                __HAL_RCC_SPI2_CLK_ENABLE()
#define SPIx_SCK_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPIx_MISO_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE() 
#define SPIx_MOSI_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE() 

#define SPIx_FORCE_RESET()               __HAL_RCC_SPI2_FORCE_RESET()
#define SPIx_RELEASE_RESET()             __HAL_RCC_SPI2_RELEASE_RESET()

/* Definition for SPIx Pins */
#define SPIx_SCK_PIN                     GPIO_PIN_13
#define SPIx_SCK_GPIO_PORT               GPIOB
#define SPIx_SCK_AF                      GPIO_AF0_SPI2
#define SPIx_MISO_PIN                    GPIO_PIN_14
#define SPIx_MISO_GPIO_PORT              GPIOB
#define SPIx_MISO_AF                     GPIO_AF0_SPI2
#define SPIx_MOSI_PIN                    GPIO_PIN_15
#define SPIx_MOSI_GPIO_PORT              GPIOB
#define SPIx_MOSI_AF                     GPIO_AF0_SPI2
/* Definition for SPIx's NVIC */
#define SPIx_IRQn                        SPI2_IRQn
#define SPIx_IRQHandler                  SPI2_IRQHandler

//FLASH POWERON config
#define FLASH_POWER_Pin     GPIO_PIN_5
#define FLASH_POWER_GPIO    GPIOB

//CARD RC5XX config
#define CARD_RC5XX_IRQ_Pin      GPIO_PIN_7
#define CARD_RC5XX_IRQ_GPIO     GPIOB

#define CARD_RC5XX_RST_Pin      GPIO_PIN_15
#define CARD_RC5XX_RST_GPIO     GPIOA



#endif