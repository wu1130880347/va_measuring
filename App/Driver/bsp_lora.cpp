#include "bsp_lora.h"
#include "bsp_switch.h"
#include "bsp_spi.h"
#include "lora_interface.h"
#include "bsp_systimer.h"

//串口调试定义宏
#if ((defined USART_DEBUG) && (USART_DEBUG == __ON__))
#define TAG "bsp_lora: "
#define EN_LOG GREEN
#endif

extern "C"
{
    static uint8_t check_lora_air = 0;
    static uint8_t irq_deal_fg = 0;//0x01 开启接收 
    static void lora_rece_handle(void);
    
     static void lora_drv_timeout_handle(void)
    {
        spi_init(BSP_SPI_LORA);
        Dprintf(EN_LOG, "", "\033[34m Lora 信道活跃检测 !!!\033[0m\r\n");
        lora_cad_enable();
        spi_deinit(BSP_SPI_LORA);
    }
    static void lora_active_rece_handle(void)
    {
        static uint8_t timecount = 0;
        if (irq_deal_fg & 0x02)
            return;
        if(timecount++ >= 40)
        {
          timecount = 0;
          return ;
        }
        lora_drv_timeout_handle();
        Dprintf(EN_LOG, "", "\033[34m 100ms探测一次加快接收速度\033[0m\r\n");
        BSP_ADD_TIMER(lora_active_rece_handle, 100); //100ms探测一次加快接收速度
    }
    void lora_irq_deal(void)
    {
        spi_init(BSP_SPI_LORA);
         Dprintf(EN_LOG,"","\033[35m Lora 产生中断事件 !!!\033[0m\r\n");
         uint32_t ret = lora_Process();
         if (ret == 8)
         {
             Dprintf(EN_LOG, "", "\033[35m 中断唤醒lora模块准备接收 \033[0m\r\n");
             irq_deal_fg |= 0x01;
             BSP_ADD_TIMER(lora_rece_handle,0);    //20ms每次等待接收 最长10s
             BSP_ADD_TIMER(lora_active_rece_handle,100);    //20ms每次等待接收 最长10s
         }
         else if (ret == 9)
         {
             Dprintf(EN_LOG, "", "\033[35m Lora 中断没有收到信号 !!!\033[0m\r\n");
         }
         else
         {
             Dprintf(EN_LOG, "", "\033[35m Lora 未知 %d  !!!\033[0m\r\n",ret);
         }
        lora_set_sleep();
         spi_deinit(BSP_SPI_LORA);
        //每两秒检测空中两下
         if (check_lora_air == 0 && !(irq_deal_fg&0x01))
         {
             check_lora_air = 1;
             BSP_ADD_TIMER(lora_drv_timeout_handle, 0);
             Dprintf(EN_LOG, "", "\033[35m 第二次活跃检测 !!!\033[0m\r\n");
         }
    }
    void BSP_LORA_IRQ_Callback(uint16_t GPIO_Pin)
    {
        if(irq_deal_fg == 0)
            BSP_ADD_TIMER(lora_irq_deal,0);//后台处理
        return ;
    }
    static void lora_getRssiAndSnr(void)
    {
#if ((defined USART_DEBUG) && (USART_DEBUG == __ON__))
        double ret_rssi = 0;
        char ret_snr = 0;
        ret_snr = SX1276LoRaGetPacketSnr();
        ret_rssi = SX1276LoRaGetPacketRssi();
        Dprintf(EN_LOG,"","\033[35mret_rssi = \033[31m%3.2f \033[35mret_snr = \033[31m%d\r\n\033[0m", ret_rssi, ret_snr);
#endif
    }
    static uint8_t lora_SendPackage(uint8_t *pData, uint16_t len)
    {
        if (len > 128)
        {
            Dprintf(EN_LOG,"","\033[32m包过长 ==>> 发送失败\r\n\033[0m");
            return 1;
        }
        lora_sendData(pData, len);
        Dprintf(EN_LOG,"","\033[33mlora Send:len = %d \033[0m\r\n", len);
        for (uint16_t i = 0; i < len; i++)
            Dprintf(EN_LOG,"","\033[31m%02x \033[0m", *(pData + i));
        Dprintf(EN_LOG,"","\r\n");
        return 0;
    }
    static uint8_t lora_RecePackage(uint8_t *Buffer, uint16_t *BufferSize)
    {
        lora_getRssiAndSnr();
        lora_receData(Buffer, BufferSize);
        Dprintf(EN_LOG,"","\033[34m通道 \033[36m%d\033[34m lora rece：\033[0m\r\n", 0);
        // for (u16 i = 6; i < 14; i++)
        //     DDprintf(EN_LOG,"","\033[36m%x%x\033[0m", (*(Buffer + i)>>4 > 0 )?*(Buffer + i)>>4:0,(*(Buffer + i)&0x0f));
        for (uint16_t i = 0; i < *BufferSize; i++)
            Dprintf(EN_LOG,"","\033[36m%02x \033[0m", *(Buffer + i));
        Dprintf(EN_LOG,"","\r\n");
        Dprintf(EN_LOG,"","\033[33mRec:len = %d \033[0m\r\n", *BufferSize);
        return 0;
    }

    //mcu唤醒后读空中两次
    void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
    {
        /* Clear Wake Up Flag */
        check_lora_air = 0;
        if (!(irq_deal_fg & 0x01))
        {
            BSP_ADD_TIMER(lora_drv_timeout_handle, 0); //每秒检测一次空中状态
        }
        __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
    }

    static void lora_rece_handle(void)
    {
        static uint16_t rece_timeout = 0;
        spi_init(BSP_SPI_LORA);
        uint32_t ret = lora_Process();
        if(ret == 1)//收到数据
        {
            irq_deal_fg |= 0x02;
            uint16_t tp_len = bsp_Rx_flu.len;
            lora_RecePackage(bsp_Rx_flu.dat,&tp_len);
            bsp_Rx_flu.len = tp_len;
            BSP_SEND_IRQ_EVENTS(BSP_DRV_LORA);//产生数据接收事件，等待应用层处理
            rece_timeout = 0;
        }
        else if(ret == 2)
        {
            Dprintf(EN_LOG, "", "\033[35m Lora 发送完成\033[0m\r\n");
        }

        if(rece_timeout++ >= 500)//十秒后解除接收，进入低功耗
        {
            irq_deal_fg = 0;
            rece_timeout = 0;
            Dprintf(EN_LOG, "", "\033[35m Lora连续接收解除\033[0m\r\n");
            return ;
        }
        BSP_ADD_TIMER(lora_rece_handle,20);    //20ms每次等待接收 最长10s
        spi_deinit(BSP_SPI_LORA);
    }
    static uint8_t lora_setPara(uint32_t freq)
    {
        if (freq <= 410 || freq >= 525)
            freq = 1; //使用默认470Mhz 475Mhz 480Mhz  485Mhz
        lora_para.RFFrequency = (freq == 1) ? (LoRa_FREQENCY) : (freq * 1000000);
        lora_para.Power = LoRaSettings.Power;
        lora_para.SignalBw = LoRaSettings.SignalBw;
        lora_para.SpreadingFactor = LoRaSettings.SpreadingFactor;
        lora_para.ErrorCoding = LoRaSettings.ErrorCoding;
        lora_para.CrcOn = LoRaSettings.CrcOn;
        lora_para.ImplicitHeaderOn = LoRaSettings.ImplicitHeaderOn;
        lora_para.RxSingleOn = LoRaSettings.RxSingleOn;
        lora_para.FreqHopOn = LoRaSettings.FreqHopOn;
        lora_para.HopPeriod = LoRaSettings.HopPeriod;
        lora_para.TxPacketTimeout = LoRaSettings.TxPacketTimeout;
        lora_para.RxPacketTimeout = LoRaSettings.RxPacketTimeout;
        lora_para.PayloadLength = LoRaSettings.PayloadLength;
        memcpy((uint8_t *)&LoRaSettings, (uint8_t *)&lora_para, sizeof(lora_para));
        Dprintf(EN_LOG,"","\033[32m通道 %d  初始化频率 %d Mhz 成功\r\n\033[0m", 0, lora_para.RFFrequency / 1000000);
        lora_init();
        return 0;
    }
    // void lora_test_timeout(void)
    // {
    //     uint8_t dat[] = {0x49,0x8a,0x5e,0xb4,0x27,0x00,0xff,0xff,0x00,0x01,0x02,0x03,0x41,0x92,0x03,0x00,0x03,0x00,0x00,0x00,0x02,0x00,0x6e,0x00,0x02,0x03,0x01,0x13,0x19,0x09,0x13,0x00,0x01,0x26,0x7b,0xd4,0x08,0x50,0x15,0x6f,0x2b,0x77,0x0e,0xbe,0x6f };
    //     lora_SendPackage(dat, sizeof(dat));
    //     //BSP_ADD_TIMER(lora_test_timeout,3000);//测试500ms读一次
    // }
}

BspLora::BspLora()
{
}
BspLora::~BspLora()
{
}

BspLora *BspLora::BspLora_registered(void)
{
    static BspLora t_ms_this;
    return &t_ms_this;
}

void BspLora::init(void)
{
    spi_init(BSP_SPI_LORA);
    uint8_t ret = lora_get_ICVersion();
    Dprintf(EN_LOG,"","lora_get_ICVersion = 0x%02x\r\n",ret);
    if(ret == 0x12)
    {
        lora_setPara(495);
        lora_set_sleep();
        //BSP_ADD_TIMER(lora_drv_timeout_handle,1000);//每秒检测一次空中状态
    }
    spi_deinit(BSP_SPI_LORA);
}

bool BspLora::bsp_deal_handle(BSP_Drv_Deal_t *const p_deal)
{
    uint8_t ret = 0;
    BSP_FLU_Package_t *tp_dat = NULL;
    switch(p_deal->s_drv_state.e_lora)
    {
    case STATE_LORA_TXD:
        tp_dat = p_deal->s_pack.flu_dat;
        ret = lora_SendPackage(tp_dat->dat,p_deal->s_pack.flu_dat->len);
        break;
    default:
        break;
    }
    return (ret == 0)?true:false;
}
bool BspLora::bsp_events_handle(BSP_Events_Package_t const *p_pack)
{
    //用于测试，收到数据#号键绿灯闪
    bsp_drv_dat.e_drv_type = BSP_DRV_LED;
    bsp_drv_dat.s_drv_state.e_led = STATE_LED_OPEN;
    bsp_drv_dat.s_pack.led_ctl.timeout = 10;
    bsp_drv_dat.s_pack.led_ctl.led_num = 1;
    bsp_drv_dat.s_pack.led_ctl.led_value[0] = 12;
    g_bsp.bsp_deal_handle(&bsp_drv_dat);
    // bsp_events_dat.e_events_type = BSP_Events_LORA_RxDone;
    // bsp_events_dat.s_event_state.e_lora = STATE_LORA_RXD;
    // bsp_events_dat.s_pack.flu_dat = &bsp_Rx_flu;
    //BspInterfaces::bsp_events_handle(&bsp_events_dat);//应用层处理
    return true;
}