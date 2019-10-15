#ifndef __CUSB_H__
#define __CUSB_H__
#include "head.h"
#include "CUdpProtocol.h"
#include "IUsb.h"

class CUsb : public IUsb,public CUdpProtocol
{
    private:
    public:
    uint8_t usb_recData[64];
    uint8_t usb_reclen;
    CUsb();
    ~CUsb();
    void Init(void);
    virtual BOOL ReceiveCheck(RECEIVE_FRAME *frame);
    virtual void TimerEvent(u32 ulTimeNum);
};
#endif