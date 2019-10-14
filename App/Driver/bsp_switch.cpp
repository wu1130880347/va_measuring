#include "bsp_switch.h"
#include "bsp_systimer.h"
//串口调试定义宏
#if ((defined USART_DEBUG) && (USART_DEBUG == __ON__))
#define TAG "bsp_switch: "
#define EN_LOG BLUE
#endif

#define LOCK_PWR_TIMEOUT 500 //锁供电时间 默认2s

extern "C"
{
    static void switch_init(void);
    static void switch_deinit(void);
    static uint8_t operate_lock_fg = 0;
    static void switch_init(void)
    {
        GPIO_InitTypeDef GPIO_InitStruct;
        GPIO_InitStruct.Pin = LOCK_A_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        //config lock con_a signal
        HAL_GPIO_Init(LOCK_A_GPIO, &GPIO_InitStruct);
        HAL_GPIO_WritePin(LOCK_A_GPIO,LOCK_A_Pin,GPIO_PIN_RESET);
        //config lock con_b signal 
        GPIO_InitStruct.Pin = LOCK_B_Pin;
        HAL_GPIO_Init(LOCK_B_GPIO, &GPIO_InitStruct);
        HAL_GPIO_WritePin(LOCK_B_GPIO,LOCK_B_Pin,GPIO_PIN_RESET);
        //config lock lock pwr default poweron
        GPIO_InitStruct.Pin = LOCK_PWR_Pin;
        HAL_GPIO_Init(LOCK_PWR_GPIO, &GPIO_InitStruct);
        HAL_GPIO_WritePin(LOCK_PWR_GPIO,LOCK_PWR_Pin,GPIO_PIN_SET);
        bsp_delay_nms(50);
    }
    static void switch_deinit(void)
    {
        HAL_GPIO_WritePin(LOCK_PWR_GPIO,LOCK_PWR_Pin,GPIO_PIN_RESET);//poweroff
        HAL_GPIO_DeInit(LOCK_PWR_GPIO,LOCK_PWR_Pin);
        HAL_GPIO_DeInit(LOCK_B_GPIO,LOCK_B_Pin);
        HAL_GPIO_DeInit(LOCK_A_GPIO,LOCK_A_Pin);
        operate_lock_fg = 0;
        Dprintf(RED,TAG,"锁操作结束\r\n");
    }
    static void deal_lock_state(bool state)
    {
        if (operate_lock_fg == 0)//2s内只操作一次
        {
            Dprintf(RED,TAG,"锁操作\r\n");
            operate_lock_fg = 1;
            if (state == true)
            {
                HAL_GPIO_WritePin(LOCK_A_GPIO, LOCK_A_Pin, GPIO_PIN_SET);
                HAL_GPIO_WritePin(LOCK_B_GPIO, LOCK_B_Pin, GPIO_PIN_RESET);
            }
            else
            {
                HAL_GPIO_WritePin(LOCK_A_GPIO, LOCK_A_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(LOCK_B_GPIO, LOCK_B_Pin, GPIO_PIN_SET);
            }
        }
    }
    static void lock_drv_timeout_handle(void)
    {
        switch_deinit();
    }
}

BspSwitch::BspSwitch()
{
}
BspSwitch::~BspSwitch()
{
}
void BspSwitch::init(void)
{
    //switch_init();
    return;
}
BspSwitch *BspSwitch::BspSwitch_registered(void)
{
    static BspSwitch t_ms_this;
    return &t_ms_this;
}

bool BspSwitch::bsp_deal_handle(BSP_Drv_Deal_t *const p_deal)
{
    switch_init();
    deal_lock_state(((p_deal->s_drv_state.e_lock)==true)?true:false);
    BSP_ADD_TIMER(lock_drv_timeout_handle,LOCK_PWR_TIMEOUT);//提供通电时间
    return true;
}

bool BspSwitch::unit_test(void)
{
    bsp_drv_dat.e_drv_type = BSP_DRV_SW;
    bsp_drv_dat.s_drv_state.e_lock = true;
    bsp_deal_handle(&bsp_drv_dat);//开锁操作
    Dprintf(EN_LOG,TAG,"单元测试操作锁成功\r\n");
    return true;
}