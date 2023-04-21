#include "stm32f7xx.h"
#include <stddef.h>

volatile uint32_t SysTime=0;
static void (*StSysTickCallBack)(uint32_t TickCount) = NULL;
void SysTick_Handler(void);

/**********************************************************************************************/
void ConfigSysTick1ms(void)
{
	/*PLL is configured on 200k, then 200k/8= 25 000 then count to: 25 000 will took 1ms*/
	SysTick_Config(200000/8);
	SysTick->CTRL &= ~SysTick_CTRL_CLKSOURCE_Msk;
	NVIC_SetPriority(SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);
}

/******************************************************************************************************************/
void SysTick_Handler(void)
{
	SysTime++;
	if(StSysTickCallBack != NULL)
	{
		StSysTickCallBack(SysTime);
	}
}
/******************************************************************************************************************/
uint32_t GetSysTime(void)
{
	return SysTime;
}
/******************************************************************************************************************/
void RegisterSysTickCallBack(void(*SysTickCallBack)(uint32_t TickCount))
{
	StSysTickCallBack = SysTickCallBack;
}
/******************************************************************************************************************/
void DelayMs(uint32_t Ticks)
{
	volatile uint32_t StartTicks = SysTime;

	while( (SysTime - StartTicks) < Ticks) {
		/*Waiting*/
	}
}
/**********************************************************************************************/
void PLL_Config200Mhz(void)
{
	//na szybkosci szkoda tez na to czasu...
	__IO uint32_t hse_st = 0;
	__IO uint32_t timeout_c = 0;

	RCC->CR |= (1U<<16); //RCC_HSEON
	/*Wait for HSE*/
    while( (hse_st == 0) &&  (timeout_c != 10000))
	{
		hse_st  = ((RCC->CR) & (1U<<17)); //(1U<<17)
		timeout_c++;
	}

	 if(hse_st)
	 {
		 RCC->APB1ENR |=(1U<<28); //RCC_PWREN
		 /*Configure voltage regulator*/
		 PWR->CR1 &= ~(1U<<14);
		 PWR->CR1 &= ~(1U<<15);
		 /*Set AHB prescaler to 1 so that HCLK = SYSCLK*/
		 RCC->CFGR &=~(1U<<7);
		 /*PCLK1 = 50Mhz*/
		 PWR->CR1 |= (1U<<10);
		 PWR->CR1 &= ~(1U<<11);
		 PWR->CR1 |= (1U<<12);
		/*PCLK1 = 100Mhz*/
		 PWR->CR1 &= ~(1U<<13);
		 PWR->CR1 &= ~(1U<<14);
		 PWR->CR1 |= (1U<<15);

	    RCC->PLLCFGR = RCC_PLLCFGR_PLLSRC_HSE | 4ul << RCC_PLLCFGR_PLLM_Pos /*M*/ | 200ul<<RCC_PLLCFGR_PLLN_Pos /*N*/ 
										| ((2ul>>1)-1) << RCC_PLLCFGR_PLLP_Pos /*P*/
										| 1ul << 29; /*bit 29 by default is equal 1 */ 

		RCC->CR |= RCC_CR_PLLON;
		while((RCC->CR & RCC_CR_PLLRDY) == 0){}
		FLASH->ACR = FLASH_ACR_LATENCY_7WS;
		RCC->CFGR &= ~(1U<<0);
		RCC->CFGR |= (1U<<1);
		/*wait till PLL is selected*/
		while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {}
	 }
}
