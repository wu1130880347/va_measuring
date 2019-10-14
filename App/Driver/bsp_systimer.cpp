#include "bsp_systimer.h"
#include "bsp_sys_pwr.h"
#include "stm32f10x_it.h"
#define ENABLE_SYS_TICK sys_sleep_fg = 0;
#define DISABLE_SYS_TICK sys_sleep_fg = 1;

extern uint8_t sys_sleep_fg;
static uint32_t pre_sys_tick = 0;
#define GET_SYS_TICK  (HAL_GetTick()-pre_sys_tick)

void BSP_SYS_TIMER_Callback(void)
{
    BSP_SEND_IRQ_EVENTS(BSP_DRV_NoEvents); //处理定时器唤醒事件
}

vector<BSP_Sys_Timer_t> BspSysTimer::m_fun_timer;

BspSysTimer::BspSysTimer()
{
}
BspSysTimer::~BspSysTimer()
{
}
BspSysTimer *BspSysTimer::BspSysTimer_registered(void)
{
    static BspSysTimer t_ms_this;
    return &t_ms_this;
}

bool BspSysTimer::BspSysTimer_AddFun(sys_tim_fun_t p_fun,uint32_t timeout)
{
    BSP_Sys_Timer_t t_fun_s;
    t_fun_s.p_fun = p_fun;
    t_fun_s.timeout = (timeout<1)?1:timeout;//不能小于 1ms
    uint16_t num = m_fun_timer.size();
    if (num > 0)
    {
        vector<BSP_Sys_Timer_t>::iterator iter = m_fun_timer.begin();
        while (num--)
        {
            if (iter->p_fun == t_fun_s.p_fun)
            {
                iter->timeout = t_fun_s.timeout;
                return true;
            }
            ++iter;
        }
    }
    m_fun_timer.push_back(t_fun_s);
    ENABLE_SYS_TICK;
    return true;
}

bool BspSysTimer::bsp_events_handle(BSP_Events_Package_t const *p_pack)
{
    uint16_t num = m_fun_timer.size();
    if (num > 0)
    {
        vector<BSP_Sys_Timer_t>::iterator iter = m_fun_timer.begin();
        while (num--)
        {
            iter->timeout = (iter->timeout>GET_SYS_TICK)?iter->timeout:GET_SYS_TICK;
            iter->timeout -= GET_SYS_TICK;
            if(iter->timeout == 0)
            {
                (iter)->p_fun();//此调用可能更新timeout的值
                if(iter->timeout == 0)
                  m_fun_timer.erase(iter);
                pre_sys_tick = HAL_GetTick();
                return true;
            }
            ++iter;
        }
    }
    else
    {
        DISABLE_SYS_TICK;
    }
    pre_sys_tick = HAL_GetTick();
    return true;
}