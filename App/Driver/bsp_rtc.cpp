#include "bsp_rtc.h"
#include "bsp_systimer.h"
#include "bsp_i2c.h"

//���ڵ��Զ����
#if ((defined USART_DEBUG) && (USART_DEBUG == __ON__))
#define TAG "bsp_rtc: "
#define EN_LOG GREEN
#endif

extern "C"
{   
    static uint8_t const time_remmap[][8] = 
    {
        {1,2,4,8,10,20,40,80},
        {1,2,4,8,10,0,0,0},
        {1,2,4,8,10,20,0,0},
        {1,2,4,0,0,0,0,0},
        {1,2,4,8,10,20,0,0},
        {1,2,4,8,10,20,40,0},
        {1,2,4,8,10,20,40,0},
    };//�ֱ�����������ʱ����ת��

    static void rtc_swap_byteToint(uint8_t *dat)
    {
        uint8_t t = 0;
        for (uint8_t j = 0; j < 7; j++)
        {
            t = 0;
            for (uint8_t i = 0; i < 8; i++)
            {
                t += (dat[j] & 0x01) * time_remmap[j][7-i];
                dat[j] = dat[j] >> 1;
            }
            dat[j] = t;
        }
    }

    static void rtc_swap_byteTotime(uint8_t *dat)
    {
        uint8_t t = 0;
        for (uint8_t j = 0; j < 7; j++)
        {
            t = 0;
            for (uint8_t i = 0; i < 8; i++)
            {
                if (time_remmap[j][7 - i] != 0)
                {
                    if ((dat[j] / (time_remmap[j][7 - i])) == 1)
                    {
                        t |= (0x01 << i);
                        dat[j] -= time_remmap[j][7 - i];
                    }
                }
            }
            dat[j] = t;
        }
        dat[4] |= 0x02;//24Сʱ�Ʊ�־λ
    }

    static bool rtc_check_set_time(uint8_t *t_dat)
    {
        uint8_t time_dat[7] = {0};
        Rtc_Read_n_Byte(0x64|0x01,time_dat,7);
        if(memcmp(time_dat,t_dat,7) != 0)
        {
            Dprintf(EN_LOG,TAG,"ʱ������ʧ��\r\n");
             return false;
        }
        Dprintf(EN_LOG,TAG,"ʱ�����óɹ�\r\n");
        return true;
    }

    static bool rtc_set_time(BSP_Rtc_Package_t * t_dat)
    {
        uint8_t time_dat[7] = {0};
        time_dat[0] = 0;
        I2C_Init(3);
        Rtc_Wirte_n_Byte(0x62, time_dat, 1);//���ж�
        Rtc_Read_n_Byte(0x60 | 0x01, time_dat, 1);//��״̬
        if (time_dat[0] & 0xc0)
        {
            time_dat[0] = 0xC0;
            Rtc_Wirte_n_Byte(0x60, time_dat, 1);//��λ
            bsp_delay_nms(10);
        }
        time_dat[0] = 0x40;
        Rtc_Wirte_n_Byte(0x60, time_dat, 1);//4Сʱ��

        bsp_delay_nms(10);
        time_dat[0] = (t_dat->year) % 2000;
        time_dat[1] = t_dat->month;
        time_dat[2] = t_dat->day;
        time_dat[4] = t_dat->hour;
        time_dat[5] = t_dat->minute;
        time_dat[6] = t_dat->second;
        rtc_swap_byteTotime(time_dat);
        Rtc_Wirte_n_Byte(0x64,time_dat,7);//����ʱ��

        bool ret = rtc_check_set_time(time_dat);
        I2C_DeInit(3);
        return ret;
    }
    static bool rtc_get_time(BSP_Rtc_Package_t * t_dat)
    {
        I2C_Init(3);
        Dprintf(EN_LOG,TAG,"��ȡRTCʱ��:\r\n");
        uint8_t time_dat[7] = {0};
        Rtc_Read_n_Byte(0x64|0x01,time_dat,7);
        rtc_swap_byteToint(time_dat);//תΪʱ��
        t_dat->year = 2000 + time_dat[0];
        t_dat->month = time_dat[1];
        t_dat->day = time_dat[2];
        t_dat->hour = time_dat[4];
        t_dat->minute = time_dat[5];
        t_dat->second = time_dat[6];

        Dprintf(EN_LOG,TAG,"%4d�� %2d�� %2d�� %2dʱ %2d�� %2d��\r\n",t_dat->year,\
        t_dat->month,t_dat->day,t_dat->hour,t_dat->minute,t_dat->second);
        I2C_DeInit(3);
        return true;
    }
    static void rtc_drv_timeout_handle(void)
    {
        BSP_Rtc_Package_t t_time;
        rtc_get_time(&t_time);
        BSP_ADD_TIMER(rtc_drv_timeout_handle,3000);//����3s��һ��
    }
}
BspRtc::BspRtc()
{
}
BspRtc::~BspRtc()
{
}

BspRtc *BspRtc::BspRtc_registered(void)
{
    static BspRtc t_ms_this;
    return &t_ms_this;
}

bool BspRtc::bsp_deal_handle(BSP_Drv_Deal_t *const p_deal)
{
    bool ret = false;
    if (p_deal->s_drv_state.e_rtc == STATE_RTC_TXD)
        ret = rtc_set_time(&(p_deal->s_pack.rtc_dat));
    else if (p_deal->s_drv_state.e_rtc == STATE_RTC_RXD)
        ret = rtc_get_time(&(p_deal->s_pack.rtc_dat));
    return ret;
}
void BspRtc::init(void)
{
    return ;
}
bool BspRtc::unit_test()
{
    //���ڲ���
    BSP_Rtc_Package_t t_time;
    t_time.year = 2019;
    t_time.month = 10;
    t_time.day = 8;
    t_time.hour = 9;
    t_time.minute = 27;
    t_time.second = 35;
    rtc_set_time(&t_time);
    BSP_ADD_TIMER(rtc_drv_timeout_handle,3000);//����3s��һ��
    return true;
}