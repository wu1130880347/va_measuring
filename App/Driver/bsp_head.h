#ifndef __BSP_HEAD_H__
#define __BSP_HEAD_H__
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "bsp_hw_config.h"
#include "dbg_log.h"

#define BSP_DAT_PACKAGE_LEN  512

typedef void (*sys_tim_fun_t)(void);

typedef enum
{
    BSP_Events_NOEVENTS = 0,
    BSP_Events_KEY,
    BSP_Events_CARD,
    BSP_Events_LORA_RxDone,
    BSP_Events_LORA_TxDone,
    BSP_Events_BAT_Low,
    BSP_Events_LOCK,      //锁舌
    BSP_Events_Anti_LOCK, //反锁
    BSP_Events_SW_KEY,    //机械钥匙

} BSP_Events_Types_t;

typedef enum
{
    BSP_DRV_SW = 0, //操作门锁 2s内只操作一次
    BSP_DRV_LED,    //操作LED
    BSP_DRV_BEEP,   //操作蜂鸣器
    BSP_DRV_FLASH,  //操作FLASH
    BSP_DRV_LORA,   //操作LORA
    BSP_DRV_CARD,   //操作卡
    BSP_DRV_RTC,    //操作RTC
    BSP_DRV_UART,   //操作串口
    BSP_DRV_KEY,    //按键驱动
    BSP_DRV_SYS_PWR,    //系统电源
    BSP_DRV_NoEvents,//无事件驱动

    BSP_DRV_COUNTS  //驱动数量
} BSP_Drv_Types_t;

//LORA FLASH UART通信数据
#pragma pack(1)
typedef struct 
{
    uint32_t addr;//读写地址
    uint16_t erase_4k;//页序号
    uint8_t erase_32k;//扇区序号
    uint8_t erase_64k;//块序号
}BSP_FLASH_Extra_t;
#pragma pack()

#pragma pack(1)
typedef struct 
{
    uint8_t dat[BSP_DAT_PACKAGE_LEN];
    uint16_t len;
    BSP_FLASH_Extra_t t_cmd;
}BSP_FLU_Package_t;
#pragma pack()

//卡信息数据结构
#pragma pack(1)
typedef struct
{
    uint32_t ulCardNO;   //卡内码
    uint8_t card_dat[16];//第15扇区 0块的数据 16byte card_dat[0]等于0时，数据无效；卡为参数配置卡时数据有效
    uint8_t read_timeout;//读取卡时间，每秒一次
}BSP_Card_Package_t;
#pragma pack()

//按键数据
#pragma pack(1)
typedef struct
{
    uint8_t key_num;       //被按的按键数量 MAX32
    uint8_t key_value[32]; //key_value[x]的值为按键的值{0-9-*-#}
} BSP_Key_Package_t;
#pragma pack()

//LED操作
#pragma pack(1)
typedef struct
{//现在只提供亮灭及时间长度；12 13 14 分别别忘 红 绿 蓝
    uint16_t timeout;      //亮的总时长 ms;
    uint8_t led_num;       //操作几个灯
    uint8_t led_value[32]; //最多32个灯,如led_num = 1; led_value[0] == 8;即设置8号键的灯
} BSP_Led_Package_t;
#pragma pack()

#pragma pack(1)
typedef struct
{
    uint16_t bat_value; //电池电量 百分比 【0-100】
}BSP_Bat_Package_t;
#pragma pack()

#pragma pack(1)
typedef struct
{
     uint16_t beep_timeout ; //单位 1ms，蜂鸣器响时间,0时直接关闭
}BSP_Beep_Package_t;
#pragma pack()

#pragma pack(1)
typedef struct
{
    uint16_t year;//20XX年
    uint8_t month;//
    uint8_t day;
    uint8_t hour;//24小时制
    uint8_t minute;
    uint8_t second;
}BSP_Rtc_Package_t;
#pragma pack()

#pragma pack(1)
typedef union 
{
    BSP_Card_Package_t card_msg;
    BSP_FLU_Package_t *flu_dat;//flash  Lora  Uart读写共用数据内存
    BSP_Key_Package_t key_dat;
    BSP_Led_Package_t led_ctl;
    BSP_Bat_Package_t bat_dat;
    BSP_Beep_Package_t beep_dat;
    BSP_Rtc_Package_t rtc_dat;
} BSP_Package_t;
#pragma pack()

typedef enum {STATE_LORA_RXD,STATE_LORA_TXD}BSP_Lora_State_t;
typedef enum {STATE_UART_RXD,STATE_UART_TXD}BSP_Uart_State_t;
typedef enum {STATE_Card_RXD,STATE_Card_TXD}BSP_Card_State_t;
typedef enum
{
    STATE_Flash_RXD,
    STATE_Flash_TXD,
    STATE_Flash_ERASE_4K,
    STATE_Flash_ERASE_32K,
    STATE_Flash_ERASE_64K
} BSP_Flash_State_t;
typedef enum {STATE_RTC_RXD,STATE_RTC_TXD}BSP_RTC_State_t;
typedef enum {STATE_LED_OPEN,STATE_LED_CLOSE}BSP_LED_State_t;
typedef bool BSP_LOCK_State_t;//true为开 false为关
typedef bool BSP_BEEP_State_t;//true为开 false为关

typedef union 
{
    BSP_Lora_State_t e_lora;
    BSP_Uart_State_t e_uart;
    BSP_Card_State_t e_card;
    BSP_LOCK_State_t e_lock;
}BSP_Events_State_t;

#pragma pack(1)
typedef union 
{
    BSP_Lora_State_t e_lora;
    BSP_Uart_State_t e_uart;
    BSP_Card_State_t e_card;
    BSP_LOCK_State_t e_lock; 
    BSP_Flash_State_t e_flash;
    BSP_RTC_State_t e_rtc;
    BSP_LED_State_t e_led;
    BSP_BEEP_State_t e_beep;
} BSP_Drv_State_t;
#pragma pack()

#pragma pack(1)
typedef struct 
{
    BSP_Events_Types_t e_events_type;//事件类型
    BSP_Events_State_t s_event_state;//事件状态
    BSP_Package_t s_pack;//数据包
    bool bsp_events_lock;//事件锁防止并发 
}BSP_Events_Package_t;
#pragma pack()

#pragma pack(1)
typedef struct 
{
    BSP_Drv_Types_t    e_drv_type;//处理操作类型
    BSP_Drv_State_t    s_drv_state;//处理操作状态
    BSP_Package_t      s_pack;     //数据包
}BSP_Drv_Deal_t;
#pragma pack()


/**************************************************************
**函数原型  static bool BspSysTimer_AddFun(sys_tim_fun_t p_fun,uint32_t timeout);
**功能描述  添加应用定时器
**入口参数  x：超时后函数调用入口(类型 void *(void)) y:超时时间(类型：uint32_t 单位ms)
**出口参数  NULL
**************************************************************/
//class BspSysTimer;
//#define APP_ADD_TIMER(x,y) BSP_ADD_TIMER(x,y)//应用定时器宏
/**************************************************************
**函数原型  APP_printf(...)
**功能描述  使用与stdio.h的 printf 一致
**入口参数  ...
**出口参数  NULL
**************************************************************/
#define APP_printf(...) Dprintf(WHITE,"App:",##__VA_ARGS__);
extern BSP_FLU_Package_t bsp_Rx_flu;//读取数据flash Lora Uart 数据内存
extern BSP_FLU_Package_t bsp_Tx_flu;//写入数据flash Lora Uart 数据内存
extern BSP_Drv_Deal_t bsp_drv_dat;  //驱动操作配置

#endif