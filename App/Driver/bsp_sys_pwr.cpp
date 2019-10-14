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
        BSP_ADD_TIMER(bsp_sleep_stop, 500); //����5s�����˯��
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
    //��ʼ��RTC ��ÿs����һ��
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;        //�˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);            //�����趨������ʼ��
    GPIO_SetBits(GPIOC, GPIO_Pin_13);
    BSP_ADD_TIMER(bsp_sleep_stop, 500); //����5s�����˯��
}
