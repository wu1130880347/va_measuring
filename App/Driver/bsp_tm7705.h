#ifndef __BSP_TM7705_H__
#define __BSP_TM7705_H__
#include "bsp_head.h"
#include "bsp_interface.h"
class BspTm7705 : public BspInterfaces
{
private:
    BspTm7705();
    BspTm7705(BspTm7705 const &);
    BspTm7705& operator = (const BspTm7705 &);

public:
    static BspTm7705 *BspTm7705_registered(void);
    virtual ~BspTm7705();
    virtual void init(void);
    virtual bool unit_test(void){return false;}
    virtual bool bsp_events_handle(BSP_Events_Package_t const *p_pack){ return false; }
    virtual bool bsp_deal_handle(BSP_Drv_Deal_t *const p_deal){ return false; }
};
#endif