#include "bsp_key.h"
#include "bsp_systimer.h"
#include "bsp_i2c.h"

//串口调试定义宏
#if ((defined USART_DEBUG) && (USART_DEBUG == __ON__))
#define TAG "bsp_key: "
#define EN_LOG GREEN
#endif

extern "C"
{
    static BSP_Key_Package_t pre_key_value;//保存上一次按键的值
    static uint8_t key_deal_fg = 0;
    static uint32_t touch_buffer = 0;
/* USER CODE BEGIN 0 */
#define CH_SENS(a, b) 0x##a##b
    /* TSM12M Register Map */
    typedef enum
    {
        SENS1 = 0x02, //Sensitivity, Channel 2 and 1
        SENS2 = 0x03, //Channel 4 and 3
        SENS3 = 0x04, //Channel 6 and 5
        SENS4 = 0x05, //Channel 8 and 7
        SENS5 = 0x06, //Channel 10 and 9
        SENS6 = 0x07, //Channel 12 and 11
        CTRL1 = 0x08,
        CTRL2 = 0x09,
        REF_RST1 = 0x0A,
        REF_RST2 = 0x0B,
        CH_HOLD1 = 0x0C,
        CH_HOLD2 = 0x0D,
        CAL_HOLD1 = 0x0E,
        CAL_HOLD2 = 0x0F,
        OUTPUT1 = 0x10,
        OUTPUT2 = 0x11,
        OUTPUT3 = 0x12,
    } TSM_RegMapAddr_TypeDef;
    /* USER CODE END 0 */
    /*************************************** 
     * TSM12 DRIVE CODE BEGIN
    ***************************************/
    /* TSM12M Devices Address ： 0xD0*/

    static uint8_t TSM_Read_Byte(uint8_t REG_Address)
    {
        uint8_t Data = 0;
        Data = I2C_Read_One_Byte(0xD0,REG_Address);
        //HAL_I2C_Master_Transmit(&hi2c1, 0xD0, &REG_Address, 1, 0x10);
        //HAL_I2C_Master_Receive(&hi2c1, 0xD1, &Data, 1, 0x10);
        //Dprintf(EN_LOG,TAG,"data %02x\r\n",Data);
        return Data;
    }
    static void TSM_Write_Byte(uint8_t REG_Address, uint8_t Data)
    {
        I2C_Write_One_Byte(0xD0,REG_Address,Data);
        // if(TSM_Read_Byte(REG_Address) == Data)
        // {Dprintf(EN_LOG,TAG,"reg = 0x%02x data %02x OK\r\n",REG_Address,Data);}
        // else
        // {Dprintf(EN_LOG,TAG,"reg = 0x%02x data %02x Fail\r\n",REG_Address,Data);}
        //HAL_I2C_Mem_Write(&hi2c1, 0xD0, REG_Address, 1, &Data, 1, 0x10);
    }



    // static void TSM_Read_MutiByte(uint8_t REG_Address, uint8_t *Buffer, uint8_t Length)
    // {
    //     HAL_I2C_Master_Transmit(&hi2c1, 0xD0, &REG_Address, 1, 0x10);
    //     HAL_I2C_Master_Receive(&hi2c1, 0xD1, Buffer, Length, 0x10);
    // }

    static uint8_t TSM12_Check(void) //Write Register And Read Register
    {
        HAL_GPIO_WritePin(CAP_EN_GPIO, CAP_EN_Pin, GPIO_PIN_RESET);
        bsp_delay_nms(10);

        TSM_Write_Byte(0x0E, 0x00);
        if (TSM_Read_Byte(0x0E) == 0x00)
        {
            HAL_GPIO_WritePin(CAP_EN_GPIO, CAP_EN_Pin, GPIO_PIN_SET);
            return 0; //Check Success
        }
        HAL_GPIO_WritePin(CAP_EN_GPIO, CAP_EN_Pin, GPIO_PIN_SET);
        return 1; //Check Error
    }

    static void TSM12_Init(void)
    {
        /* TSM12_Init */
        /* POWER ON */
        HAL_GPIO_WritePin(CAP_EN_GPIO, CAP_EN_Pin, GPIO_PIN_RESET);
        bsp_delay_nms(10);

        /* H/W Reset, Enable Software Reset */
        TSM_Write_Byte(CTRL2, 0x0F); //Enable Software Reset
        TSM_Write_Byte(CTRL2, 0x07); //Enable Sleep Mode

        /* Sensitivity setting , 0x00 is Highst Sensitivity 0~7 Adjusted */
        TSM_Write_Byte(REF_RST1, 0xFF);
        TSM_Write_Byte(REF_RST2, 0x0F);
        TSM_Write_Byte(CH_HOLD1, 0xFF);
        TSM_Write_Byte(CH_HOLD2, 0x0F);

        TSM_Write_Byte(SENS1, CH_SENS(0, 0));
        TSM_Write_Byte(SENS2, CH_SENS(0, 0));
        TSM_Write_Byte(SENS3, CH_SENS(0, 0));
        TSM_Write_Byte(SENS4, CH_SENS(0, 0));
        TSM_Write_Byte(SENS5, CH_SENS(0, 0));
        TSM_Write_Byte(SENS6, CH_SENS(0, 0));

        /* General Control Register1 setting */
        TSM_Write_Byte(CTRL1, 0x0B); //1:Fast Mode; 
        //2:Interrupt is on Low or Middle or high output;3 : Response Time Control

        TSM_Write_Byte(REF_RST1, 0x00);
        TSM_Write_Byte(REF_RST2, 0x00);
        TSM_Write_Byte(CH_HOLD1, 0x00);
        TSM_Write_Byte(CH_HOLD2, 0x00);
        TSM_Write_Byte(CTRL2, 0x07); //Enable Sleep Mode
        HAL_GPIO_WritePin(CAP_EN_GPIO, CAP_EN_Pin, GPIO_PIN_SET); //TSM12 I2C Disable
    }
    void key_drv_timeout_handle(void)
    {
        key_deal_fg = 0;
        memset(&pre_key_value,0,sizeof(BSP_Key_Package_t));
    }
    static void TSM12_Get_value(uint8_t *dat)
    {
        HAL_GPIO_WritePin(CAP_EN_GPIO, CAP_EN_Pin, GPIO_PIN_RESET);
        dat[0] = TSM_Read_Byte(OUTPUT1); //Get OUTPUT1 Register
        dat[1] = TSM_Read_Byte(OUTPUT2); //Get OUTPUT2 Register
        dat[2] = TSM_Read_Byte(OUTPUT3); //Get OUTPUT3 Register
        HAL_GPIO_WritePin(CAP_EN_GPIO, CAP_EN_Pin, GPIO_PIN_SET);
        return ;
    }
    void HAL_GPIO_EXTI_CAP_Callback(uint16_t GPIO_Pin) //IRQ Interrupt
    {
        if(GPIO_Pin == CAP_INT_Pin)
        {
            I2C_Init(1);
            bsp_delay_nus(100);
            TSM12_Get_value((uint8_t*)&touch_buffer);//获取按键值
            I2C_DeInit(1); 
            BSP_SEND_IRQ_EVENTS(BSP_DRV_KEY); //调用本文件bsp_events_handle()上报事件
            return ;
        }
    }
    void mcu_sleep_open(void)
    {
        __HAL_RCC_PWR_CLK_ENABLE();
        HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
    }
}
/*************************************** 
  * TSM12 DRIVE CODE END
***************************************/

BspKey::BspKey()
{
}
BspKey::~BspKey()
{
}
BspKey *BspKey::BspKey_registered(void)
{
    static BspKey t_ms_this;
    return &t_ms_this;
}
void BspKey::init(void)
{
    I2C_Init(1);
    TSM12_Init();
    //unit_test();
    I2C_DeInit(1);
    //BSP_ADD_TIMER(mcu_sleep_open,15000);//测试3s读一次
    return;
}

static const uint8_t key_value[] = //按键映射表
{
    7,8,4,1,3,2,6,5,9,10,0,11
};

bool BspKey::bsp_events_handle(BSP_Events_Package_t const *p_pack)
{
    
    Dprintf(EN_LOG,TAG,"touch_buffer = 0x%08x\r\n",touch_buffer);
    bsp_events_dat.e_events_type = BSP_Events_KEY;
    bsp_events_dat.s_pack.key_dat.key_num = 0;
    
    for(uint8_t i = 0;i<12;i++)//解析按键被按数量及按键值
    {
        if(touch_buffer & (0x00000003<<(i*2)))
        {
            bsp_events_dat.s_pack.key_dat.key_value[bsp_events_dat.s_pack.key_dat.key_num++] = key_value[i];
            Dprintf(EN_LOG,TAG,"按键 %d 被按下 i = %d\r\n",key_value[i],i);
        }
    }
    if(bsp_events_dat.s_pack.key_dat.key_num == 0)
        return false;
    if(memcmp(pre_key_value.key_value,bsp_events_dat.s_pack.key_dat.key_value,bsp_events_dat.s_pack.key_dat.key_num) == 0 && key_deal_fg)
    {//1s内按相同的按键不处理
        Dprintf(EN_LOG,TAG,"被优化 。。。 \r\n");
        return false;
    }
    key_deal_fg = 1;
    BSP_ADD_TIMER(key_drv_timeout_handle,300);//测试3s读一次
    memcpy(pre_key_value.key_value,bsp_events_dat.s_pack.key_dat.key_value,bsp_events_dat.s_pack.key_dat.key_num);
    //调试用
    bsp_drv_dat.e_drv_type = BSP_DRV_LED;
    bsp_drv_dat.s_drv_state.e_led = STATE_LED_OPEN;
    bsp_drv_dat.s_pack.led_ctl.timeout = 6000;
    bsp_drv_dat.s_pack.led_ctl.led_num = bsp_events_dat.s_pack.key_dat.key_num;
    memcpy(bsp_drv_dat.s_pack.led_ctl.led_value,bsp_events_dat.s_pack.key_dat.key_value,bsp_drv_dat.s_pack.led_ctl.led_num);
    BspInterfaces::bsp_deal_handle(&bsp_drv_dat);
    //启动刷卡检测6s 共六次
    bsp_drv_dat.e_drv_type = BSP_DRV_CARD;
    bsp_drv_dat.s_drv_state.e_card = STATE_Card_RXD;
    bsp_drv_dat.s_pack.card_msg.read_timeout = 6;
    BspInterfaces::bsp_deal_handle(&bsp_drv_dat);
    //蜂鸣80ms
    bsp_drv_dat.e_drv_type = BSP_DRV_BEEP;
    bsp_drv_dat.s_drv_state.e_beep = true;
    bsp_drv_dat.s_pack.beep_dat.beep_timeout = 80;
   if(bsp_events_dat.s_pack.key_dat.key_num != 0)
       BspInterfaces::bsp_deal_handle(&bsp_drv_dat);
    Dprintf(EN_LOG,TAG,"一共有 %d 个按键被按下\r\n",bsp_events_dat.s_pack.key_dat.key_num);

    if (bsp_events_dat.s_pack.key_dat.key_num == 3)
    {
        if (bsp_events_dat.s_pack.key_dat.key_value[0] == 1||bsp_events_dat.s_pack.key_dat.key_value[0] == 2||bsp_events_dat.s_pack.key_dat.key_value[0] == 3)
        {
            bsp_drv_dat.e_drv_type = BSP_DRV_SW;
            bsp_drv_dat.s_drv_state.e_lock = true;
            BspInterfaces::bsp_deal_handle(&bsp_drv_dat); //开锁操作
            Dprintf(EN_LOG, TAG, "开锁信号发出\r\n");
        }
        else
        {
            bsp_drv_dat.e_drv_type = BSP_DRV_SW;
            bsp_drv_dat.s_drv_state.e_lock = false;
            BspInterfaces::bsp_deal_handle(&bsp_drv_dat); //开锁操作
            Dprintf(EN_LOG, TAG, "关锁信号发出\r\n");
        }
    }

    //BspInterfaces::bsp_events_handle(&bsp_events_dat);//上报应用层处理
    return true;
}

bool BspKey::unit_test(void)
{
    bool ret = (TSM12_Check() == 1) ? false : true;
    if (ret == true)
    {
        Dprintf(EN_LOG, TAG, "按键测试successful\r\n");
    }
    else
    {
        Dprintf(EN_LOG, TAG, "按键测试fail\r\n");
    }
    return ret;
}
