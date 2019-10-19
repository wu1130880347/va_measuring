#include "dbg_log.h"
#include "bsp_system.h"

#if ((defined USART_DEBUG) && (USART_DEBUG == __ON__))
int fputc(int ch, FILE *f)
{
    //while ((USART1->SR & 0X40) == 0)
        //;
    bsp_delay_nus(100);
    USART1->DR = (u8)ch;
    return ch;
}
#endif

//串口调试,20181220添加,开启宏位置：driver.h
//30黑色 31红色 32绿色 33黄色 34蓝色 35品红 36青色 37白色
#if ((defined USART_DEBUG) && (USART_DEBUG == __ON__))
uint8_t Dprintf(uint8_t enable_flag, const char *tag, const char *format, ...)
{
    if (enable_flag & 0x01)
    {
        va_list args;
        va_start(args, tag);
        if (enable_flag & 0x02)
            vprintf("\033[33m", args);
        else if (enable_flag & 0x04)
            vprintf("\033[34m", args);
        else if (enable_flag & 0x08)
            vprintf("\033[35m", args);
        else if (enable_flag & 0x10)
            vprintf("\033[36m", args);
        else if (enable_flag & 0x20)
            vprintf("\033[37m", args);
        else if (enable_flag & 0x40)
            vprintf("\033[31m", args);
        else if (enable_flag & 0x80)
            vprintf("\033[30m", args);
        else
            vprintf("\033[32m", args);

        vprintf(tag, args);
        vprintf(format, args);

        vprintf("\033[0m", args);
        va_end(args);
    }
    else
        return 1;
    return 0;
}
#endif
