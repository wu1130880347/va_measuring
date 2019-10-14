#ifndef __BSP_SYS_PWR_H__
#define __BSP_SYS_PWR_H__
#include "bsp_head.h"
#include "bsp_interface.h"
//extern uint8_t sys_sleep_fg;
class BspSysPwr : public BspInterfaces
{
private:
    BspSysPwr();
    BspSysPwr(BspSysPwr const &);
    BspSysPwr& operator = (const BspSysPwr &);

public:
    static BspSysPwr *BspSysPwr_registered(void);
    virtual ~BspSysPwr();
    virtual void init(void);
    virtual bool unit_test(void){return false;}
    virtual bool bsp_events_handle(BSP_Events_Package_t const *p_pack){ return false; }
    virtual bool bsp_deal_handle(BSP_Drv_Deal_t *const p_deal) { return false; }
};
#endif