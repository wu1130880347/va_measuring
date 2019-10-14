#ifndef __BSP_SUPER_INTERFACE_H__
#define __BSP_SUPER_INTERFACE_H__
#include "bsp_head.h"

class SuperInerfaces
{
private:
    static SuperInerfaces *t_pBsp;
    static SuperInerfaces *t_pApp;
    
public:
    static void SuperInerfaces_init(SuperInerfaces *t_pthis, uint8_t init_fg) //init_fg 0初始化BSP;1初始化APP
    {
        static uint8_t once_fg = 0x03;
        if (once_fg == 0)
            return;
        if (init_fg == 0)
        {
            t_pBsp = t_pthis;
            t_pBsp->init();
            once_fg &= ~0x01;
            return;
        }
        if (init_fg == 1)
        {
            t_pApp = t_pthis;
            t_pApp->init();
            once_fg &= ~0x02;
            return;
        }
    }
    virtual void init(void) = 0; //虚基类，减少内存开销
    virtual bool bsp_events_handle(BSP_Events_Package_t const *p_pack)
    {
        return t_pApp->bsp_events_handle(p_pack);
    }
    virtual bool bsp_deal_handle(BSP_Drv_Deal_t *const p_deal)
    {
        return t_pBsp->bsp_deal_handle(p_deal);
    }
};
#endif