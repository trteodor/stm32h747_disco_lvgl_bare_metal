#include "stdint.h"
#include "stm32h747xx.h"
#include "GPIO_h7.h"
#include "stdio.h"
#include "UART1_dlt.h"
#include "DLTuc.h"
#include "System.h"

/******************************************************************/
#define LED1_Pin GPIO_PIN_12
#define LED1_GPIO_Port GPIOI
#define LED2_Pin GPIO_PIN_13
#define LED2_GPIO_Port GPIOI
#define LED3_Pin GPIO_PIN_14
#define LED3_GPIO_Port GPIOI
/******************************************************************/
void LedsInit(void)
{
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOIEN;

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = LED2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    GPIO_Init(LED3_GPIO_Port, &GPIO_InitStruct);
}
/******************************************************************/
int main()
{
    LedsInit();
    GPIO_PinReset(LED3_GPIO_Port,LED2_Pin);

    PLL_Config400Mhz();
    ConfigSysTick1ms();
    UART1InitTransmitWithDMAand_ucDLTlib();

    static uint32_t HelpTimer = 0u;
    uint32_t Iterator = 0U;

    while(1)
    {
        if(GetSysTime()-HelpTimer>=1000)
        {
            HelpTimer = GetSysTime();
            LOG("Hello DLT! %lu Iterator: %d", GetSysTime() ,Iterator);
            tooglePIN(LED3_GPIO_Port,LED2_Pin);

        }
        Iterator++;
    }
}