
#ifndef __GPIO_H_
#define __GPIO_H_

#include "stm32h7xx.h"



#define GPIO_NUMBER           16u



#define GPIO_MODE_Pos                           0u
#define GPIO_MODE                               (0x3uL << GPIO_MODE_Pos)
#define MODE_INPUT                              (0x0uL << GPIO_MODE_Pos)
#define MODE_OUTPUT                             (0x1uL << GPIO_MODE_Pos)
#define MODE_AF                                 (0x2uL << GPIO_MODE_Pos)
#define MODE_ANALOG                             (0x3uL << GPIO_MODE_Pos)
#define OUTPUT_TYPE_Pos                         4u
#define OUTPUT_TYPE                             (0x1uL << OUTPUT_TYPE_Pos)
#define OUTPUT_PP                               (0x0uL << OUTPUT_TYPE_Pos)
#define OUTPUT_OD                               (0x1uL << OUTPUT_TYPE_Pos)
#define EXTI_MODE_Pos                           16u
#define EXTI_MODE                               (0x3uL << EXTI_MODE_Pos)
#define EXTI_IT                                 (0x1uL << EXTI_MODE_Pos)
#define EXTI_EVT                                (0x2uL << EXTI_MODE_Pos)
#define TRIGGER_MODE_Pos                         20u
#define TRIGGER_MODE                            (0x7uL << TRIGGER_MODE_Pos)
#define TRIGGER_RISING                          (0x1uL << TRIGGER_MODE_Pos)
#define TRIGGER_FALLING                         (0x2uL << TRIGGER_MODE_Pos)
#define TRIGGER_LEVEL                           (0x4uL << TRIGGER_MODE_Pos)


#define GPIO_AF11_ETH           ((uint8_t)0x0BU)  /* ETHERNET Alternate Function mapping */

#define  GPIO_SPEED_FREQ_LOW         0x00000000U  /*!< IO works at 2 MHz, please refer to the product datasheet */
#define  GPIO_SPEED_FREQ_MEDIUM      0x00000001U  /*!< range 12,5 MHz to 50 MHz, please refer to the product datasheet */
#define  GPIO_SPEED_FREQ_HIGH        0x00000002U  /*!< range 25 MHz to 100 MHz, please refer to the product datasheet  */
#define  GPIO_SPEED_FREQ_VERY_HIGH   0x00000003U  /*!< range 50 MHz to 200 MHz, please refer to the product datasheet  */


#define GPIO_MODE_INPUT                 MODE_INPUT                                                  /*!< Input Floating Mode                                                */
#define GPIO_MODE_OUTPUT_PP             (MODE_OUTPUT | OUTPUT_PP)                                   /*!< Output Push Pull Mode                                              */
#define GPIO_MODE_OUTPUT_OD             (MODE_OUTPUT | OUTPUT_OD)                                   /*!< Output Open Drain Mode                                             */
#define GPIO_MODE_AF_PP                 (MODE_AF | OUTPUT_PP)                                       /*!< Alternate Function Push Pull Mode                                  */
#define GPIO_MODE_AF_OD                 (MODE_AF | OUTPUT_OD)                                       /*!< Alternate Function Open Drain Mode                                 */
#define GPIO_MODE_ANALOG                MODE_ANALOG                                                 /*!< Analog Mode                                                        */
#define GPIO_MODE_IT_RISING             (MODE_INPUT | EXTI_IT | TRIGGER_RISING)                     /*!< External Interrupt Mode with Rising edge trigger detection         */
#define GPIO_MODE_IT_FALLING            (MODE_INPUT | EXTI_IT | TRIGGER_FALLING)                    /*!< External Interrupt Mode with Falling edge trigger detection        */
#define GPIO_MODE_IT_RISING_FALLING     (MODE_INPUT | EXTI_IT | TRIGGER_RISING | TRIGGER_FALLING)   /*!< External Interrupt Mode with Rising/Falling edge trigger detection */

#define GPIO_MODE_EVT_RISING            (MODE_INPUT | EXTI_EVT | TRIGGER_RISING)                    /*!< External Event Mode with Rising edge trigger detection             */
#define GPIO_MODE_EVT_FALLING           (MODE_INPUT | EXTI_EVT | TRIGGER_FALLING)                   /*!< External Event Mode with Falling edge trigger detection            */
#define GPIO_MODE_EVT_RISING_FALLING    (MODE_INPUT | EXTI_EVT | TRIGGER_RISING | TRIGGER_FALLING)  /*!< External Event Mode with Rising/Falling edge trigger detection     */


#define  GPIO_NOPULL        0x00000000U   /*!< No Pull-up or Pull-down activation  */
#define  GPIO_PULLUP        0x00000001U   /*!< Pull-up activation                  */
#define  GPIO_PULLDOWN      0x00000002U   /*!< Pull-down activation                */


#define GPIO_PIN_0                 ((uint16_t)0x0001)  /* Pin 0 selected    */
#define GPIO_PIN_1                 ((uint16_t)0x0002)  /* Pin 1 selected    */
#define GPIO_PIN_2                 ((uint16_t)0x0004)  /* Pin 2 selected    */
#define GPIO_PIN_3                 ((uint16_t)0x0008)  /* Pin 3 selected    */
#define GPIO_PIN_4                 ((uint16_t)0x0010)  /* Pin 4 selected    */
#define GPIO_PIN_5                 ((uint16_t)0x0020)  /* Pin 5 selected    */
#define GPIO_PIN_6                 ((uint16_t)0x0040)  /* Pin 6 selected    */
#define GPIO_PIN_7                 ((uint16_t)0x0080)  /* Pin 7 selected    */
#define GPIO_PIN_8                 ((uint16_t)0x0100)  /* Pin 8 selected    */
#define GPIO_PIN_9                 ((uint16_t)0x0200)  /* Pin 9 selected    */
#define GPIO_PIN_10                ((uint16_t)0x0400)  /* Pin 10 selected   */
#define GPIO_PIN_11                ((uint16_t)0x0800)  /* Pin 11 selected   */
#define GPIO_PIN_12                ((uint16_t)0x1000)  /* Pin 12 selected   */
#define GPIO_PIN_13                ((uint16_t)0x2000)  /* Pin 13 selected   */
#define GPIO_PIN_14                ((uint16_t)0x4000)  /* Pin 14 selected   */
#define GPIO_PIN_15                ((uint16_t)0x8000)  /* Pin 15 selected   */
#define GPIO_PIN_All               ((uint16_t)0xFFFF)  /* All pins selected */


/** @defgroup GPIOEx_Get_Port_Index GPIO Get Port Index
  * @{
  */
#if defined(GPIOI)
#define GPIO_GET_INDEX(__GPIOx__)  (((__GPIOx__) == (GPIOA))? 0UL :\
                                    ((__GPIOx__) == (GPIOB))? 1UL :\
                                    ((__GPIOx__) == (GPIOC))? 2UL :\
                                    ((__GPIOx__) == (GPIOD))? 3UL :\
                                    ((__GPIOx__) == (GPIOE))? 4UL :\
                                    ((__GPIOx__) == (GPIOF))? 5UL :\
                                    ((__GPIOx__) == (GPIOG))? 6UL :\
                                    ((__GPIOx__) == (GPIOH))? 7UL :\
                                    ((__GPIOx__) == (GPIOI))? 8UL :\
                                    ((__GPIOx__) == (GPIOJ))? 9UL : 10UL)
#else
#define GPIO_GET_INDEX(__GPIOx__)  (((__GPIOx__) == (GPIOA))? 0UL :\
                                    ((__GPIOx__) == (GPIOB))? 1UL :\
                                    ((__GPIOx__) == (GPIOC))? 2UL :\
                                    ((__GPIOx__) == (GPIOD))? 3UL :\
                                    ((__GPIOx__) == (GPIOE))? 4UL :\
                                    ((__GPIOx__) == (GPIOF))? 5UL :\
                                    ((__GPIOx__) == (GPIOG))? 6UL :\
                                    ((__GPIOx__) == (GPIOH))? 7UL :\
                                    ((__GPIOx__) == (GPIOJ))? 9UL : 10UL)
#endif /* GPIOI */



/*
Application pin defines
*/

/*
Application pin end
*/


typedef struct
{
  uint32_t Pin;
  uint32_t Mode;
  uint32_t Pull; 
  uint32_t Speed;
  uint32_t Alternate;
}GPIO_InitTypeDef;

/*GPIO Pin state mask*/
typedef enum {
 Psm0 = 0x00000001,
 Psm1 = 0x00000002,
 Psm2 = 0x00000004,
 Psm3 = 0x00000008,
 Psm4 = 0x00000010,
 Psm5 = 0x00000020,
 Psm6 = 0x00000040,
 Psm7 = 0x00000080,
 Psm8 = 0x00000100,
 Psm9 = 0x00000200,
 Psm10 = 0x00000400,
 Psm11 = 0x00000800,
 Psm12 = 0x00001000,
 Psm13 = 0x00002000,
 Psm14 = 0x00004000,
 Psm15 = 0x00008000,
}GPIO_PinMask_t;


void GPIO_Init(GPIO_TypeDef  *GPIOx, GPIO_InitTypeDef *GPIO_Init);
void GPIO_PinReset(GPIO_TypeDef *GPIOx, GPIO_PinMask_t GPIO_Pin);
void GPIO_PinSet(GPIO_TypeDef *GPIOx, GPIO_PinMask_t GPIO_Pin);
void tooglePIN(GPIO_TypeDef *GPIOx, GPIO_PinMask_t GPIO_Pin);

#endif /*__GPIO_H_ */