#include "stm32h7xx.h"
#include "GPIO_h7.h"
#include "stdbool.h"

#define STLINK_TX_Pin GPIO_PIN_10
#define STLINK_TX_GPIO_Port GPIOA
#define STLINK_RX_Pin GPIO_PIN_9
#define STLINK_RX_GPIO_Port GPIOA

static void uart_write(uint8_t value)
{
	while(!((USART3->ISR & USART_ISR_TXE) == USART_ISR_TXE)){}
	USART3->TDR = value;
}

int __io_putchar(int ch)
{
	uart_write(ch);
    return ch;
}

void uart3_tx_init(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
	GPIOD->MODER &= ~(1U<<16);
	GPIOD->MODER |= (1U<<17);
	GPIOD->AFR[1]  |=(1U<<0);
	GPIOD->AFR[1]  |=(1U<<1);
	GPIOD->AFR[1]  |=(1U<<2);
	GPIOD->AFR[1]  &=~(1U<<3);

	RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
	USART3->CR1 |= USART_CR1_TE;
	USART3->BRR = 1736; //( 200Mhz+(115200/2) )/ 115200 fast way
	USART3->CR1 |= USART_CR1_UE;

	//najprosciej jak sie dalo nie ma co kombinowac - dziala :), jak bedzie potrzeba message'a sie wysle
}


