
#include "GPIO_h7.h"
#include "i2c4.h"
#include "stm32h7xx.h"

void I2C4_MspInit(void);



#define __I2C_CLEAR_FLAG(__FLAG__) (((__FLAG__) == I2C_ISR_TXE) ? \
                                                    ((I2C4)->ISR |= (__FLAG__)) : \
                                                    ((I2C4)->ICR = (__FLAG__)))


#define __I2C_GET_FLAG(__FLAG__) (((((I2C4)->ISR) & (__FLAG__)) == (__FLAG__)) ? SET : RESET)


#define I2C_RESET_CR2()                 ((I2C4)->CR2 &= \
                                                   (uint32_t)~((uint32_t)(I2C_CR2_SADD   | I2C_CR2_HEAD10R | \
                                                                          I2C_CR2_NBYTES | I2C_CR2_RELOAD  | \
                                                                          I2C_CR2_RD_WRN)))





void I2C4_Init(void)
{
  uint32_t tmpreg;

  I2C4_MspInit();

  CLEAR_BIT(I2C4->CR1, I2C_CR1_PE);

  /*---------------------------- I2Cx TIMINGR Configuration ------------------*/
  /* Configure I2Cx: Frequency range */
  I2C4->TIMINGR = 0x00602173 & TIMING_CLEAR_MASK;
  /*---------------------------- I2Cx OAR1 Configuration ---------------------*/
  /* Disable Own Address1 before set the Own Address1 configuration */
  I2C4->OAR1 &= ~I2C_OAR1_OA1EN;
  /* Configure I2Cx: Own Address1 and ack own address1 mode */
  I2C4->OAR1 = (I2C_OAR1_OA1EN | 0); //I2C_ADDRESSINGMODE_7BIT
  /*---------------------------- I2Cx CR2 Configuration ----------------------*/
  /* Configure I2Cx: Addressing Master mode */

  /* Enable the AUTOEND by default, and enable NACK (should be disable only during Slave process */
  I2C4->CR2 |= (I2C_CR2_AUTOEND | I2C_CR2_NACK);
  /*---------------------------- I2Cx OAR2 Configuration ---------------------*/
  /* Disable Own Address2 before set the Own Address2 configuration */
  I2C4->OAR2 &= ~I2C_DUALADDRESS_ENABLE;

  /* Configure I2Cx: Dual mode and Own Address2 */
  I2C4->OAR2 = (I2C_DUALADDRESS_DISABLE | 0 | \
                          (I2C_OA2_NOMASK << 8));
  /*---------------------------- I2Cx CR1 Configuration ----------------------*/
  /* Configure I2Cx: Generalcall and NoStretch mode */
  I2C4->CR1 = (I2C_GENERALCALL_DISABLE | I2C_NOSTRETCH_DISABLE);



  /* Reset I2Cx ANOFF bit */
  I2C4->CR1 &= ~(I2C_CR1_ANFOFF);

  /* Set analog filter bit*/
  I2C4->CR1 |= I2C_ANALOGFILTER_ENABLE;

      /* Get the old register value */
  tmpreg = I2C4->CR1;

  /* Reset I2Cx DNF bits [11:8] */
  tmpreg &= ~(I2C_CR1_DNF);

  /* Set I2Cx DNF coefficient */
  tmpreg |= 0 << 8U;

  /* Store the new register value */
  I2C4->CR1 = tmpreg;


  /* Enable the selected I2C peripheral */
  SET_BIT(I2C4->CR1, I2C_CR1_PE);

}



void I2C4_MspInit(void)
{

    GPIO_InitTypeDef GPIO_InitStruct = {0};


    MODIFY_REG(RCC->D3CCIPR, RCC_D3CCIPR_I2C4SEL, (uint32_t)(RCC_I2C4CLKSOURCE_HSI));

    SET_BIT(RCC->AHB4ENR, RCC_AHB4ENR_GPIODEN);

    GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C4;
    GPIO_Init(GPIOD, &GPIO_InitStruct);

    SET_BIT(RCC->APB4ENR, RCC_APB4ENR_I2C4EN);


    NVIC_SetPriority (I2C4_EV_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);
    NVIC_EnableIRQ(I2C4_EV_IRQn);
    NVIC_SetPriority (I2C4_ER_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);
    NVIC_EnableIRQ(I2C4_ER_IRQn);
}


static void I2C4_TransferConfig(uint16_t DevAddress, uint8_t Size, uint32_t Mode,
                               uint32_t Request)
{
  /* Declaration of tmp to prevent undefined behavior of volatile usage */
  uint32_t tmp = ((uint32_t)(((uint32_t)DevAddress & I2C_CR2_SADD) | \
                            (((uint32_t)Size << I2C_CR2_NBYTES_Pos) & I2C_CR2_NBYTES) | \
                              (uint32_t)Mode | (uint32_t)Request) & (~0x80000000U));

  /* update CR2 register */
  MODIFY_REG(I2C4->CR2, \
             ((I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND | \
               (I2C_CR2_RD_WRN & (uint32_t)(Request >> (31U - I2C_CR2_RD_WRN_Pos))) | \
                I2C_CR2_START | I2C_CR2_STOP)), tmp);
}




/**
  * @brief  Read an amount of data in blocking mode from a specific memory address
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  DevAddress Target device address: The device 7 bits address value
  *         in datasheet must be shifted to the left before calling the interface
  * @param  MemAddress Internal memory address
  * @param  MemAddSize Size of internal memory address
  * @param  pData Pointer to data buffer
  * @param  Size Amount of data to be sent
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
void I2C4_Mem_Read(uint16_t DevAddress, uint16_t MemAddress,
                                   uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    while (__I2C_GET_FLAG(I2C_ISR_BUSY) == SET){}
    while (__I2C_GET_FLAG(I2C_ISR_TXIS) == SET){}


    /* Send Slave Address and Memory Address */
    I2C4_TransferConfig(DevAddress, (uint8_t)MemAddSize, I2C_SOFTEND_MODE, I2C_GENERATE_START_WRITE);
    while (__I2C_GET_FLAG(I2C_ISR_TXIS) != RESET){}
    /* Send Memory Address */
    I2C4->TXDR = MemAddress;
    // while (__I2C_GET_FLAG(I2C_ISR_TXIS) != RESET){}
    while (__I2C_GET_FLAG(I2C_ISR_TC) == RESET){}


    /* Send Slave Address */
    I2C4_TransferConfig(DevAddress, (uint8_t)Size, I2C_AUTOEND_MODE,
                         I2C_GENERATE_START_READ);

    do
    {
      /* Wait until RXNE flag is set */
      while (__I2C_GET_FLAG(I2C_ISR_RXNE) != SET){};

      /* Read data from RXDR */
      *pData = (uint8_t)I2C4->RXDR;

      /* Increment Buffer pointer */
      pData++;
      Size--;
    } while (Size > 0U);

    /* No need to Check TC flag, with AUTOEND mode the stop is automatically generated */
    /* Wait until STOPF flag is reset */
    while (__I2C_GET_FLAG(I2C_ISR_STOPF) == RESET){}
    /* Clear STOP Flag */
    __I2C_CLEAR_FLAG(I2C_ISR_STOPF);
    /* Clear Configuration Register 2 */
    I2C_RESET_CR2();

}


/**
  * @brief  Write an amount of data in blocking mode to a specific memory address
  * @param  DevAddress Target device address: The device 7 bits address value
  *         in datasheet must be shifted to the left before calling the interface
  * @param  MemAddress Internal memory address
  * @param  MemAddSize Size of internal memory address
  * @param  pData Pointer to data buffer
  * @param  Size Amount of data to be sent
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
void I2C4_Mem_Write(uint16_t DevAddress, uint16_t MemAddress,
                                    uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    while (__I2C_GET_FLAG(I2C_ISR_BUSY) == SET){}
    while (__I2C_GET_FLAG(I2C_ISR_TXIS) == SET){}
    /* Prepare transfer parameters */

    /* Send Slave Address and Memory Address */
    I2C4_TransferConfig(DevAddress, (uint8_t)MemAddSize, I2C_RELOAD_MODE, I2C_GENERATE_START_WRITE);
    while (__I2C_GET_FLAG(I2C_ISR_TXIS) == RESET){}
    I2C4->TXDR = MemAddress;
    while (__I2C_GET_FLAG(I2C_ISR_TCR) == RESET){}

    /* Set NBYTES to write and reload if hi2c->XferCount > MAX_NBYTE_SIZE */
    I2C4_TransferConfig(DevAddress, (uint8_t)Size, I2C_AUTOEND_MODE, I2C_NO_STARTSTOP);

    do
    {
      /* Wait until TXIS flag is set */
      while (__I2C_GET_FLAG(I2C_ISR_TXIS) == RESET){}
      /* Write data to TXDR */
      I2C4->TXDR = *pData;
      /* Increment Buffer pointer */
      pData++;
      Size--;
    } while (Size > 0U);

    /* No need to Check TC flag, with AUTOEND mode the stop is automatically generated */
    /* Wait until STOPF flag is reset */
    while (__I2C_GET_FLAG(I2C_ISR_STOPF) == RESET){}
    /* Clear STOP Flag */
    __I2C_CLEAR_FLAG(I2C_ISR_STOPF);

    /* Clear Configuration Register 2 */
    I2C_RESET_CR2();
}

