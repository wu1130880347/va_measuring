#include "IUsb.h"
#include "driver.h"
extern "C"
{
#include "hw_config.h"
#include "usb_init.h"
#include "main.h"
#include "usbio.h"
 extern uint8_t USB_Received_Flag;
}
#if ((defined USART_DEBUG) && (USART_DEBUG == __ON__))
extern "C"
{
	//是否打开该文件内的调试LOG 
	static const char EN_LOG = YELLOW;
	//#define EN_CSystem FALSE 
	static const char TAG[] = "IUsb: ";
}
#endif
void IUsb::Init(void)
{
    USB_Interrupts_Config();
    Set_USBClock();
    USB_Init();
}
IUsb::IUsb()
{}
IUsb::~IUsb()
{}
uint8_t IUsb::Usb_read(uint8_t * data,uint8_t *len)
{
    if(USB_Received_Flag)
    {
        USB_Received_Flag=0;
        u8 ret = USB_GetData(data,64);
        *len = ret;
        // Dprintf(EN_LOG,TAG,"usb get data %d byte data\n\r", ret);
        // for (u8 i = 0; i < ret; i++)
        // {
        //     Dprintf(EN_LOG,"","0x%02X ", data[i]);
        // }
        // Dprintf(EN_LOG,"","\r\n ");
        USB_SendData(data,*len);
    }
    return 0;
}
uint8_t IUsb::Usb_write(uint8_t * data,uint8_t *len)
{
    USB_SendData(data,*len);
    return 0;
}