#include "CUsb.h"
CUsb::CUsb()
{}
CUsb::~CUsb()
{}
bool CUsb::ReceiveCheck(uint8_t *frame)
{
    Usb_read(usb_recData,&usb_reclen);
    return false;
}
void CUsb::Init(void)
{
    
}
void CUsb::TimerEvent(uint32_t ulTimeNum)
{
    switch (ulTimeNum)
	{
		case 1:
		{
            break;
		}
		default:
			break;
	}
}