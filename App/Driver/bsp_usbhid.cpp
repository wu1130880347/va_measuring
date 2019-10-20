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
            // Dprintf(EN_LOG,TAG,"usb get data %d byte data\n\r", ret);
            // for (u8 i = 0; i < ret; i++)
            // {
            //     Dprintf(EN_LOG,"","0x%02X ", data[i]);
            // }
            // Dprintf(EN_LOG,"","\r\n ");
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
      extern uint8_t buf[2];
      uint8_t dat_buf[64] = {0};
      sprintf((char *)dat_buf,"buf[1] = 0x%02x,buf[2] = 0x%02x\r\n",buf[1],buf[2]);
      Usb_write(dat_buf,strlen((char const *)dat_buf));
      bsp_delay_nms(100);
      uint16_t temp = (buf[1] << 8 | buf[2]);
      float val = temp * 1.0 * 3303.3 / 65535;
      sprintf((char *)dat_buf,"ch = 1 : Vol :  %.3f\r\n", val);
      Usb_write(dat_buf,strlen((char const *)dat_buf));
      BSP_ADD_TIMER(Usb_send_data,3000);//查询USB是否读到数据
    }

    static void UsbReceiveCheck(void)
    {
        static uint8_t usb_buf[64];
        static uint8_t len;
        if(Usb_read(usb_buf,&len))
        {
            ;//读到数据
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

