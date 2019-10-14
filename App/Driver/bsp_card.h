#ifndef __BSP_CARD_H__
#define __BSP_CARD_H__
#include "bsp_head.h"
#include "bsp_interface.h"
class BspCard : public BspInterfaces
{
private:
    BspCard(){}
    BspCard(BspCard const &);
    BspCard& operator = (const BspCard &);

public:
    static BspCard *BspCard_registered(void);
    virtual ~BspCard(){}
    virtual void init(void);
    virtual bool unit_test(void){return false;}
    virtual bool bsp_events_handle(BSP_Events_Package_t const *p_pack);
    virtual bool bsp_deal_handle(BSP_Drv_Deal_t *const p_deal);
};
#endif