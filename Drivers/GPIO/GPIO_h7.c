
#include "GPIO_h7.h"
#include "stm32h7xx.h"



void GPIO_Init(GPIO_TypeDef  *GPIOx, GPIO_InitTypeDef *GPIO_Init)
{
  uint32_t position = 0x00U;
  uint32_t iocurrent;
  uint32_t temp;
  EXTI_Core_TypeDef *EXTI_CurrentCPU;

  EXTI_CurrentCPU = EXTI_D1; /* EXTI for CM7 CPU */

  /* Configure the port pins */
  while (((GPIO_Init->Pin) >> position) != 0x00U)
  {
    /* Get current io position */
    iocurrent = (GPIO_Init->Pin) & (1UL << position);

    if (iocurrent != 0x00U)
    {
      /*--------------------- GPIO Mode Configuration ------------------------*/
      /* In case of Output or Alternate function mode selection */
      if (((GPIO_Init->Mode & GPIO_MODE) == MODE_OUTPUT) || ((GPIO_Init->Mode & GPIO_MODE) == MODE_AF))
      {
        /* Configure the IO Speed */
        temp = GPIOx->OSPEEDR;
        temp &= ~(GPIO_OSPEEDR_OSPEED0 << (position * 2U));
        temp |= (GPIO_Init->Speed << (position * 2U));
        GPIOx->OSPEEDR = temp;

        /* Configure the IO Output Type */
        temp = GPIOx->OTYPER;
        temp &= ~(GPIO_OTYPER_OT0 << position) ;
        temp |= (((GPIO_Init->Mode & OUTPUT_TYPE) >> OUTPUT_TYPE_Pos) << position);
        GPIOx->OTYPER = temp;
      }

      if ((GPIO_Init->Mode & GPIO_MODE) != MODE_ANALOG)
      {
      /* Activate the Pull-up or Pull down resistor for the current IO */
      temp = GPIOx->PUPDR;
      temp &= ~(GPIO_PUPDR_PUPD0 << (position * 2U));
      temp |= ((GPIO_Init->Pull) << (position * 2U));
      GPIOx->PUPDR = temp;
      }

      /* In case of Alternate function mode selection */
      if ((GPIO_Init->Mode & GPIO_MODE) == MODE_AF)
      {
        /* Configure Alternate function mapped with the current IO */
        temp = GPIOx->AFR[position >> 3U];
        temp &= ~(0xFU << ((position & 0x07U) * 4U));
        temp |= ((GPIO_Init->Alternate) << ((position & 0x07U) * 4U));
        GPIOx->AFR[position >> 3U] = temp;
      }

      /* Configure IO Direction mode (Input, Output, Alternate or Analog) */
      temp = GPIOx->MODER;
      temp &= ~(GPIO_MODER_MODE0 << (position * 2U));
      temp |= ((GPIO_Init->Mode & GPIO_MODE) << (position * 2U));
      GPIOx->MODER = temp;

      /*--------------------- EXTI Mode Configuration ------------------------*/
      /* Configure the External Interrupt or event for the current IO */
      if ((GPIO_Init->Mode & EXTI_MODE) != 0x00U)
      {
        /* Enable SYSCFG Clock */
        SET_BIT(RCC->APB4ENR, RCC_APB4ENR_SYSCFGEN);

        temp = SYSCFG->EXTICR[position >> 2U];
        temp &= ~(0x0FUL << (4U * (position & 0x03U)));
        temp |= (GPIO_GET_INDEX(GPIOx) << (4U * (position & 0x03U)));
        SYSCFG->EXTICR[position >> 2U] = temp;

        /* Clear Rising Falling edge configuration */
        temp = EXTI->RTSR1;
        temp &= ~(iocurrent);
        if ((GPIO_Init->Mode & TRIGGER_RISING) != 0x00U)
        {
          temp |= iocurrent;
        }
        EXTI->RTSR1 = temp;

        temp = EXTI->FTSR1;
        temp &= ~(iocurrent);
        if ((GPIO_Init->Mode & TRIGGER_FALLING) != 0x00U)
        {
          temp |= iocurrent;
        }
        EXTI->FTSR1 = temp;

        temp = EXTI_CurrentCPU->EMR1;
        temp &= ~(iocurrent);
        if ((GPIO_Init->Mode & EXTI_EVT) != 0x00U)
        {
          temp |= iocurrent;
        }
        EXTI_CurrentCPU->EMR1 = temp;

        /* Clear EXTI line configuration */
        temp = EXTI_CurrentCPU->IMR1;
        temp &= ~(iocurrent);
        if ((GPIO_Init->Mode & EXTI_IT) != 0x00U)
        {
          temp |= iocurrent;
        }
        EXTI_CurrentCPU->IMR1 = temp;
      }
    }

    position++;
  }
}


void tooglePIN(GPIO_TypeDef *GPIOx, GPIO_PinMask_t GPIO_Pin)
{
  uint32_t odr;
  odr = GPIOx->ODR;
  GPIOx->BSRR = ((odr & GPIO_Pin) << GPIO_NUMBER) | (~odr & GPIO_Pin); //taki trick
}


void GPIO_PinSet(GPIO_TypeDef *GPIOx, GPIO_PinMask_t GPIO_Pin)
{
  GPIOx->BSRR = GPIO_Pin;
}

void GPIO_PinReset(GPIO_TypeDef *GPIOx, GPIO_PinMask_t GPIO_Pin)
{
  uint32_t odr;
  odr = GPIOx->ODR;
  GPIOx->BSRR = ((odr & GPIO_Pin) << GPIO_NUMBER);
}
