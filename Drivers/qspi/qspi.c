

#include "qspi.h"
#include "GPIO_h7.h"
#include "System.h"
#include "DLTuc.h"

#define NULL ((void *)0)

/* Definition for QSPI Pins */
#define QSPI_BK2_IO2_Pin GPIO_PIN_9
#define QSPI_BK2_IO2_GPIO_Port GPIOG
#define QSPI_BK2_IO3_Pin GPIO_PIN_14
#define QSPI_BK2_IO3_GPIO_Port GPIOG
#define QSPI_BK1_NCS_Pin GPIO_PIN_6
#define QSPI_BK1_NCS_GPIO_Port GPIOG
#define QSPI_BK1_IO3_Pin GPIO_PIN_6
#define QSPI_BK1_IO3_GPIO_Port GPIOF
#define QSPI_BK1_IO2_Pin GPIO_PIN_7
#define QSPI_BK1_IO2_GPIO_Port GPIOF
#define QSPI_BK1_IO1_Pin GPIO_PIN_9
#define QSPI_BK1_IO1_GPIO_Port GPIOF
#define QSPI_BK2_IO0_Pin GPIO_PIN_2
#define QSPI_BK2_IO0_GPIO_Port GPIOH
#define QSPI_BK2_IO1_Pin GPIO_PIN_3
#define QSPI_BK2_IO1_GPIO_Port GPIOH
#define QSPI_CLK_Pin GPIO_PIN_2
#define QSPI_CLK_GPIO_Port GPIOB
#define QSPI_BK1_IO0_Pin GPIO_PIN_11
#define QSPI_BK1_IO0_GPIO_Port GPIOD


#define GPIO_AF10_QUADSPI       ((uint8_t)0x0A)  /* QUADSPI Alternate Function mapping  */
#define GPIO_AF9_QUADSPI       ((uint8_t)0x09)  /* QUADSPI Alternate Function mapping  */

#define QSPI_TIMEOUT_DEFAULT_VALUE 5000U /* 5 s */

#define __QSPI_GET_FLAG(__FLAG__)           ((READ_BIT(QUADSPI->SR, (__FLAG__)) != 0U) ? SET : RESET)

void QSPI_MspInit(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /** Initializes the peripherals clock
  */
#define RCC_PERIPHCLK_QSPI             (0x02000000U)

// MODIFY_REG(RCC->D1CCIPR, RCC_D1CCIPR_QSPISEL, RCC_PERIPHCLK_QSPI);

  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable the QuadSPI memory interface clock */
  SET_BIT(RCC->AHB3ENR, RCC_AHB3ENR_QSPIEN);
  /* Reset the QuadSPI memory interface */
  (RCC->AHB3RSTR |= (RCC_AHB3RSTR_QSPIRST));
  (RCC->AHB3RSTR &= ~ (RCC_AHB3RSTR_QSPIRST));
  /* Enable GPIO clocks */
SET_BIT(RCC->AHB4ENR, RCC_AHB4ENR_GPIOGEN);
SET_BIT(RCC->AHB4ENR, RCC_AHB4ENR_GPIOFEN);
SET_BIT(RCC->AHB4ENR, RCC_AHB4ENR_GPIOHEN);
SET_BIT(RCC->AHB4ENR, RCC_AHB4ENR_GPIOBEN);
SET_BIT(RCC->AHB4ENR, RCC_AHB4ENR_GPIODEN);

    /**QUADSPI GPIO Configuration
    PG9     ------> QUADSPI_BK2_IO2
    PG14     ------> QUADSPI_BK2_IO3
    PG6     ------> QUADSPI_BK1_NCS
    PF6     ------> QUADSPI_BK1_IO3
    PF7     ------> QUADSPI_BK1_IO2
    PF9     ------> QUADSPI_BK1_IO1
    PH2     ------> QUADSPI_BK2_IO0
    PH3     ------> QUADSPI_BK2_IO1
    PB2     ------> QUADSPI_CLK
    PD11     ------> QUADSPI_BK1_IO0
    */
    GPIO_InitStruct.Pin = QSPI_BK2_IO2_Pin|QSPI_BK2_IO3_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
    GPIO_Init(GPIOG, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = QSPI_BK1_NCS_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
    GPIO_Init(QSPI_BK1_NCS_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = QSPI_BK1_IO3_Pin|QSPI_BK1_IO2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
    GPIO_Init(GPIOF, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = QSPI_BK1_IO1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
    GPIO_Init(QSPI_BK1_IO1_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = QSPI_BK2_IO0_Pin|QSPI_BK2_IO1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
    GPIO_Init(GPIOH, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = QSPI_CLK_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
    GPIO_Init(QSPI_CLK_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = QSPI_BK1_IO0_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
    GPIO_Init(QSPI_BK1_IO0_GPIO_Port, &GPIO_InitStruct);

  /*##-3- Configure the NVIC for QSPI #########################################*/
  /* NVIC configuration for QSPI interrupt */
  NVIC_SetPriority (QUADSPI_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);
  NVIC_EnableIRQ(QUADSPI_IRQn);

}

void QSPI_Init(void)
{
    QSPI_MspInit();

    QSPI_InitTypeDef QspiInit;
    QspiInit.ClockPrescaler     = 3;
    QspiInit.FifoThreshold      = 1;
    QspiInit.SampleShifting     = QSPI_SAMPLE_SHIFTING_NONE;
    // QspiInit.FlashSize          = QSPI_FLASH_SIZE;
    QspiInit.FlashSize          = (uint32_t)POSITION_VAL((uint32_t)MT25TL01G_FLASH_SIZE) - 1U; //26 w debuggerze bylo
    QspiInit.ChipSelectHighTime = QSPI_CS_HIGH_TIME_4_CYCLE;
    QspiInit.ClockMode          = QSPI_CLOCK_MODE_0;
    QspiInit.FlashID            = QSPI_FLASH_ID_1;
    QspiInit.DualFlash          = QSPI_DUALFLASH_ENABLE;



  /* Configure QSPI FIFO Threshold */
  MODIFY_REG(QUADSPI->CR, QUADSPI_CR_FTHRES,
             ((QspiInit.FifoThreshold - 1U) << QUADSPI_CR_FTHRES_Pos));
  /* Wait till BUSY flag reset */
  while(__QSPI_GET_FLAG(QUADSPI_SR_BUSY) != RESET);

    /* Configure QSPI Clock Prescaler and Sample Shift */
    MODIFY_REG(QUADSPI->CR, (QUADSPI_CR_PRESCALER | QUADSPI_CR_SSHIFT | QUADSPI_CR_FSEL | QUADSPI_CR_DFM),
               ((QspiInit.ClockPrescaler << QUADSPI_CR_PRESCALER_Pos) |
                QspiInit.SampleShifting  | QspiInit.FlashID | QspiInit.DualFlash));

    /* Configure QSPI Flash Size, CS High Time and Clock Mode */
    MODIFY_REG(QUADSPI->DCR, (QUADSPI_DCR_FSIZE | QUADSPI_DCR_CSHT | QUADSPI_DCR_CKMODE),
               ((QspiInit.FlashSize << QUADSPI_DCR_FSIZE_Pos) |
                QspiInit.ChipSelectHighTime | QspiInit.ClockMode));
    /* Enable the QSPI peripheral */
    SET_BIT(QUADSPI->CR, QUADSPI_CR_EN);
}


static QSPI_Status QSPI_WaitFlagStateUntilTimeout( uint32_t Flag,FlagStatus State,
                                            uint32_t Tickstart, uint32_t Timeout)
{

  uint32_t volatile ret = QSPI_OK;
  uint32_t volatile InTick = Tickstart;
  /* Wait until flag is in expected state */
  while((__QSPI_GET_FLAG(Flag)) != State)
  {
    /* Check for the Timeout */
      if(((GetSysTime() - InTick) > Timeout) || (Timeout == 0U))
      {
        ret = QSPI_ERROR;
        return ret;
      }
  }

  return ret;
}



static void QSPI_Config( QSPI_CommandTypeDef *cmd, uint32_t FunctionalMode)
{
  if ((cmd->DataMode != QSPI_DATA_NONE) && (FunctionalMode != QSPI_FUNCTIONAL_MODE_MEMORY_MAPPED))
  {
    /* Configure QSPI: DLR register with the number of data to read or write */
    WRITE_REG(QUADSPI->DLR, (cmd->NbData - 1U));
  }

  if (cmd->InstructionMode != QSPI_INSTRUCTION_NONE)
  {
    if (cmd->AlternateByteMode != QSPI_ALTERNATE_BYTES_NONE)
    {
      /* Configure QSPI: ABR register with alternate bytes value */
      WRITE_REG(QUADSPI->ABR, cmd->AlternateBytes);

      if (cmd->AddressMode != QSPI_ADDRESS_NONE)
      {
        /*---- Command with instruction, address and alternate bytes ----*/
        /* Configure QSPI: CCR register with all communications parameters */
        WRITE_REG(QUADSPI->CCR, (cmd->DdrMode | cmd->DdrHoldHalfCycle | cmd->SIOOMode |
                                         cmd->DataMode | (cmd->DummyCycles << QUADSPI_CCR_DCYC_Pos) |
                                         cmd->AlternateBytesSize | cmd->AlternateByteMode |
                                         cmd->AddressSize | cmd->AddressMode | cmd->InstructionMode |
                                         cmd->Instruction | FunctionalMode));

        if (FunctionalMode != QSPI_FUNCTIONAL_MODE_MEMORY_MAPPED)
        {
          /* Configure QSPI: AR register with address value */
          WRITE_REG(QUADSPI->AR, cmd->Address);
        }
      }
      else
      {
        /*---- Command with instruction and alternate bytes ----*/
        /* Configure QSPI: CCR register with all communications parameters */
        WRITE_REG(QUADSPI->CCR, (cmd->DdrMode | cmd->DdrHoldHalfCycle | cmd->SIOOMode |
                                         cmd->DataMode | (cmd->DummyCycles << QUADSPI_CCR_DCYC_Pos) |
                                         cmd->AlternateBytesSize | cmd->AlternateByteMode |
                                         cmd->AddressMode | cmd->InstructionMode |
                                         cmd->Instruction | FunctionalMode));
      }
    }
    else
    {
      if (cmd->AddressMode != QSPI_ADDRESS_NONE)
      {
        /*---- Command with instruction and address ----*/
        /* Configure QSPI: CCR register with all communications parameters */
        WRITE_REG(QUADSPI->CCR, (cmd->DdrMode | cmd->DdrHoldHalfCycle | cmd->SIOOMode |
                                         cmd->DataMode | (cmd->DummyCycles << QUADSPI_CCR_DCYC_Pos) |
                                         cmd->AlternateByteMode | cmd->AddressSize | cmd->AddressMode |
                                         cmd->InstructionMode | cmd->Instruction | FunctionalMode));

        if (FunctionalMode != QSPI_FUNCTIONAL_MODE_MEMORY_MAPPED)
        {
          /* Configure QSPI: AR register with address value */
          WRITE_REG(QUADSPI->AR, cmd->Address);
        }
      }
      else
      {
        /*---- Command with only instruction ----*/
        /* Configure QSPI: CCR register with all communications parameters */
        WRITE_REG(QUADSPI->CCR, (cmd->DdrMode | cmd->DdrHoldHalfCycle | cmd->SIOOMode |
                                         cmd->DataMode | (cmd->DummyCycles << QUADSPI_CCR_DCYC_Pos) |
                                         cmd->AlternateByteMode | cmd->AddressMode |
                                         cmd->InstructionMode | cmd->Instruction | FunctionalMode));
      }
    }
  }
  else
  {
    if (cmd->AlternateByteMode != QSPI_ALTERNATE_BYTES_NONE)
    {
      /* Configure QSPI: ABR register with alternate bytes value */
      WRITE_REG(QUADSPI->ABR, cmd->AlternateBytes);

      if (cmd->AddressMode != QSPI_ADDRESS_NONE)
      {
        /*---- Command with address and alternate bytes ----*/
        /* Configure QSPI: CCR register with all communications parameters */
        WRITE_REG(QUADSPI->CCR, (cmd->DdrMode | cmd->DdrHoldHalfCycle | cmd->SIOOMode |
                                         cmd->DataMode | (cmd->DummyCycles << QUADSPI_CCR_DCYC_Pos) |
                                         cmd->AlternateBytesSize | cmd->AlternateByteMode |
                                         cmd->AddressSize | cmd->AddressMode |
                                         cmd->InstructionMode | FunctionalMode));

        if (FunctionalMode != QSPI_FUNCTIONAL_MODE_MEMORY_MAPPED)
        {
          /* Configure QSPI: AR register with address value */
          WRITE_REG(QUADSPI->AR, cmd->Address);
        }
      }
      else
      {
        /*---- Command with only alternate bytes ----*/
        /* Configure QSPI: CCR register with all communications parameters */
        WRITE_REG(QUADSPI->CCR, (cmd->DdrMode | cmd->DdrHoldHalfCycle | cmd->SIOOMode |
                                         cmd->DataMode | (cmd->DummyCycles << QUADSPI_CCR_DCYC_Pos) |
                                         cmd->AlternateBytesSize | cmd->AlternateByteMode |
                                         cmd->AddressMode | cmd->InstructionMode | FunctionalMode));
      }
    }
    else
    {
      if (cmd->AddressMode != QSPI_ADDRESS_NONE)
      {
        /*---- Command with only address ----*/
        /* Configure QSPI: CCR register with all communications parameters */
        WRITE_REG(QUADSPI->CCR, (cmd->DdrMode | cmd->DdrHoldHalfCycle | cmd->SIOOMode |
                                         cmd->DataMode | (cmd->DummyCycles << QUADSPI_CCR_DCYC_Pos) |
                                         cmd->AlternateByteMode | cmd->AddressSize |
                                         cmd->AddressMode | cmd->InstructionMode | FunctionalMode));

        if (FunctionalMode != QSPI_FUNCTIONAL_MODE_MEMORY_MAPPED)
        {
          /* Configure QSPI: AR register with address value */
          WRITE_REG(QUADSPI->AR, cmd->Address);
        }
      }
      else
      {
        /*---- Command with only data phase ----*/
        if (cmd->DataMode != QSPI_DATA_NONE)
        {
          /* Configure QSPI: CCR register with all communications parameters */
          WRITE_REG(QUADSPI->CCR, (cmd->DdrMode | cmd->DdrHoldHalfCycle | cmd->SIOOMode |
                                           cmd->DataMode | (cmd->DummyCycles << QUADSPI_CCR_DCYC_Pos) |
                                           cmd->AlternateByteMode | cmd->AddressMode |
                                           cmd->InstructionMode | FunctionalMode));
        }
      }
    }
  }
}


static QSPI_Status QSPI_Command(QSPI_CommandTypeDef *cmd)
{
  QSPI_Status volatile status = QSPI_ERROR;

  status = QSPI_WaitFlagStateUntilTimeout(QUADSPI_SR_BUSY, RESET, GetSysTime(), 1000);
    /* Wait till BUSY flag reset */
    if(status == QSPI_OK)
    {
      /* Call the configuration function */
      QSPI_Config(cmd, QSPI_FUNCTIONAL_MODE_INDIRECT_WRITE);

      if (cmd->DataMode == QSPI_DATA_NONE)
      {
        /* When there is no data phase, the transfer start as soon as the configuration is done
        so wait until TC flag is set to go back in idle state */
        QSPI_Status status = QSPI_WaitFlagStateUntilTimeout(QUADSPI_SR_TCF, SET, GetSysTime(), 1000);

        if(status == QSPI_OK)
        {
            WRITE_REG(QUADSPI->FCR, (QUADSPI_SR_TCF));
        }
      }
    }

  return status;
}



static QSPI_Status QSPI_Receive(uint8_t *pData)
{
  uint32_t addr_reg = READ_REG(QUADSPI->AR);

  volatile uint32_t *data_reg = &QUADSPI->DR;

    if(pData != NULL )
    {
      /* Update state */
      /* Configure counters and size of the handle */
      uint32_t RxXferCount = READ_REG(QUADSPI->DLR) + 1U;
      // uint32_t RxXferSize = READ_REG(QUADSPI->DLR) + 1U;

      /*TODO:!!!!*/
      uint8_t *pRxBuffPtr = (uint8_t *)pData;

      /* Configure QSPI: CCR register with functional as indirect read */
      MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_FMODE, QSPI_FUNCTIONAL_MODE_INDIRECT_READ);

      /* Start the transfer by re-writing the address in AR register */
      WRITE_REG(QUADSPI->AR, addr_reg);

      while(RxXferCount > 0U)
      {
        /* Wait until FT or TC flag is set to read received data */
        while((__QSPI_GET_FLAG(QUADSPI_SR_FTF | QUADSPI_SR_TCF)) != SET);

        *pRxBuffPtr = *((volatile uint8_t *)data_reg);
        pRxBuffPtr++;
        RxXferCount--;
      }

      while((__QSPI_GET_FLAG(QUADSPI_SR_TCF)) != SET);
      WRITE_REG(QUADSPI->FCR, (QUADSPI_SR_TCF));
    }

    return QSPI_OK;
}

static QSPI_Status QSPI_AutoPolling(QSPI_CommandTypeDef *cmd, QSPI_AutoPollingTypeDef *cfg)
{
    /* Wait till BUSY flag reset */
    volatile QSPI_Status status = QSPI_OK;
    status = QSPI_WaitFlagStateUntilTimeout(QUADSPI_SR_BUSY, RESET, GetSysTime(), 1000);

      if(status == QSPI_OK)
      {
          /* Configure QSPI: PSMAR register with the status match value */
          WRITE_REG(QUADSPI->PSMAR, cfg->Match);

          /* Configure QSPI: PSMKR register with the status mask value */
          WRITE_REG(QUADSPI->PSMKR, cfg->Mask);

          /* Configure QSPI: PIR register with the interval value */
          WRITE_REG(QUADSPI->PIR, cfg->Interval);

          /* Configure QSPI: CR register with Match mode and Automatic stop enabled
          (otherwise there will be an infinite loop in blocking mode) */
          MODIFY_REG(QUADSPI->CR, (QUADSPI_CR_PMM | QUADSPI_CR_APMS),
                  (cfg->MatchMode | QSPI_AUTOMATIC_STOP_ENABLE));

          /* Call the configuration function */
          cmd->NbData = cfg->StatusBytesSize;
          QSPI_Config(cmd, QSPI_FUNCTIONAL_MODE_AUTO_POLLING);
        /* Wait until SM flag is set to go back in idle state */
        QSPI_Status status = QSPI_WaitFlagStateUntilTimeout(QUADSPI_SR_SMF, SET, GetSysTime(), 5000);
          if(status == QSPI_OK)
          {
              WRITE_REG(QUADSPI->FCR, (QUADSPI_SR_SMF));
          }
      }

return status;
}


QSPI_Status MT25TL01G_WriteEnable(MT25TL01G_Interface_t Mode)
{
  volatile QSPI_Status ret = QSPI_OK;
  QSPI_CommandTypeDef     s_command;
  QSPI_AutoPollingTypeDef s_config;

  /* Enable write operations */
  s_command.InstructionMode   = (Mode == MT25TL01G_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;

  s_command.Instruction       = WRITE_ENABLE_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DummyCycles       = 0;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  if (QSPI_Command(&s_command) != QSPI_OK)
  {
    ret = QSPI_ERROR;
    return ret;
  }

  /* Configure automatic polling mode to wait for write enabling */
  s_config.Match           = MT25TL01G_SR_WREN | (MT25TL01G_SR_WREN << 8);
  s_config.Mask            = MT25TL01G_SR_WREN | (MT25TL01G_SR_WREN << 8);
  s_config.MatchMode       = QSPI_MATCH_MODE_AND;
  s_config.StatusBytesSize = 2;
  s_config.Interval        = 0x10;
  s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

  s_command.Instruction    = READ_STATUS_REG_CMD;
  s_command.DataMode       = (Mode == MT25TL01G_QPI_MODE) ? QSPI_DATA_4_LINES : QSPI_DATA_1_LINE;


  if (QSPI_AutoPolling(&s_command, &s_config) != QSPI_OK)
  {
    ret = QSPI_ERROR;
    return ret;
  }

  return ret;
}

static QSPI_Status QSPI_Transmit(uint8_t *pData)
{
  volatile uint32_t *data_reg = &QUADSPI->DR;

    if(pData != NULL )
    {

      /* Configure counters and size of the handle */
      uint32_t TxXferCount = READ_REG(QUADSPI->DLR) + 1U;
      // uint32_t TxXferSize = READ_REG(QUADSPI->DLR) + 1U;

      /*TODO:!!!!*/
      uint8_t *pTxBuffPtr = (uint8_t *)pData;

      /* Configure QSPI: CCR register with functional as indirect write */
      MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_FMODE, QSPI_FUNCTIONAL_MODE_INDIRECT_WRITE);

      while(TxXferCount > 0U)
      {
        /* Wait until FT flag is set to send data */
        while((__QSPI_GET_FLAG(QUADSPI_SR_FTF)) != SET);

        *((volatile uint8_t *)data_reg) = *pTxBuffPtr;
        pTxBuffPtr++;
        TxXferCount--;
      }

        /* Wait until TC flag is set to go back in idle state */
          /* Clear Transfer Complete bit */
      while((__QSPI_GET_FLAG(QUADSPI_SR_TCF)) != SET);
      WRITE_REG(QUADSPI->FCR, (QUADSPI_SR_TCF));
    }

    return QSPI_OK;
}


#warning TODO
//TODO::!!!!
static QSPI_Status QSPI_DummyCyclesCfg(MT25TL01G_Interface_t Mode)
{
  QSPI_CommandTypeDef sCommand;
  uint8_t reg;
  volatile QSPI_Status ret = QSPI_OK;

  /* Read Volatile Configuration register --------------------------- */
  sCommand.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
  sCommand.Instruction       = READ_VOL_CFG_REG_CMD;
  sCommand.AddressMode       = QSPI_ADDRESS_NONE;
  sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode          = QSPI_DATA_4_LINES;
  sCommand.DummyCycles       = 0;
  sCommand.NbData            = 2;
  sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
  sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;


  if(QSPI_Command(&sCommand) != QSPI_OK)
  {
    return QSPI_ERROR;
  }

  if(QSPI_Receive(&reg) != QSPI_OK)
  {
    return QSPI_ERROR;
  }

  /* Enable write operations ---------------------------------------- */
  if(MT25TL01G_WriteEnable(Mode) != QSPI_OK)
  {
    ret = QSPI_ERROR;
    return ret;
  }

  /* Write Volatile Configuration register (with new dummy cycles) -- */  
  sCommand.Instruction = WRITE_VOL_CFG_REG_CMD;
  MODIFY_REG(reg, 0xF0F0, ((DUMMY_CLOCK_CYCLES_READ_QUAD << 4) |
                               (DUMMY_CLOCK_CYCLES_READ_QUAD << 12)));

  if(QSPI_Command(&sCommand) != QSPI_OK)
  {
    ret = QSPI_ERROR;
    return ret;
  }

  if(QSPI_Transmit(&reg) != QSPI_OK)
  {
    ret = QSPI_ERROR;
    return ret;
  }

  return ret;
}

static QSPI_Status QSPI_MemoryMapped(QSPI_CommandTypeDef *cmd, QSPI_MemoryMappedTypeDef *cfg)
{
    /* Wait till BUSY flag reset */
    QSPI_Status status = QSPI_WaitFlagStateUntilTimeout(QUADSPI_SR_BUSY, RESET, GetSysTime(), 1000);
    if(status == QSPI_OK)
    {
        /* Configure QSPI: CR register with timeout counter enable */
      MODIFY_REG(QUADSPI->CR, QUADSPI_CR_TCEN, cfg->TimeOutActivation);

      if (cfg->TimeOutActivation == QSPI_TIMEOUT_COUNTER_ENABLE)
        {
          /* Configure QSPI: LPTR register with the low-power timeout value */
          WRITE_REG(QUADSPI->LPTR, cfg->TimeOutPeriod);

          /* Clear interrupt */
          WRITE_REG(QUADSPI->FCR, QUADSPI_SR_TOF);

          /* Enable the QSPI TimeOut Interrupt */
          SET_BIT(QUADSPI->CR, (QUADSPI_CR_TOIE));
        }
        /* Call the configuration function */
        QSPI_Config(cmd, QSPI_FUNCTIONAL_MODE_MEMORY_MAPPED);
    }

    return QSPI_OK;
}


void QUADSPI_IRQHandler(void)
{
  while(1);
}



#define LED1_Pin GPIO_PIN_12
#define LED1_GPIO_Port GPIOI
#define LED2_Pin GPIO_PIN_13
#define LED2_GPIO_Port GPIOI
#define LED3_Pin GPIO_PIN_14
#define LED3_GPIO_Port GPIOI

static void __attribute__((section(".qspi"), noinline)) GpioToggle(void)
{
  tooglePIN(LED3_GPIO_Port,LED2_Pin);
  /* Insert delay 100 ms */
  DelayMs(100);
}


QSPI_Status MT25TL01G_ResetEnable(MT25TL01G_Interface_t Mode)
{
  QSPI_Status volatile ret=QSPI_OK;
  QSPI_CommandTypeDef s_command;

  /* Initialize the reset enable command */
  s_command.InstructionMode   = (Mode == MT25TL01G_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = RESET_ENABLE_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DummyCycles       = 0;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (QSPI_Command(&s_command) != QSPI_OK)
  {
    ret = QSPI_ERROR;
    return ret;
  }

  return ret;
}

QSPI_Status MT25TL01G_AutoPollingMemReady( MT25TL01G_Interface_t Mode)
{

  QSPI_CommandTypeDef     s_command;
  QSPI_AutoPollingTypeDef s_config;

  /* Configure automatic polling mode to wait for memory ready */
  s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
  s_command.Instruction       = READ_STATUS_REG_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_4_LINES;
  s_command.DummyCycles       = 2;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  s_config.Match           = 0;
  s_config.MatchMode       = QSPI_MATCH_MODE_AND;
  s_config.Interval        = 0x10;
  s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;
  s_config.Mask            = WRITE_STATUS_REG_CMD | (WRITE_STATUS_REG_CMD <<8);
  s_config.StatusBytesSize = 2;

  if (QSPI_AutoPolling(&s_command, &s_config) != QSPI_OK)
  {
    return QSPI_ERROR;
  }

  return QSPI_OK;

}


QSPI_Status MT25TL01G_ResetMemory(MT25TL01G_Interface_t Mode)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the reset enable command */
  s_command.InstructionMode   = (Mode == MT25TL01G_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = RESET_MEMORY_CMD ;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DummyCycles       = 0;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (QSPI_Command(&s_command) != QSPI_OK)
  {
    return QSPI_ERROR;
  }

  return QSPI_OK;
}

QSPI_Status QSPI_ResetMemory(uint32_t InterfaceMode)
{
  QSPI_Status ret = QSPI_OK;

  /* Send RESET ENABLE command in QPI mode (QUAD I/Os, 4-4-4) */
  if(MT25TL01G_ResetEnable(MT25TL01G_QPI_MODE) != QSPI_OK)
  {
    ret =QSPI_ERROR;
  }/* Send RESET memory command in QPI mode (QUAD I/Os, 4-4-4) */
  else if(MT25TL01G_ResetMemory(MT25TL01G_QPI_MODE) != QSPI_OK)
  {
    ret = QSPI_ERROR;
  }/* Wait Flash ready */
  else if(MT25TL01G_AutoPollingMemReady(InterfaceMode) != QSPI_OK)
  {
    ret = QSPI_ERROR;
  }/* Send RESET ENABLE command in SPI mode (1-1-1) */
  else if(MT25TL01G_ResetEnable(MT25TL01G_SPI_MODE) != QSPI_OK)
  {
    ret = QSPI_ERROR;
  }/* Send RESET memory command in SPI mode (1-1-1) */
  else if(MT25TL01G_ResetMemory(MT25TL01G_SPI_MODE) != QSPI_OK)
  {
    ret = QSPI_ERROR;
  }
  /* Return BSP status */
  return ret;
}


QSPI_Status MT25TL01G_Enter4BytesAddressMode(MT25TL01G_Interface_t Mode)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the command */
  s_command.InstructionMode   = (Mode == MT25TL01G_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = ENTER_4_BYTE_ADDR_MODE_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DummyCycles       = 0;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /*write enable */
  if( MT25TL01G_WriteEnable(Mode)!=QSPI_OK)
  {
    return QSPI_ERROR;
  }
  /* Send the command */
  if (QSPI_Command(&s_command) != QSPI_OK)
  {
    return QSPI_ERROR;
  }

  /* Configure automatic polling mode to wait the memory is ready */
  else if(MT25TL01G_AutoPollingMemReady(Mode)!=QSPI_OK)
  {
    return QSPI_ERROR;
  }

  return QSPI_OK;
}


QSPI_Status MT25TL01G_ExitQPIMode(void)
{
  QSPI_CommandTypeDef s_command;

  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = EXIT_QUAD_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DummyCycles       = 0;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  if (QSPI_Command(&s_command) != QSPI_OK)
  {
    return QSPI_ERROR;
  }

  return QSPI_OK;
}


QSPI_Status MT25TL01G_EnterQPIMode(void)
{
  QSPI_CommandTypeDef s_command;

  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = ENTER_QUAD_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DummyCycles       = 0;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  if (QSPI_Command(&s_command) != QSPI_OK)
  {
    return QSPI_ERROR;
  }

  return QSPI_OK;
}


QSPI_Status QSPI_ConfigFlash(MT25TL01G_Interface_t ActualMode, MT25TL01G_Interface_t NewMode)
{
  int32_t ret = QSPI_OK;

        /* Setup Flash interface ***************************************************/
        switch(ActualMode)
        {
        case MT25TL01G_QPI_MODE :               /* 4-4-4 commands */
          if(NewMode != MT25TL01G_QPI_MODE)
          {
            if(MT25TL01G_ExitQPIMode() != QSPI_OK)
            {
              ret = QSPI_ERROR;
            }
          }
          break;

        default :
          if(NewMode == MT25TL01G_QPI_MODE)
          {
            if(MT25TL01G_EnterQPIMode() != QSPI_OK)
            {
              ret = QSPI_ERROR;
            }
          }
          break;
        }
  return ret;
}



QSPI_Status MT25TL01G_EnableMemoryMappedModeDTR(MT25TL01G_Interface_t Mode)
{
  QSPI_CommandTypeDef      s_command;
  QSPI_MemoryMappedTypeDef s_mem_mapped_cfg;
  switch(Mode)
  {
  case MT25TL01G_SPI_MODE:                /* 1-1-1 commands, Power on H/W default setting */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = FAST_READ_4_BYTE_DTR_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
    s_command.DataMode          = QSPI_DATA_1_LINE;

    break;
  case MT25TL01G_SPI_2IO_MODE:           /* 1-1-2 read commands */

    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = DUAL_OUT_FAST_READ_DTR_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
    s_command.DataMode          = QSPI_DATA_2_LINES;

    break;
  case MT25TL01G_SPI_4IO_MODE:             /* 1-4-4 read commands */

    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = QUAD_INOUT_FAST_READ_4_BYTE_ADDR_DTR_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_4_LINES;
    s_command.DataMode          = QSPI_DATA_4_LINES;

    break;
  case MT25TL01G_QPI_MODE:                 /* 4-4-4 commands */
    s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
    s_command.Instruction       = QUAD_INOUT_FAST_READ_DTR_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_4_LINES;
    s_command.DataMode          = QSPI_DATA_4_LINES;

    break;
  }
  /* Configure the command for the read instruction */
  s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = DUMMY_CLOCK_CYCLES_READ_QUAD_DTR;
  s_command.DdrMode           = QSPI_DDR_MODE_ENABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_HALF_CLK_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the memory mapped mode */
  s_mem_mapped_cfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;
  s_mem_mapped_cfg.TimeOutPeriod     = 0;

  if (QSPI_MemoryMapped(&s_command, &s_mem_mapped_cfg) != QSPI_OK)
  {
    return QSPI_ERROR;
  }

  return QSPI_OK;
}

QSPI_Status MT25TL01G_EnableMemoryMappedModeSTR(MT25TL01G_Interface_t Mode)
{
  QSPI_CommandTypeDef      s_command;
  QSPI_MemoryMappedTypeDef s_mem_mapped_cfg;
  switch(Mode)
  {
  case MT25TL01G_SPI_MODE:           /* 1-1-1 read commands */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = FAST_READ_4_BYTE_DTR_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
    s_command.DataMode          = QSPI_DATA_1_LINE;


    break;
  case MT25TL01G_SPI_2IO_MODE:           /* 1-2-2 read commands */

    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = DUAL_OUT_FAST_READ_DTR_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_2_LINES;
    s_command.DataMode          = QSPI_DATA_2_LINES;

    break;

  case MT25TL01G_SPI_4IO_MODE:             /* 1-4-4 read commands */

    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = QUAD_INOUT_FAST_READ_4_BYTE_ADDR_DTR_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_4_LINES;
    s_command.DataMode          = QSPI_DATA_4_LINES;

    break;

  case MT25TL01G_QPI_MODE:                 /* 4-4-4 commands */
    s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
    s_command.Instruction       = QUAD_INOUT_FAST_READ_DTR_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_4_LINES;
    s_command.DataMode          = QSPI_DATA_4_LINES;

    break;

  }
  /* Configure the command for the read instruction */
  s_command.DummyCycles       = DUMMY_CLOCK_CYCLES_READ;
  s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the memory mapped mode */
  s_mem_mapped_cfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;
  s_mem_mapped_cfg.TimeOutPeriod     = 0;

  if (QSPI_MemoryMapped(&s_command, &s_mem_mapped_cfg) != QSPI_OK)
  {
    return QSPI_ERROR;
  }

  return QSPI_OK;
}


QSPI_Status QSPI_EnableMemoryMappedMode(MT25TL01G_Transfer_t TransferRate)
{
  int32_t ret = QSPI_OK;

    if(TransferRate == MT25TL01G_STR_TRANSFER)
    {
      if(MT25TL01G_EnableMemoryMappedModeSTR(MT25TL01G_QPI_MODE) != QSPI_OK)
      {
        ret = QSPI_ERROR;
      }
    }
    else
    {
      if(MT25TL01G_EnableMemoryMappedModeDTR(MT25TL01G_QPI_MODE) != QSPI_OK)
      {
        ret = QSPI_ERROR;
      }
    }

  return ret;
}

void QSPI_Test(void)
{

  QSPI_Init();
  QSPI_Status volatile status = QSPI_OK;

  if(QSPI_ResetMemory(MT25TL01G_SPI_MODE) != QSPI_OK){
    status = QSPI_ERROR;
  }
  else if(MT25TL01G_AutoPollingMemReady(MT25TL01G_SPI_MODE) != QSPI_OK){
    status = QSPI_ERROR;
  }
  else if(MT25TL01G_Enter4BytesAddressMode(MT25TL01G_SPI_MODE) != QSPI_OK){
    status = QSPI_ERROR;
  }
  /* Configuration of the dummy cycles on QSPI memory side */
  else if(QSPI_DummyCyclesCfg(MT25TL01G_SPI_MODE) != QSPI_OK){
    status = QSPI_ERROR;
  }
  else
  {
    /* Configure Flash to desired mode */
    if(QSPI_ConfigFlash(MT25TL01G_SPI_MODE,MT25TL01G_QPI_MODE) != QSPI_OK)
    {

    }

    if(QSPI_EnableMemoryMappedMode(MT25TL01G_DTR_TRANSFER) != QSPI_OK)
    {
      status = QSPI_ERROR;
    };
  }

    LOG("QSPI_InitState %s", status? QSPI_OK:"INIT_SUCCESSFULL", "INIT,FAILED");

    if(status == QSPI_OK)
    {
      LOG("TryExecuteFromQSPI");
      while(1)
      {
      #warning Infinity loop!!!
              GpioToggle();
              LOG("ExecutingFromQSPI");
      }
    }

}





// static void Fill_Buffer(uint8_t *pBuffer, uint32_t uwBufferLenght, uint32_t uwOffset)
// {
//   uint32_t tmpIndex = 0;

//   /* Put in global buffer different values */
//   for (tmpIndex = 0; tmpIndex < uwBufferLenght; tmpIndex++ )
//   {
//     pBuffer[tmpIndex] = tmpIndex + uwOffset;
//   }
// }

// /**
//   * @brief  Compares two buffers.
//   * @param  pBuffer1, pBuffer2: buffers to be compared.
//   * @param  BufferLength: buffer's length
//   * @retval 1: pBuffer identical to pBuffer1
//   *         0: pBuffer differs from pBuffer1
//   */
// static uint8_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength)
// {
//   while (BufferLength--)
//   {
//     if (*pBuffer1 != *pBuffer2)
//     {
//       return 1;
//     }

//     pBuffer1++;
//     pBuffer2++;
//   }

//   return 0;
// }


// #define BUFFER_SIZE         ((uint32_t)0x0200)
// #define WRITE_READ_ADDR     ((uint32_t)0x0050)

// uint8_t qspi_aTxBuffer[BUFFER_SIZE];
// uint8_t qspi_aRxBuffer[BUFFER_SIZE];


// void QSPI_demo (void)
// {
//   /* QSPI info structure */
//   BSP_QSPI_Info_t pQSPI_Info;
//   uint8_t status;

//   /*##-1- Configure the QSPI device ##########################################*/
//   /* QSPI device configuration */
//   QSPI_Init();

//   {
//     LOG("QSPI Initialization : OK.");
//     /*##-2- Read & check the QSPI info #######################################*/
//     /* Initialize the structure */
//     pQSPI_Info.FlashSize          = (uint32_t)0x00;
//     pQSPI_Info.EraseSectorSize    = (uint32_t)0x00;
//     pQSPI_Info.EraseSectorsNumber = (uint32_t)0x00;
//     pQSPI_Info.ProgPageSize       = (uint32_t)0x00;
//     pQSPI_Info.ProgPagesNumber    = (uint32_t)0x00;
//     /* Read the QSPI memory info */
//     BSP_QSPI_GetInfo(0,&pQSPI_Info);
//     /* Test the correctness */
//     if((pQSPI_Info.FlashSize != 0x8000000) || (pQSPI_Info.EraseSectorSize != 0x2000)  ||
//        (pQSPI_Info.ProgPageSize != 0x100)  || (pQSPI_Info.EraseSectorsNumber != 0x4000) ||
//          (pQSPI_Info.ProgPagesNumber != 0x80000))
//     {
//           LOG("QSPI GET INFO : FAILED.");
//     }
//     else
//     {
//                 LOG("QSPI GET INFO : OK.");

//       /*##-3- Erase QSPI memory ################################################*/
//       if(BSP_QSPI_EraseBlock(0,WRITE_READ_ADDR,BSP_QSPI_ERASE_8K) != BSP_ERROR_NONE)
//       {
//         LOG("QSPI ERASE : FAILED.");
//       }
//       else
//       {
//         LOG("QSPI ERASE : OK.");
//         /*##-4- QSPI memory read/write access  #################################*/
//         /* Fill the buffer to write */
//         Fill_Buffer(qspi_aTxBuffer, BUFFER_SIZE, 0xD20F);

//         /* Write data to the QSPI memory */
//         if(BSP_QSPI_Write(0,qspi_aTxBuffer, WRITE_READ_ADDR, BUFFER_SIZE) != BSP_ERROR_NONE)
//         {
//           LOG("QSPI WRITE : FAILED.");
//         }
//         else
//         {
//           LOG("QSPI WRITE : OK.");
//           /* Read back data from the QSPI memory */
//           if(BSP_QSPI_Read(0,qspi_aRxBuffer, WRITE_READ_ADDR, BUFFER_SIZE) != BSP_ERROR_NONE)
//           {
//             LOG("QSPI READ : FAILED.");
//           }
//           else
//           {
//             LOG("QSPI READ : OK.");
//             /*##-5- Checking data integrity ############################################*/
//             if(Buffercmp(qspi_aRxBuffer, qspi_aTxBuffer, BUFFER_SIZE) > 0)
//             {
//               LOG("QSPI COMPARE : FAILED.");
//            }

//             }
//           }
//         }
//       }
//     }
//   }
// }


