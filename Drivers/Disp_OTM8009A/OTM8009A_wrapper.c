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


#define CONVERTRGB5652ARGB8888(Color)((((((((Color) >> (11U)) & 0x1FU) * 527U) + 23U) >> (6U)) << (16U)) |\
                                     (((((((Color) >> (5U)) & 0x3FU) * 259U) + 33U) >> (6U)) << (8U)) |\
                                     (((((Color) & 0x1FU) * 527U) + 23U) >> (6U)) | (0xFF000000U))


#define ZONES               10       /*Divide the screen into zones to handle tearing effect*/
#define HACT_HELPER                (OTM8009A_800X480_WIDTH / ZONES)

/* When changing these parameters, you should also change the settings of the LTDC & DSIHOST in the .ioc project
   (set the "horizontal pixles" value 800, 400 or 200 in LTDC section and "Maximum command size" in DSIHOST section) */
uint8_t pCols[ZONES][4] =
{
{ 0x00, 0x00 , 0x00, 0x4f},
{ 0x00, 0x50 , 0x00, 0x9f},
{ 0x00, 0xa0 , 0x00, 0xef},
{ 0x00, 0xf0 , 0x01, 0x3f},
{ 0x01, 0x40 , 0x01, 0x8f},
{ 0x01, 0x90 , 0x01, 0xdf},
{ 0x01, 0xe0 , 0x02, 0x2f},
{ 0x02, 0x30 , 0x02, 0x7f},
{ 0x02, 0x80 , 0x02, 0xcf},
{ 0x02, 0xd0 , 0x03, 0x1f}
/*
int main(void)
{
    uint16_t LowerPart=0;
    uint16_t HigherPart=0;
for(int i=0; i<4; i++){
    HigherPart +=199;
    printf("{ 0x%02x, 0x%02x , 0x%02x, 0x%02x}, \n\r",
        LowerPart >> 8,LowerPart & 0xFF,HigherPart >> 8,HigherPart & 0xFF);
    HigherPart +=1;
    LowerPart += 200;}
}
*/
};

uint8_t pPage[]       = {0x00, 0x00, 0x01, 0xDF}; /*   0 -> 479 */
uint8_t pScanCol[]    = {0x02, 0x15};             /* Scan @ 533 */

uint8_t RefreshCompleteFlag= 1; 
static volatile uint32_t LCD_ActiveRegion;

DISP_LCD_Ctx_t       Lcd_Ctx[LCD_INSTANCES_NBR];
OTM8009A_Object_t   OTM8009AObj;

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
void *ActualBufferPointer;

void OTM8009A_DisplayRefreshCommandMode(void* BuffPtr)
{
  while(RefreshCompleteFlag != 1){}
  RefreshCompleteFlag = 0;
	/* UnMask the TE */
	__DSI_UNMASK_TE();
  ActualBufferPointer = BuffPtr;
}

void DSI_TearingEffectIRQ_Callback(void)
{
  /* Mask the TE */
  DSI_Refresh();
  __DSI_MASK_TE();
}

void LCD_SetUpdateRegion(int idx)
{
  DSI_LongWrite(0, DSI_DCS_LONG_PKT_WRITE, 4, OTM8009A_CMD_CASET, pCols[idx]);
}

void DSI_EndOfRefreshIRQ_Callback(void)
{
      if(LCD_ActiveRegion < ZONES)
      {
          /* Disable DSI Wrapper */
          __DSI_WRAPPER_DISABLE();
          /* Update LTDC configuaration */
          LTDC_LAYER(0)->CFBAR  = ActualBufferPointer + LCD_ActiveRegion  * HACT_HELPER * sizeof(uint32_t);
          LTDC->SRCR |= LTDC_SRCR_IMR;
          /* Enable DSI Wrapper */
          __DSI_WRAPPER_ENABLE();
          LCD_SetUpdateRegion(LCD_ActiveRegion);
          LCD_ActiveRegion++;
          /* Refresh choosen part of the display */
          DSI_Refresh();
      }
      else
      {
          /* Disable DSI Wrapper */
          __DSI_WRAPPER_DISABLE();
          /* Update LTDC configuaration */
          LTDC_LAYER(0)->CFBAR = (uint32_t)ActualBufferPointer;
          LTDC->SRCR |= LTDC_SRCR_IMR;
          /* Enable DSI Wrapper */
          __DSI_WRAPPER_ENABLE();
          LCD_ActiveRegion = 0;
          LCD_SetUpdateRegion(LCD_ActiveRegion);
          RefreshCompleteFlag = 1;
      }
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
  DelayMs(1);
  OTM8009A_Init(&OTM8009AObj, OTM8009A_COLMOD_RGB888, Orientation);

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
  DSI_LongWrite(0, DSI_DCS_LONG_PKT_WRITE, 4, OTM8009A_CMD_CASET, pCols[0]);
  DSI_LongWrite(0, DSI_DCS_LONG_PKT_WRITE, 4, OTM8009A_CMD_PASET, pPage);
  DSI_LongWrite(0, DSI_DCS_LONG_PKT_WRITE, 2, OTM8009A_CMD_WRTESCN, pScanCol);
  __DSI_UNMASK_TE();
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


	/* LCD_TE_CTRL GPIO configuration */
	LCD_TE_GPIO_CLK_ENABLE();

    gpio_init_structure.Pin = LCD_TE_PIN;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Pull = GPIO_NOPULL;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_Init(LCD_TE_GPIO_PORT, &gpio_init_structure);
    GPIO_PinSet(LCD_TE_GPIO_PORT,LCD_TE_PIN);
}