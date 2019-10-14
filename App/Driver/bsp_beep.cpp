#include "bsp_beep.h"
#include "bsp_systimer.h"
//串口调试定义宏
#if ((defined USART_DEBUG) && (USART_DEBUG == __ON__))
#define TAG "bsp_beep: "
#define EN_LOG CYAN
#endif

#define FREQ_VALUE (10000000/2731 - 1) //频率2731Hz

extern "C"
{
    /* Timer handler declaration */
    static TIM_HandleTypeDef TimHandle;
    /* Timer Output Compare Configuration Structure declaration */
    static TIM_OC_InitTypeDef sConfig;
    static uint8_t once_beep_lock = 0;
    static void beep_init(void);
    static void beep_deinit(void);

    static void beep_init(void)
    {
        GPIO_InitTypeDef GPIO_InitStruct;
        __HAL_RCC_TIM2_CLK_ENABLE();

        GPIO_InitStruct.Pin = BEEP_FREQ_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF2_TIM2;
        //config lock con_a signal
        HAL_GPIO_Init(BEEP_FREQ_GPIO, &GPIO_InitStruct);

        TimHandle.Instance = TIM2;
        TimHandle.Init.Prescaler = (uint32_t)(SystemCoreClock / 10000000) - 1;
        TimHandle.Init.Period = FREQ_VALUE;//约2731
        TimHandle.Init.ClockDivision = 0;
        TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
        if (HAL_TIM_PWM_Init(&TimHandle) != HAL_OK)
        {
            /* Initialization Error */
            //Error_Handler();
        }
        sConfig.OCMode = TIM_OCMODE_PWM1;
        sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
        sConfig.OCFastMode = TIM_OCFAST_DISABLE;
        sConfig.Pulse = FREQ_VALUE/2;
        if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_3) != HAL_OK)
        {
            /* Configuration Error */
            //Error_Handler();
        }
    }
    static void beep_deinit(void)
    {
        once_beep_lock = 0;
        __HAL_RCC_TIM2_CLK_DISABLE();
        HAL_GPIO_DeInit(LOCK_A_GPIO,BEEP_FREQ_Pin);
        Dprintf(EN_LOG,TAG,"beep stop  ms\r\n");
    }
/**************************************************************
**函数原型  void beep_drv_timeout_handle(void)
**功能描述  蜂鸣器结束关闭硬件
**入口参数  NULL
**出口参数  NULL
**************************************************************/
    static void beep_drv_timeout_handle(void)
    {
        if (HAL_TIM_PWM_Stop(&TimHandle, TIM_CHANNEL_3) != HAL_OK)
        {
            /* PWM Generation Error */
            //Error_Handler();
        }
        beep_deinit();
    }
/**************************************************************
**函数原型  static void deal_beep_state(bool state,uint16_t time)
**功能描述  蜂鸣器操作接口
**入口参数  state：打开(true) 关闭(false) time 毫秒
**出口参数  NULL
**************************************************************/
    static void deal_beep_state(bool state,uint16_t time)
    {
        if(state == true)
        {
            if(once_beep_lock == 1)return;
            once_beep_lock = 1;
            beep_init();
            if (HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_3) != HAL_OK)
            {
                /* PWM Generation Error */
                //Error_Handler();
            }
            BSP_ADD_TIMER(beep_drv_timeout_handle,time);
            Dprintf(EN_LOG,TAG,"beep start %d ms\r\n",time);
        }
        else
        {  
            if (HAL_TIM_PWM_Stop(&TimHandle, TIM_CHANNEL_3) != HAL_OK)
            {
                /* PWM Generation Error */
                //Error_Handler();
            }
           beep_deinit();//直接关闭蜂鸣器
        }
    }
}
BspBeep::BspBeep()
{
}
BspBeep::~BspBeep()
{
}

BspBeep *BspBeep::BspBeep_registered(void)
{
    static BspBeep t_ms_this;
    return &t_ms_this;
}
void BspBeep::init(void)
{
    return;
}
bool BspBeep::bsp_deal_handle(BSP_Drv_Deal_t *const p_deal)
{
    deal_beep_state(p_deal->s_drv_state.e_beep,p_deal->s_pack.beep_dat.beep_timeout);
    return true;
}
/**************************************************************
**函数原型  bool BspBeep::unit_test(void)
**功能描述  蜂鸣器单元测试
**入口参数  NULL
**出口参数  bool
**************************************************************/
bool BspBeep::unit_test(void)
{
    bsp_drv_dat.e_drv_type = BSP_DRV_BEEP;
    bsp_drv_dat.s_drv_state.e_beep = true;
    bsp_drv_dat.s_pack.beep_dat.beep_timeout = 300;
    bsp_deal_handle(&bsp_drv_dat);
    Dprintf(EN_LOG,TAG,"蜂鸣器测试 0.8s\r\n");
    return true;
}
