#ifndef __BSP_SWITCH_H__
#define __BSP_SWITCH_H__
#include "bsp_head.h"
#include "bsp_interface.h"
class BspSwitch : public BspInterfaces
{
private:
    BspSwitch();
    BspSwitch(BspSwitch const &);
    BspSwitch& operator = (const BspSwitch &);

public:
    static BspSwitch *BspSwitch_registered(void);
    virtual ~BspSwitch();
    virtual void init(void);
    virtual bool unit_test(void);
    virtual bool bsp_events_handle(BSP_Events_Package_t const *p_pack){ return false; }
    virtual bool bsp_deal_handle(BSP_Drv_Deal_t *const p_deal);
};
#endif