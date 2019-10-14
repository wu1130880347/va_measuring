#include "bsp_uart.h"
#include "bsp_systimer.h"

//串口调试定义宏
#if ((defined USART_DEBUG) && (USART_DEBUG == __ON__))
#define TAG "bsp_uart: "
#define EN_LOG GREEN
#endif

UART_HandleTypeDef UartHandle;
extern BSP_FLU_Package_t bsp_Rx_flu;
extern "C"
{
  static void UART_MspInit(UART_HandleTypeDef *huart)
  {
    GPIO_InitTypeDef GPIO_InitStruct;

    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* Enable GPIO TX/RX clock */
    USARTx_TX_GPIO_CLK_ENABLE();
    USARTx_RX_GPIO_CLK_ENABLE();

    /* Enable USARTx clock */
    USARTx_CLK_ENABLE();

    /*##-2- Configure peripheral GPIO ##########################################*/
    /* UART TX GPIO pin configuration  */
    GPIO_InitStruct.Pin = USARTx_TX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = USARTx_TX_AF;

    HAL_GPIO_Init(USARTx_TX_GPIO, &GPIO_InitStruct);

    /* UART RX GPIO pin configuration  */
    GPIO_InitStruct.Pin = USARTx_RX_Pin;
    GPIO_InitStruct.Alternate = USARTx_RX_AF;

    HAL_GPIO_Init(USARTx_RX_GPIO, &GPIO_InitStruct);

    /*##-3- Configure the NVIC for UART ########################################*/
    /* NVIC for USART */
    HAL_NVIC_SetPriority(USARTx_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(USARTx_IRQn);
  }

  /**
  * @brief UART MSP De-Initialization 
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO and NVIC configuration to their default state
  * @param huart: UART handle pointer
  * @retval None
  */
  static void UART_MspDeInit(UART_HandleTypeDef *huart)
  {
    /*##-1- Reset peripherals ##################################################*/
    USARTx_FORCE_RESET();
    USARTx_RELEASE_RESET();

    /*##-2- Disable peripherals and GPIO Clocks #################################*/
    /* Configure UART Tx as alternate function  */
    HAL_GPIO_DeInit(USARTx_TX_GPIO, USARTx_TX_Pin);
    /* Configure UART Rx as alternate function  */
    HAL_GPIO_DeInit(USARTx_RX_GPIO, USARTx_RX_Pin);

    /*##-3- Disable the NVIC for UART ##########################################*/
    HAL_NVIC_DisableIRQ(USARTx_IRQn);
  }
  static void uart_drv_timeout_handle(void)
  {
    if(HAL_UART_Receive_IT(&UartHandle, (uint8_t *)bsp_Rx_flu.dat, 1) == HAL_OK)
    {
      bsp_drv_dat.e_drv_type = BSP_DRV_LED;
      bsp_drv_dat.s_drv_state.e_led = STATE_LED_OPEN;
      bsp_drv_dat.s_pack.led_ctl.timeout = 100;
      bsp_drv_dat.s_pack.led_ctl.led_num = 1;
      bsp_drv_dat.s_pack.led_ctl.led_value[0] = 12;
      g_bsp.bsp_deal_handle(&bsp_drv_dat);
    }
    BSP_ADD_TIMER(uart_drv_timeout_handle,1000);
  }
}
BspUart::BspUart()
{
}
BspUart::~BspUart()
{
}
void BspUart::init(void)
{
    UartHandle.Instance = USART5;
    UartHandle.Init.BaudRate = 921600;
    UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    UartHandle.Init.StopBits = UART_STOPBITS_1;
    UartHandle.Init.Parity = UART_PARITY_NONE;
    UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    UartHandle.Init.Mode = UART_MODE_TX_RX;
    UART_MspDeInit(&UartHandle);
    if (HAL_UART_DeInit(&UartHandle) != HAL_OK)
    {
        //Error_Handler();
    }
    UART_MspInit(&UartHandle);
    if (HAL_UART_Init(&UartHandle) != HAL_OK)
    {
        //Error_Handler();
    }
    // if (HAL_UART_Transmit_IT(&UartHandle, (uint8_t *)"hello world\r\n", strlen("hello world\r\n")) != HAL_OK)
    // {
    //   //Error_Handler();
    // }
    //BSP_ADD_TIMER(uart_drv_timeout_handle,10);
#if ((defined USART_DEBUG) && (USART_DEBUG == __ON__))
    static uint32_t count_wake = 0;
#endif
    Dprintf(EN_LOG,TAG, "uart init successful  %d\r\n",count_wake++);
}

BspUart *BspUart::BspUart_registered(void)
{
    static BspUart t_ms_this;
    return &t_ms_this;
}