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
    static void flash_poweron(void);
    static void flash_poweroff(void);
    static uint16_t W25QXX_ReadID(void);
    static uint8_t read_status_register(void);
    static void AAI_program(uint32_t temp_addr, uint8_t *pdata, uint8_t write_len);
    static bool flash_write(uint32_t addr, BSP_FLU_Package_t *dat, uint8_t len);
    static bool flash_read(uint32_t addr, BSP_FLU_Package_t *p_dat, uint8_t len);
    static bool flash_erase_4k(uint16_t erase_4k);
    static bool flash_erase_32k(uint8_t erase_32k);
    static bool flash_erase_64k(uint8_t erase_64k);

    static void flash_poweron(void)
    {
        GPIO_InitTypeDef GPIO_InitStruct;
        GPIO_InitStruct.Pin = FLASH_POWER_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(FLASH_POWER_GPIO, &GPIO_InitStruct);
        HAL_GPIO_WritePin(FLASH_POWER_GPIO, FLASH_POWER_Pin, GPIO_PIN_RESET);
        bsp_delay_nms(100);
    }
    static void flash_poweroff(void)
    {
        HAL_GPIO_WritePin(FLASH_POWER_GPIO, FLASH_POWER_Pin, GPIO_PIN_SET);
        HAL_GPIO_DeInit(FLASH_POWER_GPIO, FLASH_POWER_Pin);
    }
    static void enable_write_flash(void)
    {
        SPI_CS_ENABLE();
        MCU_SPI_Write(0x06);
        SPI_CS_DISABLE();
    }
    static void disable_write_flash(void)
    {
        SPI_CS_ENABLE();
        MCU_SPI_Write(0x04);
        SPI_CS_DISABLE();
    }

    static uint8_t read_status_register(void)
    {
        uint8_t ret = 0;
        SPI_CS_ENABLE();
        MCU_SPI_Write(0x05);
        ret = MCU_SPI_Read();
        SPI_CS_DISABLE();
        return ret;
    }
    static void flash_wait_busy(void)
    {
        while ((read_status_register() & 0x01) == 0x01)
            ;
    }
    static uint16_t W25QXX_ReadID(void)
    {
        uint16_t Temp = 0;
        SPI_CS_ENABLE();
        MCU_SPI_Write(0x90); //发送读取ID命令
        MCU_SPI_Write(0x00);
        MCU_SPI_Write(0x00);
        MCU_SPI_Write(0x00);
        Temp |= (MCU_SPI_Read() << 8);
        Temp |= MCU_SPI_Read();
        SPI_CS_DISABLE();
        return Temp;
    }
    static bool flash_read(uint32_t addr, BSP_FLU_Package_t *p_dat, uint8_t len)
    {
        uint8_t * dat = p_dat->dat;
        if(dat == NULL || len == 0)
            return false;
        SPI_CS_ENABLE();
        MCU_SPI_Write(0x03);

        MCU_SPI_Write((uint8_t)(addr >> 16));
        MCU_SPI_Write((uint8_t)(addr >> 8));
        MCU_SPI_Write((uint8_t)addr);
        uint16_t i = 0;
        while (i < len)
        {
            *dat++ = MCU_SPI_Read();
            i++;
        }
        SPI_CS_DISABLE();
        return true;
    }

    static void AAI_program(uint32_t temp_addr, uint8_t *pdata, uint8_t write_len)
    {
        enable_write_flash();

        SPI_CS_ENABLE();
        MCU_SPI_Write(0x02);
        MCU_SPI_Write((uint8_t)(temp_addr >> 16));
        MCU_SPI_Write((uint8_t)(temp_addr >> 8));
        MCU_SPI_Write((uint8_t)temp_addr);
        for (uint16_t i = 0; i < write_len; i++)
            MCU_SPI_Write(*pdata++);
        SPI_CS_DISABLE();

        flash_wait_busy();
        disable_write_flash();
    }

    static bool flash_write(uint32_t addr, BSP_FLU_Package_t *p_dat, uint8_t len)
    {
        uint8_t * dat = p_dat->dat;
        if(dat == NULL || len == 0)
            return false;
        uint32_t temp_addr = addr;
        uint16_t temp_len = 0;
        uint16_t write_len;
        while (temp_len < len)
        {
            if (((temp_addr & 0xff) + (len - temp_len)) > 256)
            {
                write_len = 256 - (temp_addr & 0xff);
            }
            else
            {
                write_len = len - temp_len;
            }
            AAI_program(temp_addr, dat, write_len);
            temp_len += write_len;
            temp_addr += write_len;
            dat += write_len;
        }
        return true;
    }

    static bool flash_erase_4k(uint16_t erase_4k)
    {
        // 2M的话能到1024个2K
        //地址拼接
        uint8_t temp_addr1 = 0;
        uint8_t temp_addr2 = 0;

        temp_addr1 = erase_4k >> 4;

        temp_addr2 = (erase_4k << 4) & 0xf0;

        enable_write_flash();

        SPI_CS_ENABLE();
        MCU_SPI_Write(0x20);
        MCU_SPI_Write(temp_addr1);
        MCU_SPI_Write(temp_addr2);
        MCU_SPI_Write(0x00);
        SPI_CS_DISABLE();

        flash_wait_busy();
        disable_write_flash();
        return true;
    }

    static bool flash_erase_32k(uint8_t erase_32k)
    {
        // 4M的话能到128个32K
        //地址拼接
        uint8_t temp_addr1 = 0;
        uint8_t temp_addr2 = 0;

        temp_addr1 = erase_32k >> 1;

        temp_addr2 = (erase_32k & 0x01) ? 0x80 : 0;

        enable_write_flash();


        SPI_CS_ENABLE();

        MCU_SPI_Write(0x52);
        MCU_SPI_Write(temp_addr1);
        MCU_SPI_Write(temp_addr2);
        MCU_SPI_Write(0x00);

        SPI_CS_DISABLE();

        flash_wait_busy();
        disable_write_flash();
        return true;
    }

    static bool flash_erase_64k(uint8_t erase_64k)
    {
        enable_write_flash();

        SPI_CS_ENABLE();

        MCU_SPI_Write(0xD8);
        MCU_SPI_Write(erase_64k);
        MCU_SPI_Write(0x00);
        MCU_SPI_Write(0x00);
        SPI_CS_DISABLE();

        flash_wait_busy();
        disable_write_flash();
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
    flash_poweron();
    spi_init(BSP_SPI_FLASH);

    Dprintf(EN_LOG,TAG,"Temp ID = 0x%02x\r\n",W25QXX_ReadID());	
     
    //去除写保护
    SPI_CS_ENABLE();
    MCU_SPI_Write(0x50);
    MCU_SPI_Read();
    SPI_CS_DISABLE();
    bsp_delay_nms(1);
    //写状态寄存器
    SPI_CS_ENABLE();
    MCU_SPI_Write(0x01);
    MCU_SPI_Read();
    SPI_CS_DISABLE();

    //write_disable
    disable_write_flash();

    spi_deinit(BSP_SPI_FLASH);
    flash_poweroff();

    //unit_test();
}
bool BspFlash::bsp_deal_handle(BSP_Drv_Deal_t *const p_deal)
{
    if(p_deal == NULL)
        return false;

    bool ret = false;
    spi_init(BSP_SPI_FLASH);
    flash_poweron();

    switch(p_deal->s_drv_state.e_flash)
    {
        case STATE_Flash_RXD:
            ret = flash_read(p_deal->s_pack.flu_dat->t_cmd.addr,p_deal->s_pack.flu_dat,p_deal->s_pack.flu_dat->len);
            break;
        case STATE_Flash_TXD:
            ret = flash_write(p_deal->s_pack.flu_dat->t_cmd.addr,p_deal->s_pack.flu_dat,p_deal->s_pack.flu_dat->len);
            break;
        case STATE_Flash_ERASE_4K:
            ret = flash_erase_4k(p_deal->s_pack.flu_dat->t_cmd.erase_4k);
            break;
        case STATE_Flash_ERASE_32K:
            ret = flash_erase_32k(p_deal->s_pack.flu_dat->t_cmd.erase_32k);
            break;
        case STATE_Flash_ERASE_64K:
            ret = flash_erase_64k(p_deal->s_pack.flu_dat->t_cmd.erase_64k);
            break;
        default:
            ret = false;
            break;
    }

    spi_deinit(BSP_SPI_FLASH);
    flash_poweroff();
    return ret;
}

bool BspFlash::unit_test(void)
{
    flash_poweron();
    spi_init(BSP_SPI_FLASH);
    bool ret = false;
    if (0x0000 == W25QXX_ReadID() || 0xffff == W25QXX_ReadID())
    {
        Dprintf(RED, TAG, "flash test fail\r\n");
        ret = false;
    }
    else
    {
        Dprintf(EN_LOG, TAG, "flash test successful\r\n");
        ret = true;
    }

    bsp_drv_dat.e_drv_type = BSP_DRV_FLASH;
    bsp_drv_dat.s_drv_state.e_flash = STATE_Flash_TXD;
    extern BSP_FLU_Package_t bsp_Tx_flu;
    bsp_Tx_flu.t_cmd.addr = 0x00000000;
    bsp_Tx_flu.len = 2;
    bsp_Tx_flu.dat[0] = 0x56;
    bsp_Tx_flu.dat[1] = 0x78;
    bsp_drv_dat.s_pack.flu_dat = &bsp_Tx_flu;
    bsp_deal_handle(&bsp_drv_dat);
    bsp_Tx_flu.dat[0] = 0;
    bsp_Tx_flu.dat[1] = 0;
    bsp_Tx_flu.t_cmd.addr = 0x00000000;
    bsp_drv_dat.s_drv_state.e_flash = STATE_Flash_RXD;
    bsp_deal_handle(&bsp_drv_dat);
    Dprintf(EN_LOG, TAG, "0x000000 地址数据为： 0x%02x 0x%02x\r\n", bsp_Tx_flu.dat[0], bsp_Tx_flu.dat[1]);

    spi_deinit(BSP_SPI_FLASH);
    flash_poweroff();
    return ret;
}