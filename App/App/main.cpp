#include "bsp_interface.h"
int main(void)
{
  SuperInerfaces::SuperInerfaces_init(&g_bsp, 0); //初始化驱动
  //SuperInerfaces::SuperInerfaces_init(&g_bsp);//初始化APP
  g_bsp.bsp_mian();
  return 0;
}
