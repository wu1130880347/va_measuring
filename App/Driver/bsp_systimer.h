#ifndef __BSP_SYSTIMER_H__
#define __BSP_SYSTIMER_H__
#include "bsp_head.h"
#include "bsp_interface.h"
#include <vector>
using namespace std;

#define BSP_ADD_TIMER(x,y) BspSysTimer::BspSysTimer_AddFun(x,y)

#pragma pack(1)
typedef struct 
{
    sys_tim_fun_t p_fun;
    uint32_t timeout;
}BSP_Sys_Timer_t;
#pragma pack()

class BspSysTimer : public BspInterfaces
{
private:
    /* data */
    BspSysTimer();
    BspSysTimer(BspSysTimer const &);
    BspSysTimer &operator=(const BspSysTimer &);
public:
    static vector<BSP_Sys_Timer_t> m_fun_timer;
    static BspSysTimer *BspSysTimer_registered(void);
    static bool BspSysTimer_AddFun(sys_tim_fun_t p_fun,uint32_t timeout);
    virtual ~BspSysTimer();
    virtual void init(void){}
    virtual bool unit_test(void){return false;}
    virtual bool bsp_events_handle(BSP_Events_Package_t const *p_pack);
    virtual bool bsp_deal_handle(BSP_Drv_Deal_t *const p_deal){ return false; }
};
#endif