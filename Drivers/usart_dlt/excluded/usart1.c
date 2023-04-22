#include "stm32h7xx.h"
#include "GPIO_h7.h"
#include "stdbool.h"

#define STLINK_TX_Pin GPIO_PIN_10
#define STLINK_TX_GPIO_Port GPIOA
#define STLINK_RX_Pin GPIO_PIN_9
#define STLINK_RX_GPIO_Port GPIOA

#define GPIO_AF7_USART1        ((uint8_t)0x07)  /* USART1 Alternate Function mapping */

void uart_write(uint8_t value)
{
	while(!((USART1->ISR & USART_ISR_TXE_TXFNF) == USART_ISR_TXE_TXFNF)){}
	USART1->TDR = value;
}

int __io_putchar(int ch)
{
	uart_write(ch);
    return ch;
}

//function must be called...
/*
* ""
* checked your code just now and found your printf message does not 
* contain newline "\n" so your printf doesn't hit lower
* implementation of __io_putchar since stdout is buffered.""
* https://community.st.com/s/question/0D50X0000B8kTytSQE/printf-not-working-write-never-gets-called
*/

void uart1_tx_init(void)
{
	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = STLINK_TX_Pin|STLINK_RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;

    GPIO_Init(GPIOA, &GPIO_InitStruct);


	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	USART1->CR1 |= USART_CR1_TE;
	USART1->BRR = 555; //( 200Mhz+(115200/2) )/ 115200 fast way - default freq of the board is 64Mhz
	USART1->CR1 |= USART_CR1_UE;

	//najprosciej jak sie dalo nie ma co kombinowac - dziala :), jak bedzie potrzeba message'a sie wysle
}


