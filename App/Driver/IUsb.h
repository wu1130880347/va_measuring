#ifndef __IUSB_H__
#define __IUSB_H__
#include "head.h"
#include "CSuperTimer.h"
class IUsb : public CSuperTimer
{
    private:
    public:
    IUsb();
    ~IUsb();
    void Init(void);
    uint8_t Usb_read(uint8_t * data,uint8_t *len);
    uint8_t Usb_write(uint8_t * data,uint8_t *len);
};
#endif