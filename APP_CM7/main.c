#include "stdint.h"
#include "stm32h747xx.h"
#include "GPIO_h7.h"
#include "stdio.h"
#include "UART1_dlt.h"
#include "DLTuc.h"
#include "System.h"
#include "sd_ram.h"
#include "dsihost.h"
#include "dma2d.h"
#include "ltdc.h"
#include "OTM8009A_wrapper.h"
#include "lvglAppMain.h"
#include "qspi.h"

#define SW_VERSION "001"

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
uint32_t BufferMyColor[100*100];

void FillBufer(void)
{
    for(int i=0; i < (100*100); i++)
    {
        BufferMyColor[i] = LCD_COLOR_ARGB8888_CYAN;
    }
}

int main()
{
    PLL_Config480Mhz();
    ConfigSysTick1ms();
    LedsInit();
    UART1InitTransmitWithDMAand_ucDLTlib();
    LOG("Compilation date: %s time: %s Sw_ver: %s", __DATE__, __TIME__,SW_VERSION);

    SDRAM_FMC_Init();
    // SDRAM_test();
    LTDC_Init();
    DSIHOST_DSI_Init();
    // DMA2D_Init();
    OTM8009A_DISP_LCD_Init(0, LCD_ORIENTATION_LANDSCAPE);
    TS_InitIT_OTM8009a();


    QSPI_Test();


    LvglInitApp();

    

    static uint32_t HelpTimer = 0u;


    while(1)
    {
        if(GetSysTime()-HelpTimer>=1000)
        {
            HelpTimer = GetSysTime();
            // LOG("Hello DLT! %lu Iterator: %lu", GetSysTime());
            // tooglePIN(LED3_GPIO_Port,LED2_Pin);
        }

        static uint32_t SavedLvglTime =0;
        if(GetSysTime() -SavedLvglTime >= 5)
        {
            SavedLvglTime = GetSysTime();
            LvglProcesTask();
        }
    }
}