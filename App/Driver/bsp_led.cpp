#include "bsp_led.h"
#include "bsp_systimer.h"

#define LED_SCL_H do{HAL_GPIO_WritePin(LED_DAT_GPIO,LED_SCL_Pin,GPIO_PIN_SET);bsp_delay_nus(1);}while(0);
#define LED_SCL_L do{HAL_GPIO_WritePin(LED_DAT_GPIO,LED_SCL_Pin,GPIO_PIN_RESET);bsp_delay_nus(1);}while(0);
#define LED_SDA_H do{HAL_GPIO_WritePin(LED_DAT_GPIO,LED_SDA_Pin,GPIO_PIN_SET);bsp_delay_nus(1);}while(0);
#define LED_SDA_L do{HAL_GPIO_WritePin(LED_DAT_GPIO,LED_SDA_Pin,GPIO_PIN_RESET);bsp_delay_nus(1);}while(0);

#define LED_EN_IC HAL_GPIO_WritePin(LED_EN_GPIO,LED_EN_Pin,GPIO_PIN_RESET);
#define LED_DIS_IC HAL_GPIO_WritePin(LED_EN_GPIO,LED_EN_Pin,GPIO_PIN_SET);

#define LED595_LATCH_RESET HAL_GPIO_WritePin(LED_UPDATE_GPIO,LED_UPDATE_Pin,GPIO_PIN_RESET);
#define LED595_LATCH_SET HAL_GPIO_WritePin(LED_UPDATE_GPIO,LED_UPDATE_Pin,GPIO_PIN_SET);
#define LED_UPDATE_IC do{LED595_LATCH_RESET;bsp_delay_nus(100); LED595_LATCH_SET;}while(0);

#define LED_PWR_ON  HAL_GPIO_WritePin(LED_PWR_LED_GPIO,LED_PWR_LED_Pin,GPIO_PIN_RESET);
#define LED_PWR_OFF HAL_GPIO_WritePin(LED_PWR_LED_GPIO,LED_PWR_LED_Pin,GPIO_PIN_SET);

//串口调试定义宏
#if ((defined USART_DEBUG) && (USART_DEBUG == __ON__))
#define TAG "bsp_led: "
#define EN_LOG PINK
#endif

extern "C"
{
    static void led_gpio_init(void);
    static void led_gpio_deinit(void);
    static void led_drv_timeout_handle(void);
    static void led_gpio_init(void)
    {
        //config led_scl cda pin
        GPIO_InitTypeDef GPIO_InitStruct;
        GPIO_InitStruct.Pin = LED_SCL_Pin | LED_SDA_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(LED_DAT_GPIO, &GPIO_InitStruct);
        //config led_en pin
        GPIO_InitStruct.Pin = LED_EN_Pin;
        HAL_GPIO_Init(LED_EN_GPIO, &GPIO_InitStruct);
        //config led_update pin
        GPIO_InitStruct.Pin = LED_UPDATE_Pin;
        HAL_GPIO_Init(LED_UPDATE_GPIO, &GPIO_InitStruct);
        //config led_pwr pin
        GPIO_InitStruct.Pin = LED_PWR_LED_Pin;
        HAL_GPIO_Init(LED_PWR_LED_GPIO, &GPIO_InitStruct);

        LED_PWR_ON;
        bsp_delay_nms(1);
        LED_EN_IC;
        LED_SCL_L;
        LED_SDA_L;
    }
    static void led_gpio_deinit(void)
    {
           LED_PWR_OFF;
           LED_DIS_IC;
        // HAL_GPIO_DeInit(LED_DAT_GPIO, LED_SCL_Pin | LED_SDA_Pin);
        // HAL_GPIO_DeInit(LED_UPDATE_GPIO, LED_UPDATE_Pin);
        // HAL_GPIO_DeInit(LED_EN_GPIO, LED_EN_Pin);
        // HAL_GPIO_DeInit(LED_PWR_LED_GPIO, LED_PWR_LED_Pin);
    }
    static void led_send_update(uint16_t dat)
    {
        uint8_t i = 0;         
        for (i = 0; i < 16; i++) 
        {
            if ((dat & 0x8000) == 0x8000)
            {
                LED_SDA_H;
            }
            else
            {
                LED_SDA_L;
            }

            dat = dat << 1;
            LED_SCL_L; 
            bsp_delay_nus(1);
            LED_SCL_H;      
        }
        //updata data to ic pin
        LED_UPDATE_IC;
    }
    static void led_drv_timeout_handle(void)
    {
        led_gpio_deinit();
        //BSP_ADD_TIMER(led_drv_timeout_handle,2000);
    }
}
const uint8_t led_map_hw[]
{
    0,
    6,//面板1号按键对应寄存器第七位
    5,
    9,
    7,
    2,
    10,
    3,
    1,
    8,
    4,
    11,
    12,
    13,
    14
};
BspLed::BspLed()
{
}
BspLed::~BspLed()
{
}

BspLed *BspLed::BspLed_registered(void)
{
    static BspLed t_ms_this;
    return &t_ms_this;
}

void BspLed::init(void)
{
    bsp_drv_dat.e_drv_type = BSP_DRV_LED;
    bsp_drv_dat.s_drv_state.e_led = STATE_LED_OPEN;
    bsp_drv_dat.s_pack.led_ctl.timeout = 5000;
    bsp_drv_dat.s_pack.led_ctl.led_num = 1;
    Dprintf(EN_LOG, TAG, "LED测试 挨个点亮\r\n");
    bsp_drv_dat.s_pack.led_ctl.led_value[0] = 10;
    bsp_deal_handle(&bsp_drv_dat);
    return;
}

bool BspLed::bsp_deal_handle(BSP_Drv_Deal_t *const p_deal)
{
    led_gpio_init();
    BSP_ADD_TIMER(led_drv_timeout_handle,p_deal->s_pack.led_ctl.timeout);
    uint16_t led_ram = 0;
    uint8_t led_num = p_deal->s_pack.led_ctl.led_num;
    uint8_t i = 0;
    if (led_num == 0)
        return true;
    while(led_num--)
    {
        led_ram |= (0x0001<<led_map_hw[p_deal->s_pack.led_ctl.led_value[i++]]);//
    }
    led_send_update(~led_ram);
    Dprintf(EN_LOG,TAG,"操作 LED 显存 0x%04x 时间 %d ms\r\n",led_ram,p_deal->s_pack.led_ctl.timeout);
    return true;
}

bool BspLed::unit_test(void)
{
    bsp_drv_dat.e_drv_type = BSP_DRV_LED;
    bsp_drv_dat.s_drv_state.e_led = STATE_LED_OPEN;
    bsp_drv_dat.s_pack.led_ctl.timeout = 100;
    bsp_drv_dat.s_pack.led_ctl.led_num = 1;
    Dprintf(EN_LOG,TAG,"LED测试 挨个点亮\r\n");
    for (uint8_t i = 0; i < 14; i++)
    {
        bsp_drv_dat.s_pack.led_ctl.led_value[0] = i;
        bsp_deal_handle(&bsp_drv_dat);
        bsp_delay_nms(100);
    }
    return true;
}
