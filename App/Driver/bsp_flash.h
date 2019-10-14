#ifndef __BSP_FLASH_H__
#define __BSP_FLASH_H__
#include "bsp_head.h"
#include "bsp_interface.h"
class BspFlash : public BspInterfaces
{
private:
    BspFlash();
    BspFlash(BspFlash const &);
    BspFlash& operator = (const BspFlash &);

public:
    static BspFlash *BspFlash_registered(void);
    virtual ~BspFlash();
    virtual void init(void);
    virtual bool unit_test(void);
    virtual bool bsp_events_handle(BSP_Events_Package_t const *p_pack){ return false; }
    virtual bool bsp_deal_handle(BSP_Drv_Deal_t *const p_deal);
};
#endif