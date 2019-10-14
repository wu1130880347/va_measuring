#include "bsp_sys_pwr.h"
#include "bsp_systimer.h"
#define RTC_ASYNCH_PREDIV    0x7C
#define RTC_SYNCH_PREDIV     0x0127

extern "C"
{
    uint8_t sys_sleep_fg = 0;
    static void bsp_sleep_stop(void)
    {
        PCout(13) ^= 1;
        BSP_ADD_TIMER(bsp_sleep_stop, 500); //开机5s后进入睡眠
    }
}

BspSysPwr::BspSysPwr()
{
}
BspSysPwr::~BspSysPwr()
{
}

BspSysPwr *BspSysPwr::BspSysPwr_registered(void)
{
    static BspSysPwr t_ms_this;
    return &t_ms_this;
}
void BspSysPwr::init(void)
{
    //初始化RTC ，每s唤醒一次
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;        //端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //IO口速度为50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);            //根据设定参数初始化
    GPIO_SetBits(GPIOC, GPIO_Pin_13);
    BSP_ADD_TIMER(bsp_sleep_stop, 500); //开机5s后进入睡眠
}
