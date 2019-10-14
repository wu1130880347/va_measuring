#ifndef __BSP_KEY_H__
#define __BSP_KEY_H__
#include "bsp_head.h"
#include "bsp_interface.h"
class BspKey : public BspInterfaces
{
private:
    BspKey();
    BspKey(BspKey const &);
    BspKey& operator = (const BspKey &);

public:
    static BspKey *BspKey_registered(void);
    virtual ~BspKey();
    virtual void init(void);
    virtual bool unit_test(void);
    virtual bool bsp_events_handle(BSP_Events_Package_t const *p_pack);
    virtual bool bsp_deal_handle(BSP_Drv_Deal_t *const p_deal) { return false; }
};
#endif