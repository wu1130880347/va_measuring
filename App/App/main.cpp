#include "bsp_interface.h"
int main(void)
{
  SuperInerfaces::SuperInerfaces_init(&g_bsp, 0); //��ʼ������
  //SuperInerfaces::SuperInerfaces_init(&g_bsp);//��ʼ��APP
  g_bsp.bsp_mian();
  return 0;
}
