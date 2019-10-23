#ifndef __BSP_FLASH_H__
#define __BSP_FLASH_H__
#include "bsp_head.h"
#include "bsp_interface.h"

//FLASH ʹ�� ��64ҳΪ����ҳ
#define FLASH_BASE_ADDR 0x08000000
#define FLASH_PARA_ADDE (FLASH_BASE_ADDR + 0x400 * 63)
#define FLASH_PARA_CH_ADDR(x) (FLASH_PARA_ADDE + 20*x) //ÿͨ��20Byte  4byte ��׼ֵ  4byte���  4byte ϵ�� 4byte��Ч�� ����

#define CH_KAY_VALID 0xaabb5544

#pragma pack(1)
typedef struct 
{
    uint32_t standard_para;
    uint32_t tolerance_para;
    uint32_t adjustment_para;
    uint32_t check_valid;
    uint32_t backup2;
}BSP_FlashCh_Package_t;
#pragma pack()

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