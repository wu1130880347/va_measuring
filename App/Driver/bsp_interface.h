#ifndef __BSP_INTERFACE_H__
#define __BSP_INTERFACE_H__
#include "bsp_head.h"
#include "bsp_super_interface.h"
#include "bsp_system.h"

#define BSP_EVENTS_LOCK (bsp_events_dat.bsp_events_lock = true)
#define BSP_EVENTS_UNLOCK (bsp_events_dat.bsp_events_lock = false)
#define BSP_EVENTS_NoLOCK (bsp_events_dat.bsp_events_lock == false)

#define BSP_SEND_IRQ_EVENTS(x) do{if(BSP_EVENTS_NoLOCK||(bsp_drv_type == BSP_DRV_NoEvents))bsp_drv_type = x;\
                                BSP_EVENTS_LOCK;}while(0);

class BspInterfaces;
extern BspInterfaces g_bsp;
extern BSP_Events_Package_t bsp_events_dat;
extern BSP_Drv_Types_t bsp_drv_type;
extern BSP_Drv_Deal_t bsp_drv_dat;

class BspInterfaces : public SuperInerfaces
{
private:
    static BspInterfaces *(m_pdrv[(uint8_t)BSP_DRV_COUNTS]);
    void bsp_active(void);
    void bsp_sleep(void);
public:
    BspInterfaces();
    uint8_t bsp_mian(void);
    virtual ~BspInterfaces();
    virtual void init(void);
    virtual bool unit_test(void){return true;}
    virtual bool bsp_events_handle(BSP_Events_Package_t const *p_pack);//上报事件接口
    virtual bool bsp_deal_handle(BSP_Drv_Deal_t *const p_deal);//处理系统操作
};
//default registered
class BspDefault : public BspInterfaces
{
private:
    BspDefault() {}
    BspDefault(BspDefault const &t_this) {}
    BspDefault &operator=(BspDefault &t_this) { return t_this; }

public:
    static BspDefault *BspDefault_registered(void)
    {
        static BspDefault t_ms_this;
        return &t_ms_this;
    }
    virtual ~BspDefault() {}
    virtual void init(void) {}
    virtual bool unit_test(void){return false;}
    virtual bool bsp_events_handle(BSP_Events_Package_t const *p_pack) { return false; }
    virtual bool bsp_deal_handle(BSP_Drv_Deal_t *const p_deal) { return false; }
};
#endif