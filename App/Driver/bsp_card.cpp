#include "bsp_card.h"
#include "bsp_spi.h"
#include "bsp_systimer.h"
#include "si522.h"
#include "reg_ctrl.h"
//串口调试定义宏
#if ((defined USART_DEBUG) && (USART_DEBUG == __ON__))
#define TAG "bsp_card: "
#define EN_LOG YELLOW
#endif

extern "C"
{
    static uint8_t check_time_count = 0;
    static void card_gpio_init(void);
    //static void card_gpio_deinit(void);
    static void card_gpio_init(void)
    {
        GPIO_InitTypeDef GPIO_InitStruct;
        GPIO_InitStruct.Pin = CARD_RC5XX_IRQ_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(CARD_RC5XX_IRQ_GPIO, &GPIO_InitStruct);
        HAL_NVIC_SetPriority(EXTI4_15_IRQn, 3, 0);
        HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

        GPIO_InitStruct.Pin = CARD_RC5XX_RST_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        HAL_GPIO_Init(CARD_RC5XX_RST_GPIO, &GPIO_InitStruct);
        HAL_GPIO_WritePin(CARD_RC5XX_RST_GPIO,CARD_RC5XX_RST_Pin,GPIO_PIN_RESET);
        bsp_delay_nms(100);
        HAL_GPIO_WritePin(CARD_RC5XX_RST_GPIO,CARD_RC5XX_RST_Pin,GPIO_PIN_SET);
    }
    // static void card_gpio_deinit(void)
    // {
    //     HAL_GPIO_DeInit(CARD_RC5XX_RST_GPIO, CARD_RC5XX_RST_Pin);
    //     HAL_GPIO_DeInit(CARD_RC5XX_IRQ_GPIO, CARD_RC5XX_IRQ_Pin);
    // }

    static uint8_t Test_Si522_MifareClassicValueBlock(BSP_Card_Package_t *card_dat)
    {
        if (!PICC_IsNewCardPresent())//寻卡
            return 1;
        // Select one of the cards
        if (!PICC_ReadCardSerial()) //选卡
            return 1;
        //以下读卡
        uint8_t t_cardNo[4] = {0};
        MIFARE_Key key = {{0xf7, 0xb3, 0x78, 0x90, 0x3d, 0x18}}; //第15扇区0块密匙

        t_cardNo[0] = uid.uidByte[3];
        t_cardNo[1] = uid.uidByte[2];
        t_cardNo[2] = uid.uidByte[1];
        t_cardNo[3] = uid.uidByte[0];
        card_dat->ulCardNO = *(uint32_t*)t_cardNo;//赋值卡内码上报

        uint8_t buffer[18];
        uint8_t size = sizeof(buffer);

        // Authenticate using key A
        StatusCode status = PCD_Authenticate(PICC_CMD_MF_AUTH_KEY_A, 60, &key, &(uid));

        if (status != STATUS_OK)
        {
            Dprintf(EN_LOG, TAG, "PCD_Authenticate() failed: %s\r\n",GetStatusCodeName(status));
            return 2;
        }
        // Show the whole sector as it currently is
        status = MIFARE_Read(60, buffer, &size);
        if (status != STATUS_OK)
        {
            Dprintf(EN_LOG, TAG, "MIFARE_Read() failed: %s\r\n",GetStatusCodeName(status));
            return 2;
        }
        memcpy(card_dat->card_dat,buffer,16);//赋值卡数据
        return 0;
    }

    static void card_drv_timeout_handle(void)
    {
        spi_init(BSP_SPI_CARD);
        BSP_Card_Package_t temp_card_dat;
        temp_card_dat.card_dat[0] = 0;
        PCD_SoftPowerUp();
        uint8_t ret = Test_Si522_MifareClassicValueBlock(&temp_card_dat);
        PCD_SoftPowerDown();
         if(ret == 0 || ret == 2)
         {
             bsp_drv_dat.e_drv_type = BSP_DRV_BEEP;
             bsp_drv_dat.s_drv_state.e_beep = true;
             bsp_drv_dat.s_pack.beep_dat.beep_timeout = 100;
             g_bsp.bsp_deal_handle(&bsp_drv_dat);
             Dprintf(EN_LOG, TAG, "cardNo : 0x%08x\r\n", temp_card_dat.ulCardNO);
             if (temp_card_dat.card_dat[0] != 0)
             {
                 Dprintf(EN_LOG, TAG, "Card dat : ");
                 for (uint8_t i = 0; i < 16; i++)
                     Dprintf(EN_LOG, "", "%02x ", temp_card_dat.card_dat[i]);
                 Dprintf(EN_LOG, "", "\r\n");
                memcpy(&bsp_events_dat.s_pack.card_msg,&temp_card_dat,sizeof(BSP_Card_Package_t));
                BSP_SEND_IRQ_EVENTS(BSP_DRV_CARD);//产生卡事件，等待应用层处理
             }
         }
         
         if(check_time_count-- != 0)
         {
             BSP_ADD_TIMER(card_drv_timeout_handle, 1000); //测试1s读一次
         }
         else
         {//重新配置并睡眠，防止电源不稳导致无法正常睡眠
             PCD_Init();
             //PCD_Si522ACDInit();
             PCD_SoftPowerDown();
         }
         spi_deinit(BSP_SPI_CARD);
    }

    void BSP_CARD_IRQ_Callback(uint16_t GPIO_Pin)
    {
    }
}

BspCard *BspCard::BspCard_registered(void)
{
    static BspCard t_ms_this;
    return &t_ms_this;
}

void BspCard::init(void)
{
    card_gpio_init();
    //BSP_ADD_TIMER(card_drv_timeout_handle,500);//测试3s读一次
    spi_init(BSP_SPI_CARD);
    PCD_Init();
    //PCD_Si522ACDInit();
    PCD_SoftPowerDown();
    spi_deinit(BSP_SPI_CARD);
    return ;
}

bool BspCard::bsp_deal_handle(BSP_Drv_Deal_t *const p_deal)
{
    switch (p_deal->s_drv_state.e_card)
    {
    case STATE_Card_RXD:
        check_time_count = p_deal->s_pack.card_msg.read_timeout;
        BSP_ADD_TIMER(card_drv_timeout_handle, 1000); //测试1s读一次
        break;
    case STATE_Card_TXD:
        break;
    default:
        break;
    }
    return true;
}

bool BspCard::bsp_events_handle(BSP_Events_Package_t const *p_pack)
{
    bsp_events_dat.e_events_type = BSP_Events_CARD;
    bsp_events_dat.s_event_state.e_card = STATE_Card_RXD;
    //BspInterfaces::bsp_events_handle(&bsp_events_dat)
    return true;
}