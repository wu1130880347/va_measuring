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
    BSP_Events_LOCK,      //����
    BSP_Events_Anti_LOCK, //����
    BSP_Events_SW_KEY,    //��еԿ��

} BSP_Events_Types_t;

typedef enum
{
    BSP_DRV_SW = 0, //�������� 2s��ֻ����һ��
    BSP_DRV_LED,    //����LED
    BSP_DRV_BEEP,   //����������
    BSP_DRV_FLASH,  //����FLASH
    BSP_DRV_LORA,   //����LORA
    BSP_DRV_CARD,   //������
    BSP_DRV_RTC,    //����RTC
    BSP_DRV_UART,   //��������
    BSP_DRV_KEY,    //��������
    BSP_DRV_SYS_PWR,    //ϵͳ��Դ
    BSP_DRV_NoEvents,//���¼�����

    BSP_DRV_COUNTS  //��������
} BSP_Drv_Types_t;

//LORA FLASH UARTͨ������
#pragma pack(1)
typedef struct 
{
    uint32_t addr;//��д��ַ
    uint16_t erase_4k;//ҳ���
    uint8_t erase_32k;//�������
    uint8_t erase_64k;//�����
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

//����Ϣ���ݽṹ
#pragma pack(1)
typedef struct
{
    uint32_t ulCardNO;   //������
    uint8_t card_dat[16];//��15���� 0������� 16byte card_dat[0]����0ʱ��������Ч����Ϊ�������ÿ�ʱ������Ч
    uint8_t read_timeout;//��ȡ��ʱ�䣬ÿ��һ��
}BSP_Card_Package_t;
#pragma pack()

//��������
#pragma pack(1)
typedef struct
{
    uint8_t key_num;       //�����İ������� MAX32
    uint8_t key_value[32]; //key_value[x]��ֵΪ������ֵ{0-9-*-#}
} BSP_Key_Package_t;
#pragma pack()

//LED����
#pragma pack(1)
typedef struct
{//����ֻ�ṩ����ʱ�䳤�ȣ�12 13 14 �ֱ���� �� �� ��
    uint16_t timeout;      //������ʱ�� ms;
    uint8_t led_num;       //����������
    uint8_t led_value[32]; //���32����,��led_num = 1; led_value[0] == 8;������8�ż��ĵ�
} BSP_Led_Package_t;
#pragma pack()

#pragma pack(1)
typedef struct
{
    uint16_t bat_value; //��ص��� �ٷֱ� ��0-100��
}BSP_Bat_Package_t;
#pragma pack()

#pragma pack(1)
typedef struct
{
     uint16_t beep_timeout ; //��λ 1ms����������ʱ��,0ʱֱ�ӹر�
}BSP_Beep_Package_t;
#pragma pack()

#pragma pack(1)
typedef struct
{
    uint16_t year;//20XX��
    uint8_t month;//
    uint8_t day;
    uint8_t hour;//24Сʱ��
    uint8_t minute;
    uint8_t second;
}BSP_Rtc_Package_t;
#pragma pack()

#pragma pack(1)
typedef union 
{
    BSP_Card_Package_t card_msg;
    BSP_FLU_Package_t *flu_dat;//flash  Lora  Uart��д���������ڴ�
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
typedef bool BSP_LOCK_State_t;//trueΪ�� falseΪ��
typedef bool BSP_BEEP_State_t;//trueΪ�� falseΪ��

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
    BSP_Events_Types_t e_events_type;//�¼�����
    BSP_Events_State_t s_event_state;//�¼�״̬
    BSP_Package_t s_pack;//���ݰ�
    bool bsp_events_lock;//�¼�����ֹ���� 
}BSP_Events_Package_t;
#pragma pack()

#pragma pack(1)
typedef struct 
{
    BSP_Drv_Types_t    e_drv_type;//�����������
    BSP_Drv_State_t    s_drv_state;//�������״̬
    BSP_Package_t      s_pack;     //���ݰ�
}BSP_Drv_Deal_t;
#pragma pack()


/**************************************************************
**����ԭ��  static bool BspSysTimer_AddFun(sys_tim_fun_t p_fun,uint32_t timeout);
**��������  ���Ӧ�ö�ʱ��
**��ڲ���  x����ʱ�����������(���� void *(void)) y:��ʱʱ��(���ͣ�uint32_t ��λms)
**���ڲ���  NULL
**************************************************************/
//class BspSysTimer;
//#define APP_ADD_TIMER(x,y) BSP_ADD_TIMER(x,y)//Ӧ�ö�ʱ����
/**************************************************************
**����ԭ��  APP_printf(...)
**��������  ʹ����stdio.h�� printf һ��
**��ڲ���  ...
**���ڲ���  NULL
**************************************************************/
#define APP_printf(...) Dprintf(WHITE,"App:",##__VA_ARGS__);
extern BSP_FLU_Package_t bsp_Rx_flu;//��ȡ����flash Lora Uart �����ڴ�
extern BSP_FLU_Package_t bsp_Tx_flu;//д������flash Lora Uart �����ڴ�
extern BSP_Drv_Deal_t bsp_drv_dat;  //������������

#endif