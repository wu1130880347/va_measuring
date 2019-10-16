#ifndef __IUSB_H__
#define __IUSB_H__
#include "bsp_head.h"
class IUsb
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