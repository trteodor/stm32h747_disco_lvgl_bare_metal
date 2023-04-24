#include "GPIO_h7.h"
#include "is42s32800j.h"
#include "stm32h747xx.h"
#include "sd_ram.h"
#include "DLTuc.h"

static uint32_t FMC_Initialized = 0;
static void FMC_MspInit(void);

void SDRAM_FMC_Init(void)
{
    FMC_MspInit();

    /* FMC_Bank2_SDRAM */
    MODIFY_REG(FMC_SDRAM_DEVICE->SDCR[FMC_SDRAM_BANK1],
               FMC_SDCRx_SDCLK                     |
               FMC_SDCRx_RBURST                    |
               FMC_SDCRx_RPIPE,
               (FMC_SDRAM_CLOCK_PERIOD_2           |
                FMC_SDRAM_RBURST_ENABLE            |
                FMC_SDRAM_RPIPE_DELAY_0));

    MODIFY_REG(FMC_SDRAM_DEVICE->SDCR[FMC_SDRAM_BANK2],
            SDCR_CLEAR_MASK,
            (FMC_SDRAM_COLUMN_BITS_NUM_9           |
                FMC_SDRAM_ROW_BITS_NUM_12          |
                FMC_SDRAM_MEM_BUS_WIDTH_32         |
                FMC_SDRAM_INTERN_BANKS_NUM_4       |
                FMC_SDRAM_CAS_LATENCY_3            |
                FMC_SDRAM_WRITE_PROTECTION_DISABLE ));

    FMC_SDRAM_TimingTypeDef SdramTiming = {0};
    SdramTiming.LoadToActiveDelay = 2;
    SdramTiming.ExitSelfRefreshDelay = 7;
    SdramTiming.SelfRefreshTime = 4;
    SdramTiming.RowCycleDelay = 7;
    SdramTiming.WriteRecoveryTime = 3;
    SdramTiming.RPDelay = 2;
    SdramTiming.RCDDelay = 2;

    MODIFY_REG(FMC_SDRAM_DEVICE->SDTR[FMC_SDRAM_BANK1],
               FMC_SDTRx_TRC |
               FMC_SDTRx_TRP,
               (((SdramTiming.RowCycleDelay) - 1U)         << FMC_SDTRx_TRC_Pos)  |
               (((SdramTiming.RPDelay) - 1U)               << FMC_SDTRx_TRP_Pos));

    MODIFY_REG(FMC_SDRAM_DEVICE->SDTR[FMC_SDRAM_BANK2],
               SDTR_CLEAR_MASK,
               (((SdramTiming.LoadToActiveDelay) - 1U)                                      |
                (((SdramTiming.ExitSelfRefreshDelay) - 1U) << FMC_SDTRx_TXSR_Pos) |
                (((SdramTiming.SelfRefreshTime) - 1U)      << FMC_SDTRx_TRAS_Pos) |
                (((SdramTiming.WriteRecoveryTime) - 1U)    << FMC_SDTRx_TWR_Pos)  |
                (((SdramTiming.RCDDelay) - 1U)             << FMC_SDTRx_TRCD_Pos)));

    #define __FMC_ENABLE()  (FMC_Bank1_R->BTCR[0] |= FMC_BCR1_FMCEN)
    __FMC_ENABLE();

  static IS42S32800J_Context_t pRegMode;

  /* External memory mode register configuration */
  pRegMode.TargetBank      = FMC_SDRAM_CMD_TARGET_BANK2;
  pRegMode.RefreshMode     = IS42S32800J_AUTOREFRESH_MODE_CMD;
  pRegMode.RefreshRate     = REFRESH_COUNT;
  pRegMode.BurstLength     = IS42S32800J_BURST_LENGTH_1;
  pRegMode.BurstType       = IS42S32800J_BURST_TYPE_SEQUENTIAL;
  pRegMode.CASLatency      = IS42S32800J_CAS_LATENCY_3;
  pRegMode.OperationMode   = IS42S32800J_OPERATING_MODE_STANDARD;
  pRegMode.WriteBurstMode  = IS42S32800J_WRITEBURST_MODE_SINGLE;

  /* SDRAM initialization sequence */
  IS42S32800J_Init(&pRegMode);
}


static void FMC_MspInit(void){

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (FMC_Initialized)
    {
        return;
    }
    FMC_Initialized = 1;


    MODIFY_REG(RCC->D1CCIPR, RCC_D1CCIPR_FMCSEL, (uint32_t)(RCC_PERIPHCLK_FMC));
    SET_BIT(RCC->AHB3ENR, RCC_AHB3ENR_FMCEN);


    SET_BIT(RCC->AHB4ENR, RCC_AHB4ENR_GPIOAEN | RCC_AHB4ENR_GPIOBEN | RCC_AHB4ENR_GPIOCEN |
                            RCC_AHB4ENR_GPIODEN | RCC_AHB4ENR_GPIOEEN | RCC_AHB4ENR_GPIOFEN |
                            RCC_AHB4ENR_GPIOGEN | RCC_AHB4ENR_GPIOHEN | RCC_AHB4ENR_GPIOIEN |
                            RCC_AHB4ENR_GPIOJEN | RCC_AHB4ENR_GPIOKEN);

  /* GPIO_InitStruct */
  GPIO_InitStruct.Pin = FMC_D28_Pin|FMC_NBL3_Pin|FMC_NBL2_Pin|FMC_D25_Pin
                          |FMC_D24_Pin|FMC_D29_Pin|FMC_D26_Pin|FMC_D27_Pin
                          |FMC_D30_Pin|FMC_D31_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

  GPIO_Init(GPIOI, &GPIO_InitStruct);

  /* GPIO_InitStruct */
  GPIO_InitStruct.Pin = FMC_NBL1_Pin|FMC_NBL0_Pin|FMC_D7_Pin|FMC_D6_Pin
                          |FMC_D8_Pin|FMC_D9_Pin|FMC_D12_Pin|FMC_D5_Pin
                          |FMC_D10_Pin|FMC_D4_Pin|FMC_D11_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

  GPIO_Init(GPIOE, &GPIO_InitStruct);

  /* GPIO_InitStruct */
  GPIO_InitStruct.Pin = FMC_D23_Pin|FMC_D22_Pin|FMC_D21_Pin|FMC_SDNWE_Pin
                          |FMC_D18_Pin|FMC_D19_Pin|FMC_D17_Pin|FMC_D20_Pin
                          |FMC_SDNE1_Pin|FMC_D16_Pin|FMC_SDCKE1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

  GPIO_Init(GPIOH, &GPIO_InitStruct);

  /* GPIO_InitStruct */
  GPIO_InitStruct.Pin = FMC_SDCAS_Pin|FMC_SDCLK_Pin|GPIO_PIN_5|FMC_BA0_Pin
                          |FMC_A12_Pin|FMC_A10_Pin|FMC_A11_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

  GPIO_Init(GPIOG, &GPIO_InitStruct);

  /* GPIO_InitStruct */
  GPIO_InitStruct.Pin = FMC_D2_Pin|FMC_D3_Pin|FMC_D1_Pin|FMC_D0_Pin
                          |FMC_D15_Pin|FMC_D14_Pin|FMC_D13_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

  GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* GPIO_InitStruct */
  GPIO_InitStruct.Pin = FMC_A2_Pin|FMC_A1_Pin|FMC_A0_Pin|FMC_A3_Pin
                          |FMC_A5_Pin|FMC_A4_Pin|FMC_A7_Pin|FMC_A8_Pin
                          |FMC_A6_Pin|FMC_A9_Pin|FMC_SDRAS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

  GPIO_Init(GPIOF, &GPIO_InitStruct);

  NVIC_SetPriority (FMC_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);
  NVIC_EnableIRQ(FMC_IRQn);
}




/****************************************/
/*Test functions....
/**/
/*
*
*/
static void Fill_Buffer(uint32_t *pBuffer, uint32_t uwBufferLength, uint32_t uwOffset)
{
  uint32_t tmpIndex = 0;

  for (tmpIndex = 0; tmpIndex < uwBufferLength; tmpIndex++ )
  {
    pBuffer[tmpIndex] = tmpIndex + uwOffset;
  }
}


static void SDRAM_Write_32b(uint32_t *pAddress, uint32_t *pSrcBuffer,
                                      uint32_t BufferSize)
{
  uint32_t size;
  __IO uint32_t *pSdramAddress = pAddress;
  uint32_t *psrcbuff = pSrcBuffer;

    /* Write data to memory */
    for (size = BufferSize; size != 0U; size--)
    {
      *pSdramAddress = *psrcbuff;
      psrcbuff++;
      pSdramAddress++;
    }
}


static void SDRAM_Read_32b(uint32_t *pAddress, uint32_t *pDstBuffer,
                                     uint32_t BufferSize)
{
  uint32_t size;
  __IO uint32_t *pSdramAddress = (uint32_t *)pAddress;
  uint32_t *pdestbuff = pDstBuffer;

    /* Read data from source */
    for (size = BufferSize; size != 0U; size--)
    {
      *pdestbuff = *(__IO uint32_t *)pSdramAddress;
      pdestbuff++;
      pSdramAddress++;
    }
}

static uint8_t Buffercmp(uint32_t* pBuffer1, uint32_t* pBuffer2, uint16_t BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer1 != *pBuffer2)
    {
      return 1;
    }

    pBuffer1++;
    pBuffer2++;
  }

  return 0;
}

/**
  * @brief  SDRAM Demo
  * @param  None
  * @retval None
  */
void SDRAM_demo (void)
{
    #define SDRAM_DEVICE_ADDR         0xD0000000U
    #define SDRAM_DEVICE_SIZE         0x2000000U
    #define BUFFER_SIZE            ((uint32_t)0x0100)
    #define WRITE_READ_ADDR        ((uint32_t)0x1000)

    uint32_t sdram_aTxBuffer[BUFFER_SIZE];
    uint32_t sdram_aRxBuffer[BUFFER_SIZE];

    LOG("SDRAM Test start");
    /* Fill the buffer to write */
    Fill_Buffer(sdram_aTxBuffer, BUFFER_SIZE, 0xA244250F);
    /* Write data to the SDRAM memory */
    SDRAM_Write_32b((uint32_t *)(SDRAM_DEVICE_ADDR + WRITE_READ_ADDR), (uint32_t*)sdram_aTxBuffer, BUFFER_SIZE);
    /* Read back data from the SDRAM memory */
    SDRAM_Read_32b((uint32_t *)(SDRAM_DEVICE_ADDR + WRITE_READ_ADDR), (uint32_t*)sdram_aRxBuffer, BUFFER_SIZE);

    if(Buffercmp(sdram_aTxBuffer, sdram_aRxBuffer, BUFFER_SIZE) > 0){
        LOG("SDRAM COMPARE : FAILED.");
    }
    else{
        LOG("SDRAM Test : OK / Succefull");
    }
}