#include "CUsb.h"
CUsb::CUsb()
{}
CUsb::~CUsb()
{}
BOOL CUsb::ReceiveCheck(RECEIVE_FRAME *frame)
{
    Usb_read(usb_recData,&usb_reclen);
    return FALSE;
}
void CUsb::Init(void)
{
    
}
void CUsb::TimerEvent(u32 ulTimeNum)
{
    switch (ulTimeNum)
	{
		case 1:
		{
            KillTimer(this,1);
            break;
		}
		default:
			break;
	}
}