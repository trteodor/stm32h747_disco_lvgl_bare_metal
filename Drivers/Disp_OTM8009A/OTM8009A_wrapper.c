/*
 * DisplayOTM8009A.c
 *
 *  Created on: May 15, 2022
 *      Author: teodor
 */
#include <Display_OTM8009A.h>
#include "main.h"

/**
 * VARIABLE SECTION START
 **************************************************************************************************************************
 */

extern LCD_Drv_t                *Lcd_Drv;

const LCD_UTILS_Drv_t LCD_Driver =
{
  DISP_LCD_DrawBitmap,
  DISP_LCD_FillRGBRect,
  DISP_LCD_DrawHLine,
  DISP_LCD_DrawVLine,
  DISP_LCD_FillRect,
  DISP_LCD_ReadPixel,
  DISP_LCD_WritePixel,
  DISP_LCD_GetXSize,
  DISP_LCD_GetYSize,
  DISP_LCD_SetActiveLayer,
  DISP_LCD_GetPixelFormat
};

typedef struct
{
  uint32_t      HACT;
  uint32_t      VACT;
  uint32_t      HSYNC;
  uint32_t      HBP;
  uint32_t      HFP;
  uint32_t      VSYNC;
  uint32_t      VBP;
  uint32_t      VFP;

  /* Configure the D-PHY Timings */
  uint32_t      ClockLaneHS2LPTime;
  uint32_t      ClockLaneLP2HSTime;
  uint32_t      DataLaneHS2LPTime;
  uint32_t      DataLaneLP2HSTime;
  uint32_t      DataLaneMaxReadTime;
  uint32_t      StopWaitTime;
} LCD_HDMI_Timing_t;


void                *Lcd_CompObj = NULL;
DSI_HandleTypeDef   *hlcd_dsi = NULL;
DMA2D_HandleTypeDef *hlcd_dma2d;
LTDC_HandleTypeDef  *hlcd_ltdc = NULL;
DISP_LCD_Ctx_t       Lcd_Ctx[LCD_INSTANCES_NBR];
OTM8009A_Object_t   OTM8009AObj;

static void LL_FillBuffer(uint32_t Instance, uint32_t *pDst, uint32_t xSize, uint32_t ySize, uint32_t OffLine, uint32_t Color);
static void LL_ConvertLineToRGB(uint32_t Instance, uint32_t *pSrc, uint32_t *pDst, uint32_t xSize, uint32_t ColorMode);
int32_t DISP_LCD_LL_FlushBufferDMA2D(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t Width, uint32_t Height, uint32_t *Colormap, void(*DMAtrEndCb)(void));
static void LL_DMAFlushBuffer(uint32_t Instance, uint32_t *pDst, uint32_t xSize, uint32_t ySize, uint32_t OffLine, uint32_t Color,void(*DMAtrEndCb)(void));

#define CONVERTRGB5652ARGB8888(Color)((((((((Color) >> (11U)) & 0x1FU) * 527U) + 23U) >> (6U)) << (16U)) |\
                                     (((((((Color) >> (5U)) & 0x3FU) * 259U) + 33U) >> (6U)) << (8U)) |\
                                     (((((Color) & 0x1FU) * 527U) + 23U) >> (6U)) | (0xFF000000U))




/**
 *INIT SECTION START
 **************************************************************************************************************************
 */
int32_t DISP_GetTick(void)
{
  return (int32_t)HAL_GetTick();
}

int32_t DISP_LCD_InitEx(uint32_t Instance, uint32_t Orientation, uint32_t PixelFormat, uint32_t Width, uint32_t Height);

int32_t DSI_IO_Write(uint16_t ChannelNbr, uint16_t Reg, uint8_t *pData, uint16_t Size)
{
  int32_t ret = DRV_ERR_NONE;

  if(Size <= 1U)
  {
    if(HAL_DSI_ShortWrite(hlcd_dsi, ChannelNbr, DSI_DCS_SHORT_PKT_WRITE_P1, Reg, (uint32_t)pData[Size]) != HAL_OK)
    {
      ret = DRV_ERROR_BUS_FAILURE;
    }
  }
  else
  {
    if(HAL_DSI_LongWrite(hlcd_dsi, ChannelNbr, DSI_DCS_LONG_PKT_WRITE, Size, (uint32_t)Reg, pData) != HAL_OK)
    {
      ret = DRV_ERROR_BUS_FAILURE;
    }
  }

  return ret;
}

int32_t DSI_IO_Read(uint16_t ChannelNbr, uint16_t Reg, uint8_t *pData, uint16_t Size)
{
  int32_t ret = DRV_ERR_NONE;

  if(HAL_DSI_Read(hlcd_dsi, ChannelNbr, pData, Size, DSI_DCS_SHORT_PKT_READ, Reg, pData) != HAL_OK)
  {
    ret = DRV_ERROR_BUS_FAILURE;
  }

  return ret;
}

LCD_Drv_t                *Lcd_Drv = NULL;

int32_t OTM8009A_Probe(uint32_t ColorCoding, uint32_t Orientation)
{
  int32_t ret;
  uint32_t id;
  OTM8009A_IO_t              IOCtx;
  static OTM8009A_Object_t   OTM8009AObj;

  /* Configure the audio driver */
  IOCtx.Address     = 0;
  IOCtx.GetTick     = DISP_GetTick;
  IOCtx.WriteReg    = DSI_IO_Write;
  IOCtx.ReadReg     = DSI_IO_Read;

  if(OTM8009A_RegisterBusIO(&OTM8009AObj, &IOCtx) != OTM8009A_OK)
  {
    ret = DRV_ERROR_BUS_FAILURE;
  }
  else
  {
    Lcd_CompObj = &OTM8009AObj;

    if(OTM8009A_ReadID(Lcd_CompObj, &id) != OTM8009A_OK)
    {
      ret = DRV_ERROR_COMPONENT_FAILURE;
    }

    else
    {
      Lcd_Drv = (LCD_Drv_t *)(void *) &OTM8009A_LCD_Driver;
      if(Lcd_Drv->Init(Lcd_CompObj, ColorCoding, Orientation) != OTM8009A_OK)
      {
        ret = DRV_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = DRV_ERR_NONE;
      }
    }
  }
  return ret;
}



void ConfigAndSetNecessaryGPIO(void)
{
	/*Cube is stupid don't generate me the pins initializations*/

	/* Toggle Hardware Reset of the LCD using its XRES signal (active low) */

	GPIO_InitTypeDef  gpio_init_structure;

	__HAL_RCC_GPIOG_CLK_ENABLE(); /*LCD RESET CLK ENABLE*/

	/* Configure the GPIO Reset pin */
	gpio_init_structure.Pin   = LCD_RESET_PIN;
	gpio_init_structure.Mode  = GPIO_MODE_OUTPUT_PP;
	gpio_init_structure.Pull  = GPIO_PULLUP;
	gpio_init_structure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(LCD_RESET_GPIO_PORT , &gpio_init_structure);

	/* Activate XRES active low */
	HAL_GPIO_WritePin(LCD_RESET_GPIO_PORT , LCD_RESET_PIN, GPIO_PIN_RESET);
	HAL_Delay(20);/* wait 20 ms */
	HAL_GPIO_WritePin(LCD_RESET_GPIO_PORT , LCD_RESET_PIN, GPIO_PIN_SET);/* Deactivate XRES */
	HAL_Delay(10);/* Wait for 10ms after releasing XRES before sending commands */
	/* Initialize LCD special pins GPIOs */


	/* Back-light control pin */
	/* LCD_BL_CTRL GPIO configuration */
	LCD_BL_CTRL_GPIO_CLK_ENABLE();

	gpio_init_structure.Pin       = LCD_BL_CTRL_PIN;
	gpio_init_structure.Mode      = GPIO_MODE_OUTPUT_PP;
	gpio_init_structure.Speed     = GPIO_SPEED_FREQ_HIGH;
	gpio_init_structure.Pull      = GPIO_NOPULL;

	HAL_GPIO_Init(LCD_BL_CTRL_GPIO_PORT, &gpio_init_structure);
	/* Assert back-light LCD_BL_CTRL pin */
	HAL_GPIO_WritePin(LCD_BL_CTRL_GPIO_PORT, LCD_BL_CTRL_PIN, GPIO_PIN_SET);


	/*Transmit Enable pin  LCD tearing effect pin in this app unused now */
	/* LCD_TE_CTRL GPIO configuration */
	LCD_TE_GPIO_CLK_ENABLE();

	gpio_init_structure.Pin       = LCD_TE_PIN;
	gpio_init_structure.Mode      = GPIO_MODE_INPUT;
	gpio_init_structure.Speed     = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(LCD_TE_GPIO_PORT, &gpio_init_structure);
	/* Assert back-light LCD_BL_CTRL pin */
	HAL_GPIO_WritePin(LCD_TE_GPIO_PORT, LCD_TE_PIN, GPIO_PIN_SET);

}

int32_t OTM8009A_DISP_LCD_Init(uint32_t Instance, uint32_t Orientation,DSI_HandleTypeDef *hdsi, LTDC_HandleTypeDef *hltdc)
{
	/**/
	hlcd_dma2d = &hdma2d; /*TODO*/
	hlcd_dsi = hdsi;
	hlcd_ltdc = hltdc;

  return DISP_LCD_InitEx(Instance, Orientation, LCD_PIXEL_FORMAT_RGB888, LCD_DEFAULT_WIDTH, LCD_DEFAULT_HEIGHT);
}

int32_t DISP_LCD_InitEx(uint32_t Instance, uint32_t Orientation, uint32_t PixelFormat, uint32_t Width, uint32_t Height)
{
	int32_t ret = DRV_ERR_NONE;

	ConfigAndSetNecessaryGPIO();


	(void)HAL_DSI_Start(hlcd_dsi);
	(void)HAL_DSI_ConfigFlowControl(hlcd_dsi, DSI_FLOW_CONTROL_BTA);

	/*Init the Screen*/
	OTM8009A_Probe(OTM8009A_FORMAT_RGB888, Orientation);

	/* By default the reload is activated and executed immediately */
	Lcd_Ctx[Instance].BppFactor = 4U;
	/* Store pixel format, xsize and ysize information */
	Lcd_Ctx[Instance].PixelFormat = PixelFormat;
	Lcd_Ctx[Instance].XSize  = Width;
	Lcd_Ctx[Instance].YSize  = Height;
	Lcd_Ctx[Instance].ReloadEnable = 1U;


  return ret;
}
/**
 *INIT SECTION END
 **************************************************************************************************************************
 */




int32_t DISP_LCD_SetActiveLayer(uint32_t Instance, uint32_t LayerIndex)
{
  int32_t ret = DRV_ERR_NONE;

  if(Instance >= LCD_INSTANCES_NBR)
  {
    ret = DRV_ERROR_WRONG_PARAM;
  }
  else
  {
    Lcd_Ctx[Instance].ActiveLayer = LayerIndex;
  }

  return ret;
}

int32_t DISP_LCD_GetPixelFormat(uint32_t Instance, uint32_t *PixelFormat)
{
  int32_t ret = DRV_ERR_NONE;

  if(Instance >= LCD_INSTANCES_NBR)
  {
    ret = DRV_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Only RGB565 format is supported */
    *PixelFormat = Lcd_Ctx[Instance].PixelFormat;
  }

  return ret;
}

int32_t DISP_LCD_Relaod(uint32_t Instance, uint32_t ReloadType)
{
  int32_t ret = DRV_ERR_NONE;

  if(Instance >= LCD_INSTANCES_NBR)
  {
    ret = DRV_ERROR_WRONG_PARAM;
  }
  else if(ReloadType == DISP_LCD_RELOAD_NONE)
  {
    Lcd_Ctx[Instance].ReloadEnable = 0U;
  }
  else if(HAL_LTDC_Reload (hlcd_ltdc, ReloadType) != HAL_OK)
  {
    ret = DRV_ERROR_PERIPH_FAILURE;
  }
  else
  {
    Lcd_Ctx[Instance].ReloadEnable = 1U;
  }

  return ret;
}

int32_t DISP_LCD_SetLayerVisible(uint32_t Instance, uint32_t LayerIndex, FunctionalState State)
{
  int32_t ret = DRV_ERR_NONE;

  if(Instance >= LCD_INSTANCES_NBR)
  {
    ret = DRV_ERROR_WRONG_PARAM;
  }
  else
  {
    if(State == ENABLE)
    {
      __HAL_LTDC_LAYER_ENABLE(hlcd_ltdc, LayerIndex);
    }
    else
    {
      __HAL_LTDC_LAYER_DISABLE(hlcd_ltdc, LayerIndex);
    }

    if(Lcd_Ctx[Instance].ReloadEnable == 1U)
    {
      __HAL_LTDC_RELOAD_IMMEDIATE_CONFIG(hlcd_ltdc);
    }
  }

  return ret;
}

int32_t DISP_LCD_SetTransparency(uint32_t Instance, uint32_t LayerIndex, uint8_t Transparency)
{
  int32_t ret = DRV_ERR_NONE;

  if(Instance >= LCD_INSTANCES_NBR)
  {
    ret = DRV_ERROR_WRONG_PARAM;
  }
  else
  {
    if(Lcd_Ctx[Instance].ReloadEnable == 1U)
    {
      (void)HAL_LTDC_SetAlpha(hlcd_ltdc, Transparency, LayerIndex);
    }
    else
    {
      (void)HAL_LTDC_SetAlpha_NoReload(hlcd_ltdc, Transparency, LayerIndex);
    }
  }

  return ret;
}

int32_t DISP_LCD_SetLayerAddress(uint32_t Instance, uint32_t LayerIndex, uint32_t Address)
{
  int32_t ret = DRV_ERR_NONE;

  if(Instance >= LCD_INSTANCES_NBR)
  {
    ret = DRV_ERROR_WRONG_PARAM;
  }
  else
  {
    if(Lcd_Ctx[Instance].ReloadEnable == 1U)
    {
      (void)HAL_LTDC_SetAddress(hlcd_ltdc, Address, LayerIndex);
    }
    else
    {
      (void)HAL_LTDC_SetAddress_NoReload(hlcd_ltdc, Address, LayerIndex);
    }
  }

  return ret;
}

int32_t DISP_LCD_SetLayerWindow(uint32_t Instance, uint16_t LayerIndex, uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
  int32_t ret = DRV_ERR_NONE;

  if(Instance >= LCD_INSTANCES_NBR)
  {
    ret = DRV_ERROR_WRONG_PARAM;
  }
  else
  {
    if(Lcd_Ctx[Instance].ReloadEnable == 1U)
    {
      /* Reconfigure the layer size  and position */
      (void)HAL_LTDC_SetWindowSize(hlcd_ltdc, Width, Height, LayerIndex);
      (void)HAL_LTDC_SetWindowPosition(hlcd_ltdc, Xpos, Ypos, LayerIndex);
    }
    else
    {
      /* Reconfigure the layer size and position */
      (void)HAL_LTDC_SetWindowSize_NoReload(hlcd_ltdc, Width, Height, LayerIndex);
      (void)HAL_LTDC_SetWindowPosition_NoReload(hlcd_ltdc, Xpos, Ypos, LayerIndex);
    }

    Lcd_Ctx[Instance].XSize = Width;
    Lcd_Ctx[Instance].YSize = Height;
  }

  return ret;
}

int32_t DISP_LCD_SetColorKeying(uint32_t Instance, uint32_t LayerIndex, uint32_t Color)
{
  int32_t ret = DRV_ERR_NONE;

  if(Instance >= LCD_INSTANCES_NBR)
  {
    ret = DRV_ERROR_WRONG_PARAM;
  }
  else
  {
    if(Lcd_Ctx[Instance].ReloadEnable == 1U)
    {
      /* Configure and Enable the color Keying for LCD Layer */
      (void)HAL_LTDC_ConfigColorKeying(hlcd_ltdc, Color, LayerIndex);
      (void)HAL_LTDC_EnableColorKeying(hlcd_ltdc, LayerIndex);
    }
    else
    {
      /* Configure and Enable the color Keying for LCD Layer */
      (void)HAL_LTDC_ConfigColorKeying_NoReload(hlcd_ltdc, Color, LayerIndex);
      (void)HAL_LTDC_EnableColorKeying_NoReload(hlcd_ltdc, LayerIndex);
    }
  }
  return ret;
}

int32_t DISP_LCD_ResetColorKeying(uint32_t Instance, uint32_t LayerIndex)
{
  int32_t ret = DRV_ERR_NONE;

  if(Instance >= LCD_INSTANCES_NBR)
  {
    ret = DRV_ERROR_WRONG_PARAM;
  }
  else
  {
    if(Lcd_Ctx[Instance].ReloadEnable == 1U)
    {
      /* Disable the color Keying for LCD Layer */
      (void)HAL_LTDC_DisableColorKeying(hlcd_ltdc, LayerIndex);
    }
    else
    {
      /* Disable the color Keying for LCD Layer */
      (void)HAL_LTDC_DisableColorKeying_NoReload(hlcd_ltdc, LayerIndex);
    }
  }

  return ret;
}

int32_t DISP_LCD_GetXSize(uint32_t Instance, uint32_t *XSize)
{
  int32_t ret = DRV_ERR_NONE;

  if(Instance >= LCD_INSTANCES_NBR)
  {
    ret = DRV_ERROR_WRONG_PARAM;
  }
  else if(Lcd_Drv->GetXSize != NULL)
  {
    *XSize = Lcd_Ctx[Instance].XSize;
  }

  return ret;
}

int32_t DISP_LCD_GetYSize(uint32_t Instance, uint32_t *YSize)
{
  int32_t ret = DRV_ERR_NONE;

  if(Instance >= LCD_INSTANCES_NBR)
  {
    ret = DRV_ERROR_WRONG_PARAM;
  }
  else if(Lcd_Drv->GetYSize != NULL)
  {
    *YSize = Lcd_Ctx[Instance].YSize;
  }

  return ret;
}

int32_t DISP_LCD_DisplayOn(uint32_t Instance)
{
  int32_t ret;

  if(Instance >= LCD_INSTANCES_NBR)
  {
    ret = DRV_ERROR_WRONG_PARAM;
  }
  else
  {
    if(Lcd_Drv->DisplayOn(Lcd_CompObj) != DRV_ERR_NONE)
    {
      ret = DRV_ERROR_PERIPH_FAILURE;
    }
    else
    {
      ret = DRV_ERR_NONE;
    }
  }

  return ret;
}

int32_t DISP_LCD_DisplayOff(uint32_t Instance)
{
  int32_t ret;

  if(Instance >= LCD_INSTANCES_NBR)
  {
    ret = DRV_ERROR_WRONG_PARAM;
  }
  else
  {
    if(Lcd_Drv->DisplayOff(Lcd_CompObj) != DRV_ERR_NONE)
    {
      ret = DRV_ERROR_PERIPH_FAILURE;
    }
    else
    {
      ret = DRV_ERR_NONE;
    }
  }

  return ret;
}

int32_t DISP_LCD_SetBrightness(uint32_t Instance, uint32_t Brightness)
{
  int32_t ret = DRV_ERR_NONE;

  if(Instance >= LCD_INSTANCES_NBR)
  {
    ret = DRV_ERROR_WRONG_PARAM;
  }
  else
  {
    if(Lcd_Drv->SetBrightness(Lcd_CompObj, Brightness) != DRV_ERR_NONE)
    {
      ret = DRV_ERROR_PERIPH_FAILURE;
    }
  }

  return ret;
}

int32_t DISP_LCD_GetBrightness(uint32_t Instance, uint32_t *Brightness)
{
  int32_t ret = DRV_ERR_NONE;

  if(Instance >= LCD_INSTANCES_NBR)
  {
    ret = DRV_ERROR_WRONG_PARAM;
  }
  else
  {
    if(Lcd_Drv->GetBrightness(Lcd_CompObj, Brightness) != DRV_ERR_NONE)
    {
      ret = DRV_ERROR_PERIPH_FAILURE;
    }
  }

  return ret;
}

int32_t DISP_LCD_DrawBitmap(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint8_t *pBmp)
{
  int32_t ret = DRV_ERR_NONE;
  uint32_t index, width, height, bit_pixel;
  uint32_t Address;
  uint32_t input_color_mode;
  uint8_t *pbmp;

  /* Get bitmap data address offset */
  index = (uint32_t)pBmp[10] + ((uint32_t)pBmp[11] << 8) + ((uint32_t)pBmp[12] << 16)  + ((uint32_t)pBmp[13] << 24);

  /* Read bitmap width */
  width = (uint32_t)pBmp[18] + ((uint32_t)pBmp[19] << 8) + ((uint32_t)pBmp[20] << 16)  + ((uint32_t)pBmp[21] << 24);

  /* Read bitmap height */
  height = (uint32_t)pBmp[22] + ((uint32_t)pBmp[23] << 8) + ((uint32_t)pBmp[24] << 16)  + ((uint32_t)pBmp[25] << 24);

  /* Read bit/pixel */
  bit_pixel = (uint32_t)pBmp[28] + ((uint32_t)pBmp[29] << 8);

  /* Set the address */
  Address = hlcd_ltdc->LayerCfg[Lcd_Ctx[Instance].ActiveLayer].FBStartAdress + (((Lcd_Ctx[Instance].XSize*Ypos) + Xpos)*Lcd_Ctx[Instance].BppFactor);

  /* Get the layer pixel format */
  if ((bit_pixel/8U) == 4U)
  {
    input_color_mode = DMA2D_INPUT_ARGB8888;
  }
  else if ((bit_pixel/8U) == 2U)
  {
    input_color_mode = DMA2D_INPUT_RGB565;
  }
  else
  {
    input_color_mode = DMA2D_INPUT_RGB888;
  }

  /* Bypass the bitmap header */
  pbmp = pBmp + (index + (width * (height - 1U) * (bit_pixel/8U)));

  /* Convert picture to ARGB8888 pixel format */
  for(index=0; index < height; index++)
  {
    /* Pixel format conversion */
    LL_ConvertLineToRGB(Instance, (uint32_t *)pbmp, (uint32_t *)Address, width, input_color_mode);

    /* Increment the source and destination buffers */
    Address+=  (Lcd_Ctx[Instance].XSize * Lcd_Ctx[Instance].BppFactor);
    pbmp -= width*(bit_pixel/8U);
  }

  return ret;
}

int32_t DISP_LCD_FillRGBRect(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint8_t *pData, uint32_t Width, uint32_t Height)
{
    uint32_t i;

#if (USE_DMA2D_TO_FILL_RGB_RECT == 1)
  uint32_t  Xaddress;
  for(i = 0; i < Height; i++)
  {
    /* Get the line address */
    Xaddress = hlcd_ltdc->LayerCfg[Lcd_Ctx[Instance].ActiveLayer].FBStartAdress + (Lcd_Ctx[Instance].BppFactor*(((Lcd_Ctx[Instance].XSize + i)*Ypos) + Xpos));

    /* Write line */
    if(Lcd_Ctx[Instance].PixelFormat == LCD_PIXEL_FORMAT_RGB565)
    {
      LL_ConvertLineToRGB(Instance, (uint32_t *)pData, (uint32_t *)Xaddress, Width, DMA2D_INPUT_RGB565);
    }
    else
    {
      LL_ConvertLineToRGB(Instance, (uint32_t *)pData, (uint32_t *)Xaddress, Width, DMA2D_INPUT_ARGB8888);
    }
    pData += Lcd_Ctx[Instance].BppFactor*Width;
  }
#else
  uint32_t color, j;
  for(i = 0; i < Height; i++)
  {
    for(j = 0; j < Width; j++)
    {
      color = *pData | (*(pData + 1) << 8) | (*(pData + 2) << 16) | (*(pData + 3) << 24);
      DISP_LCD_WritePixel(Instance, Xpos + j, Ypos + i, color);
      pData += Lcd_Ctx[Instance].BppFactor;
    }
  }
#endif
  return DRV_ERR_NONE;
}

int32_t DISP_LCD_DrawHLine(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t Length, uint32_t Color)
{
  uint32_t  Xaddress;

  /* Get the line address */
  Xaddress = hlcd_ltdc->LayerCfg[Lcd_Ctx[Instance].ActiveLayer].FBStartAdress + (Lcd_Ctx[Instance].BppFactor*((Lcd_Ctx[Instance].XSize*Ypos) + Xpos));

  /* Write line */
  if((Xpos + Length) > Lcd_Ctx[Instance].XSize)
  {
    Length = Lcd_Ctx[Instance].XSize - Xpos;
  }
  LL_FillBuffer(Instance, (uint32_t *)Xaddress, Length, 1, 0, Color);

  return DRV_ERR_NONE;
}

int32_t DISP_LCD_DrawVLine(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t Length, uint32_t Color)
{
  uint32_t  Xaddress;

  /* Get the line address */
  Xaddress = (hlcd_ltdc->LayerCfg[Lcd_Ctx[Instance].ActiveLayer].FBStartAdress) + (Lcd_Ctx[Instance].BppFactor*(Lcd_Ctx[Instance].XSize*Ypos + Xpos));

  /* Write line */
  if((Ypos + Length) > Lcd_Ctx[Instance].YSize)
  {
    Length = Lcd_Ctx[Instance].YSize - Ypos;
  }
 LL_FillBuffer(Instance, (uint32_t *)Xaddress, 1, Length, (Lcd_Ctx[Instance].XSize - 1U), Color);

  return DRV_ERR_NONE;
}

int32_t DISP_LCD_FillRect(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t Width, uint32_t Height, uint32_t Color)
{
  uint32_t  Xaddress;

  /* Get the rectangle start address */
  Xaddress = (hlcd_ltdc->LayerCfg[Lcd_Ctx[Instance].ActiveLayer].FBStartAdress) + (Lcd_Ctx[Instance].BppFactor*(Lcd_Ctx[Instance].XSize*Ypos + Xpos));

  /* Fill the rectangle */
 LL_FillBuffer(Instance, (uint32_t *)Xaddress, Width, Height, (Lcd_Ctx[Instance].XSize - Width), Color);

  return DRV_ERR_NONE;
}

int32_t DISP_LCD_ReadPixel(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t *Color)
{
  if(hlcd_ltdc->LayerCfg[Lcd_Ctx[Instance].ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB8888)
  {
    /* Read data value from SDRAM memory */
    *Color = *(__IO uint32_t*) (hlcd_ltdc->LayerCfg[Lcd_Ctx[Instance].ActiveLayer].FBStartAdress + (4U*(Ypos*Lcd_Ctx[Instance].XSize + Xpos)));
  }
  else /* if((hlcd_ltdc->LayerCfg[layer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565) */
  {
    /* Read data value from SDRAM memory */
    *Color = *(__IO uint16_t*) (hlcd_ltdc->LayerCfg[Lcd_Ctx[Instance].ActiveLayer].FBStartAdress + (2U*(Ypos*Lcd_Ctx[Instance].XSize + Xpos)));
  }

  return DRV_ERR_NONE;
}

int32_t DISP_LCD_WritePixel(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t Color)
{
  if(hlcd_ltdc->LayerCfg[Lcd_Ctx[Instance].ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB8888)
  {
    /* Write data value to SDRAM memory */
    *(__IO uint32_t*) (hlcd_ltdc->LayerCfg[Lcd_Ctx[Instance].ActiveLayer].FBStartAdress + (4U*(Ypos*Lcd_Ctx[Instance].XSize + Xpos))) = Color;
  }
  else
  {
    /* Write data value to SDRAM memory */
    *(__IO uint16_t*) (hlcd_ltdc->LayerCfg[Lcd_Ctx[Instance].ActiveLayer].FBStartAdress + (2U*(Ypos*Lcd_Ctx[Instance].XSize + Xpos))) = Color;
  }

  return DRV_ERR_NONE;
}

static volatile int FlagDmaTransmitEnd = 0;
void DMA2D_TransmitCpltCallBack(DMA2D_HandleTypeDef *hdma2d);

static void LL_FillBuffer(uint32_t Instance, uint32_t *pDst, uint32_t xSize, uint32_t ySize, uint32_t OffLine, uint32_t Color)
{
  uint32_t output_color_mode, input_color = Color;

  switch(Lcd_Ctx[Instance].PixelFormat)
  {
  case LCD_PIXEL_FORMAT_RGB565:
    output_color_mode = DMA2D_OUTPUT_RGB565; /* RGB565 */
    input_color = CONVERTRGB5652ARGB8888(Color);
    break;
  case LCD_PIXEL_FORMAT_RGB888:
  default:
    output_color_mode = DMA2D_OUTPUT_ARGB8888; /* ARGB8888 */
    break;
  }

  /* Register to memory mode with ARGB8888 as color Mode */
  hlcd_dma2d->Init.Mode         = DMA2D_R2M;
  hlcd_dma2d->Init.ColorMode    = output_color_mode;
  hlcd_dma2d->Init.OutputOffset = OffLine;

  hlcd_dma2d->Instance = DMA2D;

//  /*Temporary - wait for previous transmit end..*/
  /************************************************************************************************/
  /*Can't use it because buffers aren't static in this LCD libraries attached from ST*/
  /*I have to wait... on transmission end*/
  /************************************************************************************************/

  /*Register the callback...*/
  hlcd_dma2d->XferCpltCallback = DMA2D_TransmitCpltCallBack;
  FlagDmaTransmitEnd = 1;

  if(HAL_DMA2D_Init(hlcd_dma2d) == HAL_OK)
  {
    if(HAL_DMA2D_ConfigLayer(hlcd_dma2d, 1) == HAL_OK)
    {
      if (HAL_DMA2D_Start_IT(hlcd_dma2d, input_color, (uint32_t)pDst, xSize, ySize) == HAL_OK)
      {

        /* Polling For DMA transfer */
//         (void)HAL_DMA2D_PollForTransfer(hlcd_dma2d, 25);

         while(FlagDmaTransmitEnd == 1)
         {

         }

      }
    }
  }
}

void DMA2D_TransmitCpltCallBack(DMA2D_HandleTypeDef *hdma2d)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdma2d);
  FlagDmaTransmitEnd = 0;

}




int32_t DISP_LCD_LL_FlushBufferDMA2D(uint32_t Instance,
									uint32_t Xpos,
									uint32_t Ypos,
									uint32_t Width,
									uint32_t Height,
									uint32_t *Colormap,
									void(*DMAtrEndCb)(void)
									)
{
  uint32_t  Xaddress;

  Xaddress = (hlcd_ltdc->LayerCfg[Lcd_Ctx[Instance].ActiveLayer].FBStartAdress) + (Lcd_Ctx[Instance].BppFactor*(Lcd_Ctx[Instance].XSize*Ypos + Xpos));

  /* Flush the buffer using dma */
  LL_DMAFlushBuffer(Instance, (uint32_t *)Xaddress, Width, Height, (Lcd_Ctx[Instance].XSize - Width), (uint32_t)Colormap,DMAtrEndCb);

  return DRV_ERR_NONE;
}

static void LL_DMAFlushBuffer(uint32_t Instance, uint32_t *pDst, uint32_t xSize, uint32_t ySize, uint32_t OffLine, uint32_t Color,void(*DMAtrEndCb)(void))
{
  /* Register to memory mode with ARGB8888 as color Mode */
  hlcd_dma2d->Init.Mode         = DMA2D_M2M;
  hlcd_dma2d->Init.ColorMode    = DMA2D_OUTPUT_ARGB8888;
  hlcd_dma2d->Init.OutputOffset = OffLine;

  hlcd_dma2d->Instance = DMA2D;

  hlcd_dma2d->XferCpltCallback = (void*)DMAtrEndCb;

  if(HAL_DMA2D_Init(hlcd_dma2d) == HAL_OK)
  {
    if(HAL_DMA2D_ConfigLayer(hlcd_dma2d, 1) == HAL_OK)
    {
      if (HAL_DMA2D_Start_IT(hlcd_dma2d, Color, (uint32_t)pDst, xSize, ySize) == HAL_OK)
      {

      }
    }
  }
}


static void LL_ConvertLineToRGB(uint32_t Instance, uint32_t *pSrc, uint32_t *pDst, uint32_t xSize, uint32_t ColorMode)
{
  uint32_t output_color_mode;

  switch(Lcd_Ctx[Instance].PixelFormat)
  {
  case LCD_PIXEL_FORMAT_RGB565:
    output_color_mode = DMA2D_OUTPUT_RGB565; /* RGB565 */
    break;
  case LCD_PIXEL_FORMAT_RGB888:
  default:
    output_color_mode = DMA2D_OUTPUT_ARGB8888; /* ARGB8888 */
    break;
  }

  /* Configure the DMA2D Mode, Color Mode and output offset */
  hlcd_dma2d->Init.Mode         = DMA2D_M2M_PFC;
  hlcd_dma2d->Init.ColorMode    = output_color_mode;
  hlcd_dma2d->Init.OutputOffset = 0;

  /* Foreground Configuration */
  hlcd_dma2d->LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
  hlcd_dma2d->LayerCfg[1].InputAlpha = 0xFF;
  hlcd_dma2d->LayerCfg[1].InputColorMode = ColorMode;
  hlcd_dma2d->LayerCfg[1].InputOffset = 0;

  hlcd_dma2d->Instance = DMA2D;

  /* DMA2D Initialization */
  if(HAL_DMA2D_Init(hlcd_dma2d) == HAL_OK)
  {
    if(HAL_DMA2D_ConfigLayer(hlcd_dma2d, 1) == HAL_OK)
    {
      if (HAL_DMA2D_Start(hlcd_dma2d, (uint32_t)pSrc, (uint32_t)pDst, xSize, 1) == HAL_OK)
      {
        /* Polling For DMA transfer */
        (void)HAL_DMA2D_PollForTransfer(hlcd_dma2d, 50);
      }
    }
  }
}

