#ifndef __BSP_SYSTEM_H__
#define __BSP_SYSTEM_H__
#include "bsp_head.h"

#define _Error_Handler(a, b)

extern void SystemClock_Config(void);

extern void bsp_init_dog(void);
extern void bsp_clear_dog(void);
extern void bsp_active_drv(void);//低功耗唤醒管理
extern void bsp_sleep_drv(void); //低功耗休眠管理
extern void bsp_delay_nus(uint32_t n);
extern void bsp_delay_nms(uint32_t n);
#endif