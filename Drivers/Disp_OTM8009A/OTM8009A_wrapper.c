/*
 * DisplayOTM8009A.c
 *
 *  Created on: May 15, 2022
 *      Author: teodor
 */
#include "OTM8009A_wrapper.h"
#include "dsihost.h"
#include "System.h"
#include "GPIO_h7.h"
#include "dma2d.h"
#include "DLTuc.h"
#include "ltdc.h"

uint8_t pColLeft[]    = {0x00, 0x00, 0x01, 0x8F}; /*   0 -> 399 */
uint8_t pColRight[]   = {0x01, 0x90, 0x03, 0x1F}; /* 400 -> 799 */
uint8_t pPage[]       = {0x00, 0x00, 0x01, 0xDF}; /*   0 -> 479 */
uint8_t pScanCol[]    = {0x02, 0x15};             /* Scan @ 533 */
static volatile int32_t pending_buffer = -1;
static volatile int32_t active_area = 0;


DISP_LCD_Ctx_t       Lcd_Ctx[LCD_INSTANCES_NBR];
OTM8009A_Object_t   OTM8009AObj;

#define CONVERTRGB5652ARGB8888(Color)((((((((Color) >> (11U)) & 0x1FU) * 527U) + 23U) >> (6U)) << (16U)) |\
                                     (((((((Color) >> (5U)) & 0x3FU) * 259U) + 33U) >> (6U)) << (8U)) |\
                                     (((((Color) & 0x1FU) * 527U) + 23U) >> (6U)) | (0xFF000000U))

#define SDRAM_DEVICE_ADDR_WRAPPER         0xD0000000U

static void ConfigAndSetNecessaryGPIO(void);

/********************************************************************************************/
/********************************************************************************************/
/********************************************************************************************/

void DSI_IO_Write(uint16_t ChannelNbr, uint16_t Reg, uint8_t *pData, uint16_t Size)
{
  if(Size <= 1U)
  {
    DSI_ShortWrite(ChannelNbr, DSI_DCS_SHORT_PKT_WRITE_P1, Reg, (uint32_t)pData[Size]);
  }
  else
  {
    DSI_LongWrite(ChannelNbr, DSI_DCS_LONG_PKT_WRITE, Size, (uint32_t)Reg, pData);
  }
}

void DSI_IO_Read(uint16_t ChannelNbr, uint16_t Reg, uint8_t *pData, uint16_t Size)
{
  DSI_Read(ChannelNbr, pData, Size, DSI_DCS_SHORT_PKT_READ, Reg, pData);
}


/********************************************************************************************/
#define LCD_FRAME_BUFFER        0xD0000000

void *ActualBufferPointer;

void OTM8009A_DisplayRefreshCommandMode(void* BuffPtr)
{
	    while(pending_buffer != -1){
	    	// if(FunInputFlag == 0)
	    	// {
	    	// 	FunInputFlag = 1;
	    	// 	pendinfBufferInrementor++;
	    	// }
	    }


  pending_buffer = 1;
	/* UnMask the TE */
	__DSI_UNMASK_TE();

  ActualBufferPointer = BuffPtr;
	LTDC_Layer1->CFBAR = (uint32_t)BuffPtr;
	LTDC->SRCR = LTDC_SRCR_IMR;
}

void DSI_TearingEffectIRQ_Callback(void)
{
  /* Mask the TE */
  __DSI_MASK_TE();
  /* Refresh the right part of the display */
  DSI_Refresh();   
}

void DSI_EndOfRefreshIRQ_Callback(void)
{
  if(pending_buffer >= 0)
  {
    if(active_area == LEFT_AREA)
    {
      /* Disable DSI Wrapper */
      __DSI_WRAPPER_DISABLE();
      /* Update LTDC configuaration */
      LTDC_LAYER(0)->CFBAR = (uint32_t)ActualBufferPointer + 400 * 4;

      LTDC->SRCR |= LTDC_SRCR_IMR;
      /* Enable DSI Wrapper */
      __DSI_WRAPPER_ENABLE();

      DSI_LongWrite(0, DSI_DCS_LONG_PKT_WRITE, 4, OTM8009A_CMD_CASET, pColRight);
      /* Refresh the right part of the display */
      DSI_Refresh();    

    }
    else if(active_area == RIGHT_AREA)
    {

      /* Disable DSI Wrapper */
      __DSI_WRAPPER_DISABLE();
      /* Update LTDC configuaration */
      LTDC_LAYER(0)->CFBAR = (uint32_t)ActualBufferPointer;
      LTDC->SRCR |= LTDC_SRCR_IMR;
      /* Enable DSI Wrapper */
      __DSI_WRAPPER_ENABLE();
      
      DSI_LongWrite(0, DSI_DCS_LONG_PKT_WRITE, 4, OTM8009A_CMD_CASET, pColLeft);
      pending_buffer = -1;     
    }
  }
  active_area = (active_area == LEFT_AREA)? RIGHT_AREA : LEFT_AREA; 
}


/********************************************************************************************/
void OTM8009A_DISP_LCD_Init(uint32_t Instance, uint32_t Orientation)
{
  /*Initialized in main file...*/
    // SDRAM_FMC_Init();
    // SDRAM_test();
    // LTDC_Init();   /* By default the reload is activated and executed immediately */
    // DSIHOST_DSI_Init();
    // DMA2D_Init();
  Lcd_Ctx[Instance].BppFactor = 4U;
  /* Store pixel format, xsize and ysize information */
  Lcd_Ctx[Instance].PixelFormat  = LCD_PIXEL_FORMAT_RGB888;
  Lcd_Ctx[Instance].XSize        = LCD_DEFAULT_WIDTH;
  Lcd_Ctx[Instance].YSize        = LCD_DEFAULT_HEIGHT;
  Lcd_Ctx[Instance].ReloadEnable = 1U;
  Lcd_Ctx[0].ActiveLayer = 0;
  ConfigAndSetNecessaryGPIO();
  DSI_RegisterIrqCallBacks(DSI_TearingEffectIRQ_Callback,DSI_EndOfRefreshIRQ_Callback);
  DSI_Start();
  uint32_t id;
  /*Init the Screen*/
  OTM8009A_IO_t              IOCtx;
  static OTM8009A_Object_t   OTM8009AObj;
  /* Configure the display driver */
  IOCtx.Address     = 0;
  IOCtx.GetTick     = (OTM8009A_GetTick_Func)GetSysTime;
  IOCtx.WriteReg    = (OTM8009A_WriteReg_Func)DSI_IO_Write;
  IOCtx.ReadReg     = (OTM8009A_ReadReg_Func)DSI_IO_Read;
  OTM8009A_RegisterBusIO(&OTM8009AObj, &IOCtx);
    if(OTM8009A_ReadID(&OTM8009AObj, &id) != OTM8009A_OK){
      LOG("DISP_COMPONENT_COMUNICATION_FAILURE");}
    else{
      LOG("DISP_COMPONENT_COMUNICATION_SUCCESFULL");}

  if(OTM8009A_Init(&OTM8009AObj, OTM8009A_COLMOD_RGB888, Orientation) != OTM8009A_OK){
    LOG("DISP_COMPONENT_COMUNICATION_FAILURE");}

  DSI_LPCmdTypeDef LLPCmd;
  LLPCmd.LPGenShortWriteNoP    = DSI_LP_GSW0P_DISABLE;
  LLPCmd.LPGenShortWriteOneP   = DSI_LP_GSW1P_DISABLE;
  LLPCmd.LPGenShortWriteTwoP   = DSI_LP_GSW2P_DISABLE;
  LLPCmd.LPGenShortReadNoP     = DSI_LP_GSR0P_DISABLE;
  LLPCmd.LPGenShortReadOneP    = DSI_LP_GSR1P_DISABLE;
  LLPCmd.LPGenShortReadTwoP    = DSI_LP_GSR2P_DISABLE;
  LLPCmd.LPGenLongWrite        = DSI_LP_GLW_DISABLE;
  LLPCmd.LPDcsShortWriteNoP    = DSI_LP_DSW0P_DISABLE;
  LLPCmd.LPDcsShortWriteOneP   = DSI_LP_DSW1P_DISABLE;
  LLPCmd.LPDcsShortReadNoP     = DSI_LP_DSR0P_DISABLE;
  LLPCmd.LPDcsLongWrite        = DSI_LP_DLW_DISABLE;
  LLPCmd.LPMaxReadPacket       = 0u;
  LLPCmd.AcknowledgeRequest    = 0u;
  DSI_ConfigAdaptedCommandMode(&LLPCmd);
  DSI_ConfigFlowControl(DSI_FLOW_CONTROL_BTA);
  DSI_Start();

  __DSI_WRAPPER_DISABLE();
  LTDC_SetPitch(LCD_PIXEL_FORMAT_ARGB8888, LCD_DEFAULT_WIDTH, 0);
  __DSI_WRAPPER_ENABLE();
  DSI_LongWrite(0, DSI_DCS_LONG_PKT_WRITE, 4, OTM8009A_CMD_CASET, pColLeft);
  DSI_LongWrite(0, DSI_DCS_LONG_PKT_WRITE, 4, OTM8009A_CMD_PASET, pPage);
  pending_buffer = 0;
  active_area = LEFT_AREA;
  DSI_LongWrite(0, DSI_DCS_LONG_PKT_WRITE, 2, OTM8009A_CMD_WRTESCN, pScanCol);
  __DSI_UNMASK_TE();
}



/********************************************************************************************/
void (*DMAtrEndCbPtr)(void);

void DISP_LCD_LL_FlushBufferDMA2D(
                  uint32_t Instance,
									uint32_t Xpos,
									uint32_t Ypos,
									uint32_t Width,
									uint32_t Height,
									uint32_t *Colormap,
                  void(*DMAtrEndCb)(void)
									)
{
  uint32_t  Xaddress;

  DMAtrEndCbPtr = DMAtrEndCb;

#define LCD_FB_START_ADDRESS 0xD0000000

  Xaddress = LCD_FB_START_ADDRESS + (Lcd_Ctx[Instance].BppFactor*(Lcd_Ctx[Instance].XSize*Ypos + Xpos));

  /* Flush the buffer using dma */
  // DMA2D_FlushBuffer((uint32_t *)Xaddress, Width, Height, (Lcd_Ctx[Instance].XSize - Width), (uint32_t)Colormap);
  DMA2D_InitRefresh(
                        DMA2D_M2M, /*Init mode*/
                        DMA2D_LOM_PIXELS, /*Line offset mode*/
                        DMA2D_REGULAR_ALPHA,
                        DMA2D_RB_REGULAR,
                        (Lcd_Ctx[Instance].XSize - Width), /*output offset*/
                        DMA2D_BYTES_REGULAR, /**/
                        DMA2D_OUTPUT_ARGB8888 /*Color mode*/
                        );

  DMA2D_SetConfig((uint32_t)Colormap, Xaddress, Width,
                            Height, DMA2D_M2M, DMA2D_OUTPUT_ARGB8888);
  DMA2D_StartIT();
}




void DMA2D_IRQHandler(void)
{
  // uint32_t isrflags = READ_REG(DMA2D->ISR);
  // uint32_t crflags = READ_REG(DMA2D->CR);
  // DMA2D->CR &= ~(DMA2D_CR_TCIE | DMA2D_CR_TEIE | DMA2D_CR_CEIE);
  DMA2D->IFCR = (DMA2D_ISR_TCIF | DMA2D_ISR_TEIF | DMA2D_ISR_CEIF);

  if(DMAtrEndCbPtr != (void *)0)
  {
  DMAtrEndCbPtr();
  }

}


/********************************************************************************************/
static void ConfigAndSetNecessaryGPIO(void)
{
	/*Cube is stupid don't generate me the pins initializations*/

	/* Toggle Hardware Reset of the LCD using its XRES signal (active low) */

	GPIO_InitTypeDef  gpio_init_structure;

	LCD_RESET_GPIO_CLK_ENABLE(); /*LCD RESET CLK ENABLE*/

	/* Configure the GPIO Reset pin */
	gpio_init_structure.Pin   = LCD_RESET_PIN;
	gpio_init_structure.Mode  = GPIO_MODE_OUTPUT_PP;
	gpio_init_structure.Pull  = GPIO_PULLUP;
	gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_Init(LCD_RESET_GPIO_PORT , &gpio_init_structure);

	/* Activate XRES active low */
  GPIO_PinReset(LCD_RESET_GPIO_PORT,LCD_RESET_PIN);
	DelayMs(20);/* wait 20 ms */
  GPIO_PinSet(LCD_RESET_GPIO_PORT,LCD_RESET_PIN);/* Deactivate XRES */
	DelayMs(10);/* Wait for 10ms after releasing XRES before sending commands */
	/* Initialize LCD special pins GPIOs */


	/* Back-light control pin */
	/* LCD_BL_CTRL GPIO configuration */
	LCD_BL_CTRL_GPIO_CLK_ENABLE();

	gpio_init_structure.Pin       = LCD_BL_CTRL_PIN;
	gpio_init_structure.Mode      = GPIO_MODE_OUTPUT_PP;
	gpio_init_structure.Speed     = GPIO_SPEED_FREQ_HIGH;
	gpio_init_structure.Pull      = GPIO_NOPULL;

	GPIO_Init(LCD_BL_CTRL_GPIO_PORT, &gpio_init_structure);
	/* Assert back-light LCD_BL_CTRL pin */
  GPIO_PinSet(LCD_BL_CTRL_GPIO_PORT,LCD_BL_CTRL_PIN);

	/*Transmit Enable pin  LCD tearing effect pin in this app unused now */
	/* LCD_TE_CTRL GPIO configuration */
	LCD_TE_GPIO_CLK_ENABLE();

    gpio_init_structure.Pin = LCD_TE_PIN;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Pull = GPIO_NOPULL;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;
    // gpio_init_structure.Alternate = GPIO_AF13_DSI;
	GPIO_Init(LCD_TE_GPIO_PORT, &gpio_init_structure);
	/* Assert back-light LCD_BL_CTRL pin */
  GPIO_PinSet(LCD_TE_GPIO_PORT,LCD_TE_PIN);
}