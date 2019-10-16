#include "bsp_interface.h"
#include "CUsb.h"
int main(void)
{
  SuperInerfaces::SuperInerfaces_init(&g_bsp, 0); //初始化驱动
  //SuperInerfaces::SuperInerfaces_init(&g_bsp);//初始化APP
  CUsb g_CUsb;
  g_CUsb.IUsb::Init();
  g_bsp.bsp_mian();
  return 0;
}
