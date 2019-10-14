#include "bsp_system.h"
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "stm32f10x_it.h"

//���ڵ��Զ����
#if ((defined USART_DEBUG) && (USART_DEBUG == __ON__))
#define TAG "bsp_system: "
#define EN_LOG GREEN
#endif

//#define
void SystemClock_Config(void);
void bsp_init_dog(void);
void bsp_clear_dog(void);
void bsp_active_drv(void);//�͹��Ļ��ѹ���
void bsp_sleep_drv(void); //�͹������߹���
void bsp_delay_nus(uint32_t n);
void bsp_delay_nms(uint32_t n);
void System_init_hw(void);

void System_init_hw(void)
{
    //10ms ��tick
    NVIC_InitTypeDef NVIC_InitStructure;
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); //ѡ���ⲿʱ��  HCLK/8
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;            //����SYSTICK�ж�
    SysTick->LOAD = 10000 * 9;                            //ÿ1/delay_ostickspersec���ж�һ��
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;             //����SYSTICK
                                                          //SysTick_IRQn ����
    NVIC_InitStructure.NVIC_IRQChannel = SysTick_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        //�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);                           //����ָ���Ĳ�����ʼ��VIC�Ĵ���
}

void SystemClock_Config(void)
{
    RCC_DeInit();
    RCC_HSEConfig(RCC_HSE_ON);
    while (SUCCESS != RCC_WaitForHSEStartUp())
    {
    }
    /*����FLASH�洢����ʱʱ��������*/
    FLASH_SetLatency(FLASH_Latency_2); //FLASH_Latency_2  2��ʱ����
    /*ѡ��FLASHԤȡָ�����ģʽ*/
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable); // Ԥȡָ����ʹ��
    /*����PLLʱ��Դ����Ƶϵ��*/
    /* Fcpu = (PLL_src * PLL_MUL) = (8 Mhz / 1) * (9) = 72Mhz   */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
    /* Enable PLL */
    RCC_PLLCmd(ENABLE);
    /*���ָ����RCC��־λ(PLL׼���ñ�־)�������*/
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }
    /* Set system clock dividers */
    RCC_HCLKConfig(RCC_SYSCLK_Div1);
    RCC_PCLK2Config(RCC_HCLK_Div1);
    RCC_PCLK1Config(RCC_HCLK_Div2);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);
    /* Embedded Flash Configuration */
    FLASH_SetLatency(FLASH_Latency_2);
    FLASH_HalfCycleAccessCmd(FLASH_HalfCycleAccess_Disable);
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    /*SYSCLK configuration*/
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, DISABLE);

    __disable_irq(); //�ر��ж�
    System_init_hw();
    __enable_irq();
}

/* USER CODE BEGIN 1 */
void bsp_delay_nms(uint32_t n)
{
    uint32_t tickstart = HAL_GetTick();
    while ((HAL_GetTick() - tickstart) < n)
    {
    }
}
void bsp_delay_nus(uint32_t n)
{
    while (n--)
    {
        uint16_t t = 1;
        while (t--)
            ;
    }
}
void bsp_init_dog(void)
{
    return;
}
void bsp_clear_dog(void)
{
    return;
}
void bsp_active_drv(void)
{
    return;
}//�͹��Ļ��ѹ���

void bsp_sleep_drv(void)
{
    //SystemClock_Config();
    return;
} //�͹������߹���