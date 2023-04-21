#include "stm32f7xx.h"
#include "stdbool.h"

#include "DLTuc.h"
#include "System.h"

static volatile bool UART3_DMA_TransmitEndFlag = true;

void DMA1_Stream3_IRQHandler(void)
{	
	UART3_DMA_TransmitEndFlag = true;
	DMA1->LIFCR |= (DMA_LIFCR_CTCIF3);
	DLTuc_MessageTransmitDone();
}

void UART3_DMA_Transmit(uint8_t *Buff, uint8_t size)
{
	while(UART3_DMA_TransmitEndFlag != true)
	{
		/*Wait for transmit end...*/
	}

	DLTuc_UpdateTimeStampMs(GetSysTime() ); /*Not best way but for me it's inaf solution*/

	DMA1_Stream3->NDTR = size;
	DMA1_Stream3->PAR = (volatile long unsigned int)&USART3->TDR;
	DMA1_Stream3->M0AR =  (volatile long unsigned int)Buff;
	UART3_DMA_TransmitEndFlag = false;
	DMA1_Stream3->CR |= DMA_SxCR_EN;
}

void UpdateDltTimeStamp(uint32_t TickCount)
{
	DLTuc_UpdateTimeStampMs(TickCount);
}

void UART3InitTransmitWithDMAand_ucDLTlib(void)
{
	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = STLINK_TX_Pin|STLINK_RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;


/*USART*/
	RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
	USART3->ISR = 0;
	USART3->BRR = 1736; /*115200 for 84Mhz*/
	USART3->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_TXEIE | USART_CR1_RXNEIE | USART_CR1_UE;
	USART3->CR3 |= USART_CR3_DMAT;
	/*DMA_Cfg*/ 	/*DMA1 Channel 4, stream5 RX, stream6 TX*/
	/* Only transmit acutally is used and configured */
	RCC->AHB1ENR  |= RCC_AHB1ENR_DMA1EN;
	DMA1_Stream3->CR = DMA_SxCR_TCIE | DMA_SxCR_DIR_0 | DMA_SxCR_MINC | (4ul << DMA_SxCR_CHSEL_Pos);
	NVIC_SetPriority (DMA1_Stream3_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL); 
	NVIC_EnableIRQ(DMA1_Stream3_IRQn);

	/*Register Low Level Transmit function for ucDltLibrary*/
	DLTuc_RegisterTransmitSerialDataCallback(UART3_DMA_Transmit);
	RegisterSysTickCallBack(UpdateDltTimeStamp);
	/*Now ucDLTlib is ready to work!*/
}