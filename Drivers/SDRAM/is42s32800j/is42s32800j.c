
#include "stdint.h"
#include "stm32h7xx.h"
#include "is42s32800j.h"
#include "System.h"


static FMC_SDRAM_CommandTypeDef Command;
static void IS42S32800J_Delay(uint32_t Delay);

#define FMC_SDRAM_DEVICE               FMC_Bank5_6_R

void IS42S32800J_Init(IS42S32800J_Context_t *pRegMode)
{
  /* Step 1: Configure a clock configuration enable command */
  IS42S32800J_ClockEnable(pRegMode->TargetBank);
  /* Step 2: Insert 100 us minimum delay */
  /* Inserted delay is equal to 1 ms due to systick time base unit (ms) */
  IS42S32800J_Delay(1);

  /* Step 3: Configure a PALL (precharge all) command */
  IS42S32800J_Precharge(pRegMode->TargetBank);
  /* Step 4: Configure a Refresh command */
  IS42S32800J_RefreshMode(pRegMode->TargetBank, pRegMode->RefreshMode);

  /* Step 5: Program the external memory mode register */
  IS42S32800J_ModeRegConfig(pRegMode);

  /* Step 6: Set the refresh rate counter */
  IS42S32800J_RefreshRate(pRegMode->RefreshRate);
}

void IS42S32800J_ClockEnable(uint32_t Interface)
{
  Command.CommandMode            = IS42S32800J_CLK_ENABLE_CMD;
  Command.CommandTarget          = Interface;
  Command.AutoRefreshNumber      = 1;
  Command.ModeRegisterDefinition = 0;

  MODIFY_REG(FMC_SDRAM_DEVICE->SDCMR, (FMC_SDCMR_MODE | FMC_SDCMR_CTB2 | FMC_SDCMR_CTB1 | FMC_SDCMR_NRFS | FMC_SDCMR_MRD),
             ((Command.CommandMode) | (Command.CommandTarget) |
              (((Command.AutoRefreshNumber) - 1U) << FMC_SDCMR_NRFS_Pos) |
              ((Command.ModeRegisterDefinition) << FMC_SDCMR_MRD_Pos)));
}

void IS42S32800J_Precharge(uint32_t Interface) 
{
  Command.CommandMode            = IS42S32800J_PALL_CMD;
  Command.CommandTarget          = Interface;
  Command.AutoRefreshNumber      = 1;
  Command.ModeRegisterDefinition = 0;

  MODIFY_REG(FMC_SDRAM_DEVICE->SDCMR, (FMC_SDCMR_MODE | FMC_SDCMR_CTB2 | FMC_SDCMR_CTB1 | FMC_SDCMR_NRFS | FMC_SDCMR_MRD),
             ((Command.CommandMode) | (Command.CommandTarget) |
              (((Command.AutoRefreshNumber) - 1U) << FMC_SDCMR_NRFS_Pos) |
              ((Command.ModeRegisterDefinition) << FMC_SDCMR_MRD_Pos)));
}

void IS42S32800J_ModeRegConfig(IS42S32800J_Context_t *pRegMode) 
{
  uint32_t tmpmrd;
  /* Program the external memory mode register */
  tmpmrd = (uint32_t)pRegMode->BurstLength   |\
                     pRegMode->BurstType     |\
                     pRegMode->CASLatency    |\
                     pRegMode->OperationMode |\
                     pRegMode->WriteBurstMode;

  Command.CommandMode            = IS42S32800J_LOAD_MODE_CMD;
  Command.CommandTarget          = pRegMode->TargetBank;
  Command.AutoRefreshNumber      = 1;
  Command.ModeRegisterDefinition = tmpmrd;

  /* Send the command */
  MODIFY_REG(FMC_SDRAM_DEVICE->SDCMR, (FMC_SDCMR_MODE | FMC_SDCMR_CTB2 | FMC_SDCMR_CTB1 | FMC_SDCMR_NRFS | FMC_SDCMR_MRD),
             ((Command.CommandMode) | (Command.CommandTarget) |
              (((Command.AutoRefreshNumber) - 1U) << FMC_SDCMR_NRFS_Pos) |
              ((Command.ModeRegisterDefinition) << FMC_SDCMR_MRD_Pos)));
}

void IS42S32800J_RefreshMode(uint32_t Interface, uint32_t RefreshMode)
{
  Command.CommandMode            = RefreshMode;
  Command.CommandTarget          = Interface;
  Command.AutoRefreshNumber      = 8;
  Command.ModeRegisterDefinition = 0;

  /* Send the command */
  MODIFY_REG(FMC_SDRAM_DEVICE->SDCMR, (FMC_SDCMR_MODE | FMC_SDCMR_CTB2 | FMC_SDCMR_CTB1 | FMC_SDCMR_NRFS | FMC_SDCMR_MRD),
             ((Command.CommandMode) | (Command.CommandTarget) |
              (((Command.AutoRefreshNumber) - 1U) << FMC_SDCMR_NRFS_Pos) |
              ((Command.ModeRegisterDefinition) << FMC_SDCMR_MRD_Pos)));
}

void IS42S32800J_RefreshRate(uint32_t RefreshRate)
{
    MODIFY_REG(FMC_SDRAM_DEVICE->SDRTR, FMC_SDRTR_COUNT, (RefreshRate << FMC_SDRTR_COUNT_Pos));
}

void IS42S32800J_Sendcmd(FMC_SDRAM_CommandTypeDef *SdramCmd)
{
  MODIFY_REG(FMC_SDRAM_DEVICE->SDCMR, (FMC_SDCMR_MODE | FMC_SDCMR_CTB2 | FMC_SDCMR_CTB1 | FMC_SDCMR_NRFS | FMC_SDCMR_MRD),
             ((Command.CommandMode) | (Command.CommandTarget) |
              (((Command.AutoRefreshNumber) - 1U) << FMC_SDCMR_NRFS_Pos) |
              ((Command.ModeRegisterDefinition) << FMC_SDCMR_MRD_Pos)));
}

void IS42S32800J_Delay(uint32_t Delay)
{
  uint32_t tickstart;
  tickstart = GetSysTime();
  while((GetSysTime() - tickstart) < Delay)
  {
  }
}

