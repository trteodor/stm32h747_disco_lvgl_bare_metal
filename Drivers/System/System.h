#ifndef __SYSTEM_CONFIG_H__
#define __SYSTEM_CONFIG_H__

#include "stdint.h"

void ConfigSysTick1ms(void);
uint32_t GetSysTime(void);
void RegisterSysTickCallBack(void(*SysTickCallBack)(uint32_t TickCount));
void DelayMs(uint32_t Ticks);
void PLL_Config480Mhz(void);

#endif 