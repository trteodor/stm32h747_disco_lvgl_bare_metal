#include "stm32h7xx.h"
#include "stdbool.h"

#include "GPIO_h7.h"
#include "DLTuc.h"
#include "System.h"

#define STLINK_TX_Pin GPIO_PIN_10
#define STLINK_TX_GPIO_Port GPIOA
#define STLINK_RX_Pin GPIO_PIN_9
#define STLINK_RX_GPIO_Port GPIOA

#define GPIO_AF7_USART1        ((uint8_t)0x07)  /* USART1 Alternate Function mapping */

static volatile bool UART1_DMA_TransmitEndFlag = true;

void DMA1_Stream3_IRQHandler(void)
{
	UART1_DMA_TransmitEndFlag = true;
	DMA1->LIFCR |= (DMA_LIFCR_CTCIF3);
	DLTuc_MessageTransmitDone();
}

void UART1_DMA_Transmit(uint8_t *Buff, uint8_t size)
{
	while(UART1_DMA_TransmitEndFlag != true)
	{
		/*Wait for transmit end...*/
	}

	DMA1_Stream3->NDTR = size;
	DMA1_Stream3->PAR = (volatile long unsigned int)&USART1->TDR;
	DMA1_Stream3->M0AR =  (volatile long unsigned int)Buff;
	UART1_DMA_TransmitEndFlag = false;
	DMA1_Stream3->CR |= DMA_SxCR_EN;
	USART1->CR3 |= USART_CR3_DMAT;
}

void UART1InitTransmitWithDMAand_ucDLTlib(void)
{
/*GPIO*/
	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = STLINK_TX_Pin|STLINK_RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

/*USART*/
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	USART1->ISR = 0;
	USART1->BRR = 868;
	// USART1->BRR = 555;
	USART1->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_TXEIE | USART_CR1_RXNEIE | USART_CR1_UE;

/*DMA*/
	RCC->AHB1ENR  |= RCC_AHB1ENR_DMA1EN;
	DMA1_Stream3->CR = DMA_SxCR_TCIE | DMA_SxCR_DIR_0 | DMA_SxCR_MINC | DMA_SxCR_TRBUFF;
	DMAMUX1_Channel3->CCR = 0x2a; /*Szybciej byloby przeczytac dokumentacje ale db dziala*/
	NVIC_SetPriority (DMA1_Stream3_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL); 
	NVIC_EnableIRQ(DMA1_Stream3_IRQn);

/*DLTuc*/
	/*Register Low Level Transmit function for DLTuc Library*/
	DLTuc_RegisterTransmitSerialDataCallback(UART1_DMA_Transmit);
	DLTuc_RegisterGetTimeStampMsCallback(GetSysTime);
	/*Now ucDLTlib is ready to work!*/
}