#include "bsp_system.h"
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "stm32f10x_it.h"

//串口调试定义宏
#if ((defined USART_DEBUG) && (USART_DEBUG == __ON__))
#define TAG "bsp_system: "
#define EN_LOG GREEN
#endif

//#define
void SystemClock_Config(void);
void bsp_init_dog(void);
void bsp_clear_dog(void);
void bsp_active_drv(void);//低功耗唤醒管理
void bsp_sleep_drv(void); //低功耗休眠管理
void bsp_delay_nus(uint32_t n);
void bsp_delay_nms(uint32_t n);
void System_init_hw(void);

void System_init_hw(void)
{
    //10ms 的tick
    NVIC_InitTypeDef NVIC_InitStructure;
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); //选择外部时钟  HCLK/8
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;            //开启SYSTICK中断
    SysTick->LOAD = 10000 * 9;                            //每1/delay_ostickspersec秒中断一次
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;             //开启SYSTICK
                                                          //SysTick_IRQn 配置
    NVIC_InitStructure.NVIC_IRQChannel = SysTick_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);                           //根据指定的参数初始化VIC寄存器
}

void SystemClock_Config(void)
{
    RCC_DeInit();
    RCC_HSEConfig(RCC_HSE_ON);
    while (SUCCESS != RCC_WaitForHSEStartUp())
    {
    }
    /*设置FLASH存储器延时时钟周期数*/
    FLASH_SetLatency(FLASH_Latency_2); //FLASH_Latency_2  2延时周期
    /*选择FLASH预取指缓存的模式*/
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable); // 预取指缓存使能
    /*设置PLL时钟源及倍频系数*/
    /* Fcpu = (PLL_src * PLL_MUL) = (8 Mhz / 1) * (9) = 72Mhz   */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
    /* Enable PLL */
    RCC_PLLCmd(ENABLE);
    /*检查指定的RCC标志位(PLL准备好标志)设置与否*/
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

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置中断优先级分组为组2：2位抢占优先级，2位响应优先级

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, DISABLE);

    __disable_irq(); //关闭中断
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
}//低功耗唤醒管理

void bsp_sleep_drv(void)
{
    //SystemClock_Config();
    return;
} //低功耗休眠管理