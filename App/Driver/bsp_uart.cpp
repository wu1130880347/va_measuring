#include "bsp_uart.h"
#include "bsp_systimer.h"

//串口调试定义宏
#if ((defined USART_DEBUG) && (USART_DEBUG == __ON__))
#define TAG "bsp_uart: "
#define EN_LOG GREEN
#endif

extern BSP_FLU_Package_t bsp_Rx_flu;
extern "C"
{
  static void USART_Config(uint32_t BaudRate)
  {
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure; //定义串口初始化结构体
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
    //本函数（使能时钟）参数中，RCC_APB2Periph_USART1是必不可少的，有人会问，对于串口用到的PA9和
    //PA10不用使能时钟吗？其实由于USART1默认的就是PA9和PA10，所以这一个就行了，当然你要是加上
    //这个|RCC_APB2Periph_GPIOA也是不报错的，只是重复了。
    /*  USART1_TX -> PA9 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;         //选中串口默认输出管脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //定义输出最大速率
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //定义管脚9的模式
    GPIO_Init(GPIOA, &GPIO_InitStructure);            //调用函数，把结构体参数输入进行初始化
    /*  USART1_RX ->	PA10*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = BaudRate;                                      //波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     //数据位8位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          //停止位1位
    USART_InitStructure.USART_Parity = USART_Parity_No;                             //校验位 无
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 //使能接收和发送引脚

    USART_Init(USART1, &USART_InitStructure); //将以上赋完值的结构体带入库函数USART_Init进行初始化
    USART_ClearFlag(USART1, USART_FLAG_TC);
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
    USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
    USART_Cmd(USART1, ENABLE); //开启USART1，注意与上面RCC_APB2PeriphClockCmd()设置的区别
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
    USART_Config(921600);
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