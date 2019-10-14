#ifndef __BSP_RTC_H__
#define __BSP_RTC_H__
#include "bsp_head.h"
#include "bsp_interface.h"
class BspRtc : public BspInterfaces
{
private:
    BspRtc();
    BspRtc(BspRtc const &);
    BspRtc& operator = (const BspRtc &);

public:
    static BspRtc *BspRtc_registered(void);
    virtual ~BspRtc();
    virtual void init(void);
    virtual bool unit_test(void);
    virtual bool bsp_events_handle(BSP_Events_Package_t const *p_pack){ return false; }
    virtual bool bsp_deal_handle(BSP_Drv_Deal_t *const p_deal);
};
#endif