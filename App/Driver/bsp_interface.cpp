#include "bsp_interface.h"
#include "bsp_connect.h"
#include "bsp_systimer.h"
#include "bsp_sys_pwr.h"
#include "bsp_usbhid.h"
#include "bsp_tm7705.h"
extern "C"
{
#include "bsp_system.h"
}
//串口调试定义宏
#if ((defined USART_DEBUG) && (USART_DEBUG == __ON__))
#define TAG "bsp_interface: "
#define EN_LOG BLUE
#endif

SuperInerfaces *SuperInerfaces::t_pBsp = NULL;
SuperInerfaces *SuperInerfaces::t_pApp = NULL;

#define BSP_DEFAULT (BspDefault::BspDefault_registered())
BSP_FLU_Package_t bsp_Rx_flu;//读取数据flash Lora Uart 数据内存
BSP_FLU_Package_t bsp_Tx_flu;//写入数据flash Lora Uart 数据内存

BspInterfaces g_bsp;
BSP_Drv_Types_t bsp_drv_type;//驱动类型

BSP_Events_Package_t bsp_events_dat; //上报事件
BSP_Drv_Deal_t bsp_drv_dat;  //驱动数据


BspInterfaces* BspInterfaces::m_pdrv[(uint8_t)BSP_DRV_COUNTS] = {NULL};
BspInterfaces::BspInterfaces()
{
    // drivers registered
    m_pdrv[(uint8_t)BSP_DRV_USBHID] = BspUsbhid::BspUsbhid_registered();
    m_pdrv[(uint8_t)BSP_DRV_LED] = BspTm7705::BspTm7705_registered();
    m_pdrv[(uint8_t)BSP_DRV_BEEP] = BspDefault::BspDefault_registered();
    m_pdrv[(uint8_t)BSP_DRV_FLASH] = BspDefault::BspDefault_registered();
    m_pdrv[(uint8_t)BSP_DRV_LORA] = BspDefault::BspDefault_registered();
    m_pdrv[(uint8_t)BSP_DRV_CARD] = BspDefault::BspDefault_registered();
    m_pdrv[(uint8_t)BSP_DRV_RTC] = BspDefault::BspDefault_registered();
    m_pdrv[(uint8_t)BSP_DRV_UART] = BspUart::BspUart_registered();
    m_pdrv[(uint8_t)BSP_DRV_KEY] = BspDefault::BspDefault_registered();
    m_pdrv[(uint8_t)BSP_DRV_SYS_PWR] = BspSysPwr::BspSysPwr_registered();
    m_pdrv[(uint8_t)BSP_DRV_NoEvents] = BspSysTimer::BspSysTimer_registered();
    memset(&bsp_events_dat,0,sizeof(BSP_Events_Package_t));
    bsp_events_dat.bsp_events_lock = false;
    bsp_drv_type = BSP_DRV_NoEvents;
}
BspInterfaces::~BspInterfaces() 
{

}
void BspInterfaces::bsp_active(void)
{
    if(!BSP_EVENTS_NoLOCK)//有中断
    {
        BSP_Drv_Types_t temp_type = bsp_drv_type;
        bsp_active_drv();//激活MCU外设
        if (bsp_drv_type != BSP_DRV_NoEvents)
        {
            Dprintf(EN_LOG, TAG, "产生事件 %d \r\n", (uint8_t)bsp_drv_type);
        }
        m_pdrv[(uint8_t)bsp_drv_type]->bsp_events_handle(&bsp_events_dat);
        if(bsp_drv_type == temp_type)
        {
            bsp_drv_type = BSP_DRV_NoEvents;
            BSP_EVENTS_UNLOCK;
        }
    }
    return ;
}
void BspInterfaces::bsp_sleep(void)
{
    extern uint8_t sys_sleep_fg;
    if(BSP_EVENTS_NoLOCK && sys_sleep_fg)
    {
        bsp_sleep_drv(); //睡眠MCU外设
#if ((defined USART_DEBUG) && (USART_DEBUG == __ON__))
        m_pdrv[(uint8_t)BSP_DRV_UART]->init();//配置串口
#endif
    }
        return;
}
uint8_t BspInterfaces::bsp_mian(void)
{
    bsp_init_dog();
    while(1)
    {
        bsp_clear_dog();
        bsp_sleep();
        bsp_active();
    }
}

void BspInterfaces::init(void)
{ 
    SystemClock_Config();
    uint8_t t_drv = 0;
#if ((defined USART_DEBUG) && (USART_DEBUG == __ON__))
    m_pdrv[(uint8_t)BSP_DRV_UART]->init();//初始化调试串口
#endif
    while( t_drv < ((uint8_t)BSP_DRV_COUNTS)-1)
    {
        m_pdrv[t_drv++]->init();//初始化各个硬件
    }
    Dprintf(EN_LOG,TAG,"驱动配置完成 \r\n");
#if ((defined UNIT_TEST) && (UNIT_TEST == __ON__))
    t_drv = 0;
    while( (t_drv++) < ((uint8_t)BSP_DRV_COUNTS)-1)
    {
        if(m_pdrv[t_drv]->unit_test() == false)//初始化各个硬件
        {
            ;//硬件错误处理
        }
    }
    Dprintf(EN_LOG,TAG,"运行单元测试完成 \r\n");
#endif
    return ;
}

//事件处理 应用层实现
bool BspInterfaces::bsp_events_handle(BSP_Events_Package_t const *p_pack)
{
    return SuperInerfaces::bsp_events_handle(p_pack);
}
//系统操作 BSP实现
bool BspInterfaces::bsp_deal_handle(BSP_Drv_Deal_t *const p_deal)
{
    return m_pdrv[(uint8_t)(p_deal->e_drv_type)]->bsp_deal_handle(p_deal);
}