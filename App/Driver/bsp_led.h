#ifndef __BSP_LED_H__
#define __BSP_LED_H__
#include "bsp_head.h"
#include "bsp_interface.h"
class BspLed : public BspInterfaces
{
private:
    BspLed();
    BspLed(BspLed const &);
    BspLed& operator = (const BspLed &);

public:
    static BspLed *BspLed_registered(void);
    virtual ~BspLed();
    virtual void init(void);
    virtual bool unit_test(void);
    virtual bool bsp_events_handle(BSP_Events_Package_t const *p_pack){ return false; }
    virtual bool bsp_deal_handle(BSP_Drv_Deal_t *const p_deal);
};
#endif