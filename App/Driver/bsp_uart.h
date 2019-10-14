#ifndef __BSP_UART_H__
#define __BSP_UART_H__
#include "bsp_head.h"
#include "bsp_interface.h"
class BspUart : public BspInterfaces
{
private:
    BspUart();
    BspUart(BspUart const &);
    BspUart& operator = (const BspUart &);

public:
    static BspUart *BspUart_registered(void);
    virtual ~BspUart();
    virtual void init(void);
    virtual bool unit_test(void){return false;}
    virtual bool bsp_events_handle(BSP_Events_Package_t const *p_pack){ return false; }
    virtual bool bsp_deal_handle(BSP_Drv_Deal_t *const p_deal) { return false; }
};
#endif