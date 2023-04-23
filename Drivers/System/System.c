#include "stm32h7xx.h"
#include <stddef.h>

/*Defines */

#define PWR_REGULATOR_VOLTAGE_SCALE1  (PWR_D3CR_VOS_1 | PWR_D3CR_VOS_0)
#define PWR_SUPPLY_CONFIG_MASK (PWR_CR3_SMPSLEVEL | PWR_CR3_SMPSEXTHP | \
                                PWR_CR3_SMPSEN | PWR_CR3_LDOEN | PWR_CR3_BYPASS)

#define __RCC_PLL_CONFIG(__RCC_PLLSOURCE__, __PLLM1__, __PLLN1__, __PLLP1__, __PLLQ1__,__PLLR1__ ) \
                  do{ MODIFY_REG(RCC->PLLCKSELR, (RCC_PLLCKSELR_PLLSRC | RCC_PLLCKSELR_DIVM1) , ((__RCC_PLLSOURCE__) | ( (__PLLM1__) <<4U)));  \
                      WRITE_REG (RCC->PLL1DIVR , ( (((__PLLN1__) - 1U )& RCC_PLL1DIVR_N1) | ((((__PLLP1__) -1U ) << 9U) & RCC_PLL1DIVR_P1) | \
                                ((((__PLLQ1__) -1U) << 16U)& RCC_PLL1DIVR_Q1) | ((((__PLLR1__) - 1U) << 24U)& RCC_PLL1DIVR_R1))); \
                    } while(0)


#define RCC_FLAG_MASK  ((uint8_t)0x1F)

#define __RCC_GET_FLAG(__FLAG__) (((((((__FLAG__) >> 5U) == 1U)? RCC->CR :((((__FLAG__) >> 5U) == 2U) ? RCC->BDCR : \
((((__FLAG__) >> 5U) == 3U)? RCC->CSR : ((((__FLAG__) >> 5U) == 4U)? RCC->RSR :RCC->CIFR))))  & (1UL << ((__FLAG__) & RCC_FLAG_MASK)))!= 0U)? 1U : 0U)



#define RCC_PLLSOURCE_HSE              (0x00000002U)
#define RCC_FLAG_PLLRDY                ((uint8_t)0x39)




/**/



volatile uint32_t SysTime=0;
static void (*StSysTickCallBack)(uint32_t TickCount) = NULL;
void SysTick_Handler(void);

/**********************************************************************************************/
void ConfigSysTick1ms(void)
{
	SysTick_Config(400000/8);
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
void PLL_Config400Mhz(void)
{
	__IO uint32_t timeout_c = 0;

	RCC->CR |= RCC_CR_HSEON; //RCC_HSEON
	/*Wait for HSE*/
    while( (((RCC->CR) & RCC_CR_HSERDY) == 0) &&  (timeout_c != 10000)){ timeout_c++; }

	if( ((RCC->CR) & RCC_CR_HSERDY) )
	{
		MODIFY_REG (PWR->CR3, PWR_SUPPLY_CONFIG_MASK, PWR_CR3_SMPSEN);
		MODIFY_REG(PWR->D3CR, PWR_D3CR_VOS, PWR_REGULATOR_VOLTAGE_SCALE1);
		while(!((PWR->D3CR & PWR_D3CR_VOSRDY)    == PWR_D3CR_VOSRDY)){}

		/*Configure MAIN PLL*/
		MODIFY_REG(RCC->PLLCKSELR, RCC_PLLCKSELR_PLLSRC, RCC_PLLSOURCE_HSE);
		CLEAR_BIT(RCC->CR, RCC_CR_PLL1ON); /*Disable pll*/
		while(__RCC_GET_FLAG(RCC_FLAG_PLLRDY) != 0U){}
		__RCC_PLL_CONFIG(RCC_PLLSOURCE_HSE,5,160,2,6,2);
		CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLL1FRACEN);
		MODIFY_REG(RCC->PLL1FRACR, RCC_PLL1FRACR_FRACN1, (uint32_t)(0U) << RCC_PLL1FRACR_FRACN1_Pos);
		MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLL1RGE, (RCC_PLLCFGR_PLL2RGE_3));
		MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLL1VCOSEL, (RCC_PLLCFGR_PLL2VCOSEL));
		SET_BIT(RCC->PLLCFGR, (RCC_PLLCFGR_DIVP1EN));
		SET_BIT(RCC->PLLCFGR, (RCC_PLLCFGR_DIVQ1EN));
		SET_BIT(RCC->PLLCFGR, (RCC_PLLCFGR_DIVR1EN));
		SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLL1FRACEN);
		SET_BIT(RCC->CR, RCC_CR_PLL1ON);
		while(__RCC_GET_FLAG(RCC_FLAG_PLLRDY) != 0U){}

		/*Prepare dividers*/
		MODIFY_REG(RCC->D1CFGR, RCC_D1CFGR_HPRE, RCC_D1CFGR_HPRE_DIV2);
		MODIFY_REG(RCC->D1CFGR, RCC_D1CFGR_D1PPRE, RCC_D1CFGR_D1PPRE_DIV2);
		MODIFY_REG(RCC->D2CFGR, RCC_D2CFGR_D2PPRE1, RCC_D2CFGR_D2PPRE1_DIV2);
		MODIFY_REG(RCC->D2CFGR, RCC_D2CFGR_D2PPRE2, RCC_D2CFGR_D2PPRE2_DIV2);
		MODIFY_REG(RCC->D3CFGR, RCC_D3CFGR_D3PPRE, RCC_D3CFGR_D3PPRE_DIV2);
		MODIFY_REG(RCC->D1CFGR, RCC_D1CFGR_HPRE, RCC_D1CFGR_HPRE_DIV2);
		MODIFY_REG(RCC->D1CFGR, RCC_D1CFGR_D1CPRE, RCC_D1CFGR_D1CPRE_DIV1);

		/*Set flash latency acces*/
		MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_2WS);
		/*Select PLL as core frequence source*/
		MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL1);
		while (  ((uint32_t)(RCC->CFGR & RCC_CFGR_SWS)) !=  (RCC_CFGR_SW_PLL1 << RCC_CFGR_SWS_Pos)){}
	}
}
