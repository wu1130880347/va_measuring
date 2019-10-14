#ifndef __BSP_BEEP_H__
#define __BSP_BEEP_H__
#include "bsp_head.h"
#include "bsp_interface.h"
class BspBeep : public BspInterfaces
{
private:
    BspBeep();
    BspBeep(BspBeep const &);
    BspBeep& operator = (const BspBeep &);

public:
    static BspBeep *BspBeep_registered(void);
    virtual ~BspBeep();
    virtual void init(void);
    virtual bool unit_test(void);
    virtual bool bsp_events_handle(BSP_Events_Package_t const *p_pack){ return false; }
    virtual bool bsp_deal_handle(BSP_Drv_Deal_t *const p_deal);
};
#endif