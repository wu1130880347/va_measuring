#ifndef __BSP_USBHID_H__
#define __BSP_USBHID_H__
#include "bsp_head.h"
#include "bsp_interface.h"
class BspUsbhid : public BspInterfaces
{
private:
    BspUsbhid();
    BspUsbhid(BspUsbhid const &);
    BspUsbhid& operator = (const BspUsbhid &);

public:
    static BspUsbhid *BspUsbhid_registered(void);
    virtual ~BspUsbhid();
    virtual void init(void);
    virtual bool unit_test(void){return false;}
    virtual bool bsp_events_handle(BSP_Events_Package_t const *p_pack){ return false; }
    virtual bool bsp_deal_handle(BSP_Drv_Deal_t *const p_deal){ return false; }
};
#endif