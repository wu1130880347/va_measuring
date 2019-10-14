#ifndef __BSP_LORA_H__
#define __BSP_LORA_H__
#include "bsp_head.h"
#include "bsp_interface.h"
class BspLora : public BspInterfaces
{
private:
    BspLora();
    BspLora(BspLora const &);
    BspLora& operator = (const BspLora &);

public:
    static BspLora *BspLora_registered(void);
    virtual ~BspLora();
    virtual void init(void);
    virtual bool unit_test(void){return false;}
    virtual bool bsp_events_handle(BSP_Events_Package_t const *p_pack);
    virtual bool bsp_deal_handle(BSP_Drv_Deal_t *const p_deal);
};
#endif