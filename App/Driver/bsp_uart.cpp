#include "bsp_uart.h"
#include "bsp_systimer.h"

//���ڵ��Զ����
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
    USART_InitTypeDef USART_InitStructure; //���崮�ڳ�ʼ���ṹ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
    //��������ʹ��ʱ�ӣ������У�RCC_APB2Periph_USART1�Ǳز����ٵģ����˻��ʣ����ڴ����õ���PA9��
    //PA10����ʹ��ʱ������ʵ����USART1Ĭ�ϵľ���PA9��PA10��������һ�������ˣ���Ȼ��Ҫ�Ǽ���
    //���|RCC_APB2Periph_GPIOAҲ�ǲ�����ģ�ֻ���ظ��ˡ�
    /*  USART1_TX -> PA9 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;         //ѡ�д���Ĭ������ܽ�
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //��������������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //����ܽ�9��ģʽ
    GPIO_Init(GPIOA, &GPIO_InitStructure);            //���ú������ѽṹ�����������г�ʼ��
    /*  USART1_RX ->	PA10*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = BaudRate;                                      //������
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     //����λ8λ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          //ֹͣλ1λ
    USART_InitStructure.USART_Parity = USART_Parity_No;                             //У��λ ��
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 //ʹ�ܽ��պͷ�������

    USART_Init(USART1, &USART_InitStructure); //�����ϸ���ֵ�Ľṹ�����⺯��USART_Init���г�ʼ��
    USART_ClearFlag(USART1, USART_FLAG_TC);
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
    USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
    USART_Cmd(USART1, ENABLE); //����USART1��ע��������RCC_APB2PeriphClockCmd()���õ�����
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