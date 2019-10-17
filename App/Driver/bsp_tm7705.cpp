#include "bsp_tm7705.h"
BspTm7705::BspTm7705()
{}
BspTm7705::~BspTm7705()
{}
BspTm7705 *BspTm7705::BspTm7705_registered(void)
{
    static BspTm7705 t_ms_this;
    return &t_ms_this;
}
void BspTm7705::init(void)
{
}

