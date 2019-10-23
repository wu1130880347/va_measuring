#include "bsp_flash.h"
#include "bsp_spi.h"
#include "dbg_log.h"

//串口调试定义宏
#if ((defined USART_DEBUG) && (USART_DEBUG == __ON__))
#define TAG "bsp_flash: "
#define EN_LOG YELLOW
#endif
extern "C"
{
    bool flash_write(uint32_t addr, BSP_FlashCh_Package_t *p_dat,uint8_t len);
    bool flash_read(uint32_t addr,BSP_FlashCh_Package_t *ch_dat);
    bool flash_read(uint32_t addr,BSP_FlashCh_Package_t *ch_dat)
    {
        memcpy(ch_dat,(BSP_FlashCh_Package_t *)(addr),sizeof(BSP_FlashCh_Package_t));
        return true;
    }
    bool flash_write(uint32_t addr, BSP_FlashCh_Package_t *p_dat,uint8_t len)
    {
        uint8_t para_buf[1024];
        uint8_t dat_len = sizeof(BSP_FlashCh_Package_t);
        uint32_t ch = (addr-FLASH_PARA_ADDE)/dat_len;
        memcpy(para_buf,(uint8_t *)(FLASH_PARA_ADDE),1024);
        while(len--)
        {
            memcpy(para_buf + dat_len * (ch++), (uint8_t *)p_dat, dat_len);
        }
        FLASH_Unlock(); //解锁
        FLASH_ErasePage(FLASH_PARA_ADDE);
        uint32_t *p = (uint32_t*)para_buf;
        for(uint16_t i = 0;i<256;i++)
            FLASH_ProgramWord(FLASH_PARA_ADDE+i*4,p[i]);
        FLASH_Lock(); //上锁
        return true;
    }
}

BspFlash::BspFlash()
{
}
BspFlash::~BspFlash()
{
}

BspFlash *BspFlash::BspFlash_registered(void)
{
    static BspFlash t_ms_this;
    return &t_ms_this;
}

void BspFlash::init(void)
{
    BSP_FlashCh_Package_t t_dat[10];
    flash_read(FLASH_PARA_CH_ADDR(0),t_dat);
    if (t_dat[0].check_valid != CH_KAY_VALID)
    {
        BSP_FlashCh_Package_t ref_dat;
        ref_dat.adjustment_para = 1001;
        ref_dat.standard_para = 3300;
        ref_dat.tolerance_para = 100;
        ref_dat.check_valid = CH_KAY_VALID;
        ref_dat.backup2 = 0xffffffff;
        for (uint8_t i = 0; i < 10; i++)
            memcpy((uint8_t*)(t_dat + i), (uint8_t*)&ref_dat, sizeof(BSP_FlashCh_Package_t));
    }
    else
    {
        return;
    }
    
    flash_write(FLASH_PARA_CH_ADDR(0), t_dat, 10);
}
bool BspFlash::bsp_deal_handle(BSP_Drv_Deal_t *const p_deal)
{
    return true;
}

bool BspFlash::unit_test(void)
{
    return true;
}