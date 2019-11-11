#include "bsp_usbhid.h"
#include "bsp_systimer.h"
#include "hw_config.h"
#include "usb_init.h"
#include "main.h"
#include "usbio.h"
#include "bsp_flash.h"
extern uint8_t USB_Received_Flag;
#if ((defined USART_DEBUG) && (USART_DEBUG == __ON__))
extern "C"
{
//	//�Ƿ�򿪸��ļ��ڵĵ���LOG 
//	static const char EN_LOG = YELLOW;
//	//#define EN_CSystem FALSE 
//	static const char TAG[] = "IUsb: ";
}
#endif

extern "C"
{
    extern bool flash_write(uint32_t addr, BSP_FlashCh_Package_t *p_dat,uint8_t len);
    extern bool flash_read(uint32_t addr, BSP_FlashCh_Package_t *ch_dat);
    extern uint8_t m_panel_status;
    uint32_t buf_std[20] = {0};
    uint32_t buf_tol[20] = {0};
    uint32_t buf_para[20] = {0};//У׼ϵ��
    static bool normal_send_fg = true;
    static void write_dat_to_flash(uint32_t *p_std,uint32_t *p_tol,uint8_t len)
    {
        BSP_FlashCh_Package_t t_dat;
        for(uint8_t i = 0;i<len;i++)
        {
            t_dat.standard_para = p_std[i];
            t_dat.tolerance_para = p_tol[i];
            t_dat.adjustment_para = buf_para[i];
            t_dat.check_valid = CH_KAY_VALID;
            t_dat.backup2 = 0xffffffff;
            flash_write(FLASH_PARA_CH_ADDR(i),&t_dat,1);
        }
    }
    static uint8_t Usb_read(uint8_t *data, uint8_t *len)
    {
        if (USB_Received_Flag)
        {
            USB_Received_Flag = 0;
            u8 ret = USB_GetData(data, 64);
            *len = ret;
            if (*(uint32_t *)data == 0xaabbccdd)
            {
                normal_send_fg = false;
                *(uint32_t *)data = 0xabcdabcd;
            }
            if (*(uint32_t *)data == 0xaabbccff)
            {
                m_panel_status = (u8)*(uint32_t *)(data+4);
            }
            if(*(uint32_t *)data == 0xaabbccee)
            {
                normal_send_fg = false;
                *(uint32_t *)data = 0xabceabce;
                if(*(uint32_t *)(data+4) == 1)
                {
                    memcpy(data+8,(uint8_t *)buf_std,40);
                }
                else if(*(uint32_t *)(data+4) == 2)
                {
                    memcpy(data+8,(uint8_t *)(buf_std+10),40);
                }
                else if(*(uint32_t *)(data+4) == 3)
                {
                    memcpy(data+8,(uint8_t *)buf_tol,40);
                    if(buf_tol[0] == 1000)
                        memcpy(data+8,(uint8_t *)buf_para,40);
                }
                else if(*(uint32_t *)(data+4) == 4)
                {
                    normal_send_fg = true;
                    memcpy(data+8,(uint8_t *)(buf_tol+10),40);
                    if(buf_tol[0] == 1000)
                        memcpy(data+8,(uint8_t *)(buf_para+10),40);
                }
            }
            USB_SendData(data, *len);
            return 1;
        }
        return 0;
    }
    static uint8_t Usb_write(uint8_t *data, uint8_t len)
    {
        USB_SendData(data, len);
        return 0;
    }
    static void Usb_send_data(void)
    {
      static uint32_t count_usb = 0;
      extern uint32_t ch_get_value[20];
      extern uint8_t hc595_ram[10];
      if (normal_send_fg == true)
      {
          uint8_t dat_buf[64] = {0};
          memcpy(dat_buf, (uint8_t *)&ch_get_value, 64);
          for(uint8_t i = 0;i<4;i++)
          {
              dat_buf[15 + 16*i] = hc595_ram[4-i];//����ͨ��״̬
          }
          dat_buf[3] = m_panel_status;//����״̬��ֵ�ڵ�һͨ���ĸ�24λ��32λ�� 1��ѹ 0����
          Usb_write(dat_buf, 64);
      }
      else
      {
          if(count_usb++ >= 10)
          {
              normal_send_fg = true;
              count_usb = 0;
          }
      }
      BSP_ADD_TIMER(Usb_send_data, 1000); //��ѯUSB�Ƿ��������
    }

    static void UsbReceiveCheck(void)
    {
      static uint8_t usb_buf[64];
            static uint8_t len;
        if(Usb_read(usb_buf,&len))
        {
            if (*(uint32_t *)usb_buf == 0xabcdabcd)
            {
                uint16_t start_inx = *(uint16_t*)(usb_buf+6);
                 start_inx /= 10;
                if (start_inx < 3)
                {
                    memcpy((uint8_t *)(buf_std + 10*(start_inx-1)), usb_buf + 8, 40);
                }
                else
                {
                    memcpy((uint8_t *)(buf_tol + 10*(start_inx-3)), usb_buf + 8, 40);
                    if(buf_tol[0] == 1000)
                        memcpy((uint8_t *)(buf_para + 10*(start_inx-3)), usb_buf + 8, 40);
                }
                if(start_inx == 4)
                {
                    normal_send_fg = true;
                    write_dat_to_flash(buf_std,buf_tol,20);
                }
            }
        }
        BSP_ADD_TIMER(UsbReceiveCheck,0);//��ѯUSB�Ƿ��������
    }
    static void init_para_std_tol(void)
    {
        BSP_FlashCh_Package_t t_dat;
        for(uint8_t i = 0;i<20;i++)
        {
            flash_read(FLASH_PARA_CH_ADDR(i), &t_dat);
            buf_std[i] = t_dat.standard_para;
            buf_tol[i] = t_dat.tolerance_para;
            buf_para[i] = t_dat.adjustment_para;
        }
    }
}

BspUsbhid::BspUsbhid()
{}
BspUsbhid::~BspUsbhid()
{}
BspUsbhid *BspUsbhid::BspUsbhid_registered(void)
{
    static BspUsbhid t_ms_this;
    return &t_ms_this;
}
void BspUsbhid::init(void)
{
    init_para_std_tol();
    USB_Interrupts_Config();
    Set_USBClock();
    USB_Init();
    BSP_ADD_TIMER(UsbReceiveCheck,0);//��ѯUSB�Ƿ��������
    BSP_ADD_TIMER(Usb_send_data,3000);//��ѯUSB�Ƿ��������
}

