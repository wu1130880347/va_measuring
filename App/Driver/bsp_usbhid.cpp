#include "bsp_usbhid.h"
#include "bsp_systimer.h"
#include "hw_config.h"
#include "usb_init.h"
#include "main.h"
#include "usbio.h"
extern uint8_t USB_Received_Flag;
#if ((defined USART_DEBUG) && (USART_DEBUG == __ON__))
extern "C"
{
//	//是否打开该文件内的调试LOG 
//	static const char EN_LOG = YELLOW;
//	//#define EN_CSystem FALSE 
//	static const char TAG[] = "IUsb: ";
}
#endif

extern "C"
{
    
    static uint8_t Usb_read(uint8_t *data, uint8_t *len)
    {
        if (USB_Received_Flag)
        {
            USB_Received_Flag = 0;
            u8 ret = USB_GetData(data, 64);
            *len = ret;
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
      //static uint32_t count_usb = 0;
      extern uint8_t buf[2];
      extern uint32_t ch_get_value[20];
      uint8_t dat_buf[64] = {0};
      memcpy(dat_buf,(uint8_t*)&ch_get_value,64);
      Usb_write(dat_buf,64);
      BSP_ADD_TIMER(Usb_send_data,1000);//查询USB是否读到数据
    }
    static uint8_t usb_buf[64];
    static uint8_t len;
    static uint32_t buf_std[20] = {0};
    static uint32_t buf_tol[20] = {0};
    static void UsbReceiveCheck(void)
    {
        if(Usb_read(usb_buf,&len))
        {
            if (*(uint32_t *)usb_buf == 0xaabbccdd)
            {
                // uint16_t start_inx = *(uint16_t*)(usb_buf+6);
                // start_inx /= 10;
                // if (start_inx < 3)
                // {
                //     memcpy((uint8_t *)(buf_std + 10*(start_inx-1)), usb_buf + 8, 40);
                // }
                // else
                // {
                //     memcpy((uint8_t *)(buf_tol + 10*(start_inx-3)), usb_buf + 8, 40);
                // }
                // if (start_inx == 4)
                //     ;
                //Usb_write(usb_buf,len);
            }
        }
        BSP_ADD_TIMER(UsbReceiveCheck,0);//查询USB是否读到数据
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
    USB_Interrupts_Config();
    Set_USBClock();
    USB_Init();
    BSP_ADD_TIMER(UsbReceiveCheck,0);//查询USB是否读到数据
    BSP_ADD_TIMER(Usb_send_data,3000);//查询USB是否读到数据
}

