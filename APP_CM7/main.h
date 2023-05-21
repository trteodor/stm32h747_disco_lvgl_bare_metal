

#ifndef __MAIN_H_
#define __MAIN_H_

#define SW_VERSION "001"

/******************************************************************/
#define LED1_Pin GPIO_PIN_12
#define LED1_GPIO_Port GPIOI
#define LED2_Pin GPIO_PIN_13
#define LED2_GPIO_Port GPIOI
#define LED3_Pin GPIO_PIN_14
#define LED3_GPIO_Port GPIOI
/******************************************************************/

#include "stdint.h"
#include "stm32h747xx.h"
#include "GPIO_h7.h"
#include "stdio.h"
#include "UART1_dlt.h"
#include "DLTuc.h"
#include "System.h"
#include "sd_ram.h"
#include "dsihost.h"
#include "dma2d.h"
#include "ltdc.h"
#include "OTM8009A_wrapper.h"
#include "TouchC_ft6x06.h"
#include "lvglAppMain.h"
#include "qspi.h"










#endif /*__MAIN_H_*/