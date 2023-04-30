
#include "stm32h7xx.h"
#include "ltdc.h"


#define SDRAM_DEVICE_ADDR_LTDC         0xD0000000U


void LTDC_MspInit(void);
void LTDC_PeriphInit(void);
void LTDC_ConfigLayer(LTDC_LayerCfgTypeDef *pLayerCfg, uint32_t LayerIdx);

void LTDC_Init(void)
{
    LTDC_MspInit();
    LTDC_PeriphInit();
    LTDC_LayertCommandModeInit(0,SDRAM_DEVICE_ADDR_LTDC);
}

void LTDC_LayertCommandModeInit(uint16_t LayerIndex, uint32_t Address)
{
    LTDC_LayerCfgTypeDef  layercfg;

  /* Layer Init */
  layercfg.WindowX0 = 0;
  layercfg.WindowX1 = 800/8 ;
  layercfg.WindowY0 = 0;
  layercfg.WindowY1 = 480; 
  layercfg.PixelFormat = LTDC_PIXEL_FORMAT_ARGB8888;
  layercfg.FBStartAdress = Address;
  layercfg.Alpha = 255;
  layercfg.Alpha0 = 0;
  layercfg.Backcolor.Blue = 0;
  layercfg.Backcolor.Green = 0;
  layercfg.Backcolor.Red = 0;
  layercfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
  layercfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
  layercfg.ImageWidth = 800/8;
  layercfg.ImageHeight = 480;
  LTDC_ConfigLayer(&layercfg, LayerIndex); 
}

/*MSP - MCU Support Package.*/
void LTDC_MspInit(void)
{
    /* Disable  PLL3. */
    CLEAR_BIT(RCC->CR, RCC_CR_PLL3ON);



    /* Wait till PLL3 is ready */
    while(___RCC_GET_FLAG(RCC_FLAG_PLL3RDY) != 0U){}

    /* Configure the PLL3  multiplication and division factors. */
    ___RCC_PLL3_CONFIG(   5,
                          132, /*M, N, P, Q, R*/
                          2,
                          2,
                          24);

    /* Select PLL3 input reference frequency range: VCI */
    MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLL3RGE, RCC_PLLCFGR_PLL3RGE_2);


    /* Select PLL3 output frequency range : VCO */
    MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLL3VCOSEL, RCC_PLL3VCOWIDE);

    /* Disable PLL3FRACN . */
    CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLL3FRACEN);

    /* Configures PLL3 clock Fractional Part Of The Multiplication Factor */
    MODIFY_REG(RCC->PLL3FRACR, RCC_PLL3FRACR_FRACN3, (uint32_t)(0) << RCC_PLL3FRACR_FRACN3_Pos);

    /* Enable PLL3FRACN . */
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLL3FRACEN);

    /* Enable the PLL3 clock output */
     SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_DIVR3EN);

    /* Enable  PLL3. */
    SET_BIT(RCC->CR, RCC_CR_PLL3ON);


    /* Wait till PLL3 is ready */
    while(___RCC_GET_FLAG(RCC_FLAG_PLL3RDY) == 0U){}

    /* LTDC clock enable */
    SET_BIT(RCC->APB3ENR, RCC_APB3ENR_LTDCEN);

    /* LTDC interrupt Init */
    NVIC_SetPriority (LTDC_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);
    NVIC_EnableIRQ(LTDC_IRQn);
}


void LTDC_PeriphInit(void)
{
    uint32_t tmp;
    uint32_t tmp1;

    LTDC_InitTypeDef LTDC_InitStruct;
    LTDC_InitStruct.HorizontalSync = HSYNC;
    LTDC_InitStruct.VerticalSync = VSYNC;
    LTDC_InitStruct.AccumulatedHBP = HSYNC+HBP;
    LTDC_InitStruct.AccumulatedVBP = VSYNC+VBP;
    LTDC_InitStruct.AccumulatedActiveH = VSYNC+VBP+VACT;
    LTDC_InitStruct.AccumulatedActiveW = HSYNC+HBP+HACT;
    LTDC_InitStruct.TotalHeigh = VSYNC+VBP+VACT+VFP;
    LTDC_InitStruct.TotalWidth = HSYNC+HBP+HACT+HFP;
    /* background value */
    LTDC_InitStruct.Backcolor.Blue = 0;
    LTDC_InitStruct.Backcolor.Green = 0;
    LTDC_InitStruct.Backcolor.Red = 0;
    /* Polarity */
    LTDC_InitStruct.HSPolarity = LTDC_HSPOLARITY_AL;
    LTDC_InitStruct.VSPolarity = LTDC_VSPOLARITY_AL;
    LTDC_InitStruct.DEPolarity = LTDC_DEPOLARITY_AL;
    LTDC_InitStruct.PCPolarity = LTDC_PCPOLARITY_IPC;

    /* Configure the HS, VS, DE and PC polarity */
    LTDC->GCR &= ~(LTDC_GCR_HSPOL | LTDC_GCR_VSPOL | LTDC_GCR_DEPOL | LTDC_GCR_PCPOL);
    LTDC->GCR |= (uint32_t)(LTDC_InitStruct.HSPolarity | LTDC_InitStruct.VSPolarity | \
                                        LTDC_InitStruct.DEPolarity | LTDC_InitStruct.PCPolarity);

    /* Set Synchronization size */
    LTDC->SSCR &= ~(LTDC_SSCR_VSH | LTDC_SSCR_HSW);
    tmp = (LTDC_InitStruct.HorizontalSync << 16U);
    LTDC->SSCR |= (tmp | LTDC_InitStruct.VerticalSync);

    /* Set Accumulated Back porch */
    LTDC->BPCR &= ~(LTDC_BPCR_AVBP | LTDC_BPCR_AHBP);
    tmp = (LTDC_InitStruct.AccumulatedHBP << 16U);
    LTDC->BPCR |= (tmp | LTDC_InitStruct.AccumulatedVBP);

    /* Set Accumulated Active Width */
    LTDC->AWCR &= ~(LTDC_AWCR_AAH | LTDC_AWCR_AAW);
    tmp = (LTDC_InitStruct.AccumulatedActiveW << 16U);
    LTDC->AWCR |= (tmp | LTDC_InitStruct.AccumulatedActiveH);

    /* Set Total Width */
    LTDC->TWCR &= ~(LTDC_TWCR_TOTALH | LTDC_TWCR_TOTALW);
    tmp = (LTDC_InitStruct.TotalWidth << 16U);
    LTDC->TWCR |= (tmp | LTDC_InitStruct.TotalHeigh);

    /* Set the background color value */
    tmp = ((uint32_t)(LTDC_InitStruct.Backcolor.Green) << 8U);
    tmp1 = ((uint32_t)(LTDC_InitStruct.Backcolor.Red) << 16U);
    LTDC->BCCR &= ~(LTDC_BCCR_BCBLUE | LTDC_BCCR_BCGREEN | LTDC_BCCR_BCRED);
    LTDC->BCCR |= (tmp1 | tmp | LTDC_InitStruct.Backcolor.Blue);


    /* Enable the Transfer Error and FIFO underrun interrupts */
    LTDC->IER |= (LTDC_IER_TERRIE | LTDC_IER_FUIE);

    /* Enable LTDC by setting LTDCEN bit */
    LTDC->GCR |= LTDC_GCR_LTDCEN;
}


void LTDC_ConfigLayer(LTDC_LayerCfgTypeDef *pLayerCfg, uint32_t LayerIdx)
{
    /* Copy new layer configuration into handle structure */

    /* Configure the LTDC Layer */
    uint32_t tmp;
    uint32_t tmp1;
    uint32_t tmp2;

    /* Configure the horizontal start and stop position */
    tmp = ((pLayerCfg->WindowX1 + ((LTDC->BPCR & LTDC_BPCR_AHBP) >> 16U)) << 16U);
    LTDC_LAYER(LayerIdx)->WHPCR &= ~(LTDC_LxWHPCR_WHSTPOS | LTDC_LxWHPCR_WHSPPOS);
    LTDC_LAYER(LayerIdx)->WHPCR = ((pLayerCfg->WindowX0 + \
                                            ((LTDC->BPCR & LTDC_BPCR_AHBP) >> 16U) + 1U) | tmp);

    /* Configure the vertical start and stop position */
    tmp = ((pLayerCfg->WindowY1 + (LTDC->BPCR & LTDC_BPCR_AVBP)) << 16U);
    LTDC_LAYER(LayerIdx)->WVPCR &= ~(LTDC_LxWVPCR_WVSTPOS | LTDC_LxWVPCR_WVSPPOS);
    LTDC_LAYER(LayerIdx)->WVPCR  = ((pLayerCfg->WindowY0 + (LTDC->BPCR & LTDC_BPCR_AVBP) + 1U) | tmp);

    /* Specifies the pixel format */
    LTDC_LAYER(LayerIdx)->PFCR &= ~(LTDC_LxPFCR_PF);
    LTDC_LAYER(LayerIdx)->PFCR = (pLayerCfg->PixelFormat);

    /* Configure the default color values */
    tmp = ((uint32_t)(pLayerCfg->Backcolor.Green) << 8U);
    tmp1 = ((uint32_t)(pLayerCfg->Backcolor.Red) << 16U);
    tmp2 = (pLayerCfg->Alpha0 << 24U);
    LTDC_LAYER(LayerIdx)->DCCR &= ~(LTDC_LxDCCR_DCBLUE | LTDC_LxDCCR_DCGREEN | LTDC_LxDCCR_DCRED |
                                            LTDC_LxDCCR_DCALPHA);
    LTDC_LAYER(LayerIdx)->DCCR = (pLayerCfg->Backcolor.Blue | tmp | tmp1 | tmp2);

    /* Specifies the constant alpha value */
    LTDC_LAYER(LayerIdx)->CACR &= ~(LTDC_LxCACR_CONSTA);
    LTDC_LAYER(LayerIdx)->CACR = (pLayerCfg->Alpha);

    /* Specifies the blending factors */
    LTDC_LAYER(LayerIdx)->BFCR &= ~(LTDC_LxBFCR_BF2 | LTDC_LxBFCR_BF1);
    LTDC_LAYER(LayerIdx)->BFCR = (pLayerCfg->BlendingFactor1 | pLayerCfg->BlendingFactor2);

    /* Configure the color frame buffer start address */
    LTDC_LAYER(LayerIdx)->CFBAR &= ~(LTDC_LxCFBAR_CFBADD);
    LTDC_LAYER(LayerIdx)->CFBAR = (pLayerCfg->FBStartAdress);

    if (pLayerCfg->PixelFormat == LTDC_PIXEL_FORMAT_ARGB8888)
    {
    tmp = 4U;
    }
    else if (pLayerCfg->PixelFormat == LTDC_PIXEL_FORMAT_RGB888)
    {
    tmp = 3U;
    }
    else if ((pLayerCfg->PixelFormat == LTDC_PIXEL_FORMAT_ARGB4444) || \
            (pLayerCfg->PixelFormat == LTDC_PIXEL_FORMAT_RGB565)   || \
            (pLayerCfg->PixelFormat == LTDC_PIXEL_FORMAT_ARGB1555) || \
            (pLayerCfg->PixelFormat == LTDC_PIXEL_FORMAT_AL88))
    {
    tmp = 2U;
    }
    else
    {
    tmp = 1U;
    }

    /* Configure the color frame buffer pitch in byte */
    LTDC_LAYER(LayerIdx)->CFBLR  &= ~(LTDC_LxCFBLR_CFBLL | LTDC_LxCFBLR_CFBP);
    LTDC_LAYER(LayerIdx)->CFBLR  = (((pLayerCfg->ImageWidth * tmp) << 16U) | (((pLayerCfg->WindowX1 - pLayerCfg->WindowX0) * tmp)  + 7U));
    /* Configure the frame buffer line number */
    LTDC_LAYER(LayerIdx)->CFBLNR  &= ~(LTDC_LxCFBLNR_CFBLNBR);
    LTDC_LAYER(LayerIdx)->CFBLNR  = (pLayerCfg->ImageHeight);

    /* Enable LTDC_Layer by setting LEN bit */
    LTDC_LAYER(LayerIdx)->CR |= (uint32_t)LTDC_LxCR_LEN;

    /* Set the Immediate Reload type */
    LTDC->SRCR = LTDC_SRCR_IMR;
}



void LTDC_SetPitch(uint32_t PixelFormatIn, uint32_t LinePitchInPixels, uint32_t LayerIdx)
{
  uint32_t tmp;
  uint32_t pitchUpdate;
  uint32_t pixelFormat;

  /* get LayerIdx used pixel format */
  pixelFormat = PixelFormatIn;

  if (pixelFormat == LTDC_PIXEL_FORMAT_ARGB8888)
  {
    tmp = 4U;
  }
  else if (pixelFormat == LTDC_PIXEL_FORMAT_RGB888)
  {
    tmp = 3U;
  }
  else if ((pixelFormat == LTDC_PIXEL_FORMAT_ARGB4444) || \
           (pixelFormat == LTDC_PIXEL_FORMAT_RGB565)   || \
           (pixelFormat == LTDC_PIXEL_FORMAT_ARGB1555) || \
           (pixelFormat == LTDC_PIXEL_FORMAT_AL88))
  {
    tmp = 2U;
  }
  else
  {
    tmp = 1U;
  }

  pitchUpdate = ((LinePitchInPixels * tmp) << 16U);

  /* Clear previously set standard pitch */
  LTDC_LAYER(LayerIdx)->CFBLR &= ~LTDC_LxCFBLR_CFBP;
  /* Set the Reload type as immediate update of LTDC pitch configured above */
  LTDC->SRCR |= LTDC_SRCR_IMR;
  /* Set new line pitch value */
  LTDC_LAYER(LayerIdx)->CFBLR |= pitchUpdate;
  /* Set the Reload type as immediate update of LTDC pitch configured above */
  LTDC->SRCR |= LTDC_SRCR_IMR;
}