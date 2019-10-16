#ifndef __CUSB_H__
#define __CUSB_H__
#include "IUsb.h"
#include "bsp_head.h"
class CUsb : public IUsb
{
    private:
    public:
    uint8_t usb_recData[64];
    uint8_t usb_reclen;
    CUsb();
    ~CUsb();
    void Init(void);
    virtual bool ReceiveCheck(uint8_t *frame);
    virtual void TimerEvent(uint32_t ulTimeNum);
};
#endif