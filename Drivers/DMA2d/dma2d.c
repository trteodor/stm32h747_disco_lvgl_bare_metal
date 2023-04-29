#include "stm32h7xx.h"

#include "dma2d.h"




void DMA2D_Init(void)
{
    SET_BIT(RCC->AHB3ENR, RCC_AHB3ENR_DMA2DEN);

// #warning TODO!!!!
    // DMA2D_InitRefresh(DMA2D_R2M,DMA2D_LOM_PIXELS,DMA2D_REGULAR_ALPHA,DMA2D_RB_REGULAR);

  // DMA2D_InitRefresh(
  //                       DMA2D_R2M, /*Init mode*/
  //                       DMA2D_LOM_PIXELS, /*Line offset mode*/
  //                       DMA2D_REGULAR_ALPHA,
  //                       DMA2D_RB_REGULAR,
  //                       (Lcd_Ctx[Instance].XSize - Width), /*output offset*/
  //                       DMA2D_BYTES_REGULAR,
  //                       DMA2D_OUTPUT_ARGB8888
  //                       )

    /* DMA2D interrupt Init */
    NVIC_SetPriority (DMA2D_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);
    NVIC_EnableIRQ(DMA2D_IRQn);
}


void DMA2D_InitRefresh(
                        uint32_t InitMode,
                        uint32_t LineOffsetMode,
                        uint32_t AlfaInvMode,
                        uint32_t RedBlueSwap,
                        uint32_t OutputOffset,
                        uint32_t BytesSwap,
                        uint32_t ColorMode
                        )
{
            /* DMA2D CR register configuration -------------------------------------------*/
    MODIFY_REG(DMA2D->CR, DMA2D_CR_MODE | DMA2D_CR_LOM, InitMode | LineOffsetMode);

    /* DMA2D OPFCCR register configuration ---------------------------------------*/
    MODIFY_REG(DMA2D->OPFCCR, DMA2D_OPFCCR_CM | DMA2D_OPFCCR_SB,
                ColorMode | BytesSwap);

    /* DMA2D OOR register configuration ------------------------------------------*/
    MODIFY_REG(DMA2D->OOR, DMA2D_OOR_LO, OutputOffset);
    /* DMA2D OPFCCR AI and RBS fields setting (Output Alpha Inversion)*/
    MODIFY_REG(DMA2D->OPFCCR, (DMA2D_OPFCCR_AI | DMA2D_OPFCCR_RBS),
                ((AlfaInvMode << DMA2D_OPFCCR_AI_Pos) | \
                (RedBlueSwap << DMA2D_OPFCCR_RBS_Pos)));
}




void DMA2D_SetConfig(uint32_t pdata, uint32_t DstAddress, uint32_t Width,
                            uint32_t Height, uint32_t InitMode, uint32_t ColorMode)
{
  uint32_t tmp;
  uint32_t tmp1;
  uint32_t tmp2;
  uint32_t tmp3;
  uint32_t tmp4;

  /* Configure DMA2D data size */
  MODIFY_REG(DMA2D->NLR, (DMA2D_NLR_NL | DMA2D_NLR_PL), (Height | (Width << DMA2D_NLR_PL_Pos)));

  /* Configure DMA2D destination address */
  WRITE_REG(DMA2D->OMAR, DstAddress);

  /* Register to memory DMA2D mode selected */
  if (InitMode == DMA2D_R2M)
  {
    tmp1 = pdata & DMA2D_OCOLR_ALPHA_1;
    tmp2 = pdata & DMA2D_OCOLR_RED_1;
    tmp3 = pdata & DMA2D_OCOLR_GREEN_1;
    tmp4 = pdata & DMA2D_OCOLR_BLUE_1;

    /* Prepare the value to be written to the OCOLR register according to the color mode */
    if (ColorMode == DMA2D_OUTPUT_ARGB8888)
    {
      tmp = (tmp3 | tmp2 | tmp1 | tmp4);
    }
    else if (ColorMode == DMA2D_OUTPUT_RGB888)
    {
      tmp = (tmp3 | tmp2 | tmp4);
    }
    else if (ColorMode == DMA2D_OUTPUT_RGB565)
    {
      tmp2 = (tmp2 >> 19U);
      tmp3 = (tmp3 >> 10U);
      tmp4 = (tmp4 >> 3U);
      tmp  = ((tmp3 << 5U) | (tmp2 << 11U) | tmp4);
    }
    else if (ColorMode == DMA2D_OUTPUT_ARGB1555)
    {
      tmp1 = (tmp1 >> 31U);
      tmp2 = (tmp2 >> 19U);
      tmp3 = (tmp3 >> 11U);
      tmp4 = (tmp4 >> 3U);
      tmp  = ((tmp3 << 5U) | (tmp2 << 10U) | (tmp1 << 15U) | tmp4);
    }
    else /* DColorMode = DMA2D_OUTPUT_ARGB4444 */
    {
      tmp1 = (tmp1 >> 28U);
      tmp2 = (tmp2 >> 20U);
      tmp3 = (tmp3 >> 12U);
      tmp4 = (tmp4 >> 4U);
      tmp  = ((tmp3 << 4U) | (tmp2 << 8U) | (tmp1 << 12U) | tmp4);
    }
    /* Write to DMA2D OCOLR register */
    WRITE_REG(DMA2D->OCOLR, tmp);
  }
  else if (InitMode == DMA2D_M2M_BLEND_FG) /*M2M_blending with fixed color FG DMA2D Mode selected*/
  {
    WRITE_REG(DMA2D->BGMAR, pdata);
  }
  else /* M2M, M2M_PFC,M2M_Blending or M2M_blending with fixed color BG DMA2D Mode */
  {
    /* Configure DMA2D source address */
    WRITE_REG(DMA2D->FGMAR, pdata);
  }
}

void DMA2D_StartIT(void)
{
    DMA2D->CR |= DMA2D_CR_TCIE | DMA2D_CR_TEIE | DMA2D_CR_CEIE;
    /* Enable the Peripheral */
    DMA2D->CR |= DMA2D_CR_START;
}


void DMA2D_ConfigLayer(DMA2D_LayerCfgTypeDef *pLayerCfg, uint32_t LayerIdx)
{
  uint32_t regMask;
  uint32_t regValue;


  /* Prepare the value to be written to the BGPFCCR or FGPFCCR register */
  regValue = pLayerCfg[LayerIdx].InputColorMode | (pLayerCfg[LayerIdx].AlphaMode << DMA2D_BGPFCCR_AM_Pos) | \
             (pLayerCfg[LayerIdx].AlphaInverted << DMA2D_BGPFCCR_AI_Pos) | (pLayerCfg[LayerIdx].RedBlueSwap << DMA2D_BGPFCCR_RBS_Pos);
  regMask  = (DMA2D_BGPFCCR_CM | DMA2D_BGPFCCR_AM | DMA2D_BGPFCCR_ALPHA | DMA2D_BGPFCCR_AI | DMA2D_BGPFCCR_RBS);


  if ((pLayerCfg[LayerIdx].InputColorMode == DMA2D_INPUT_A4) || (pLayerCfg[LayerIdx].InputColorMode == DMA2D_INPUT_A8))
  {
    regValue |= (pLayerCfg[LayerIdx].InputAlpha & DMA2D_BGPFCCR_ALPHA);
  }
  else
  {
    regValue |= (pLayerCfg[LayerIdx].InputAlpha << DMA2D_BGPFCCR_ALPHA_Pos);
  }

  /* Configure the background DMA2D layer */
  if (LayerIdx == DMA2D_BACKGROUND_LAYER)
  {
    /* Write DMA2D BGPFCCR register */
    MODIFY_REG(DMA2D->BGPFCCR, regMask, regValue);

    /* DMA2D BGOR register configuration -------------------------------------*/
    WRITE_REG(DMA2D->BGOR, pLayerCfg[LayerIdx].InputOffset);

    /* DMA2D BGCOLR register configuration -------------------------------------*/
    if ((pLayerCfg[LayerIdx].InputColorMode == DMA2D_INPUT_A4) || (pLayerCfg[LayerIdx].InputColorMode == DMA2D_INPUT_A8))
    {
      WRITE_REG(DMA2D->BGCOLR, pLayerCfg[LayerIdx].InputAlpha & (DMA2D_BGCOLR_BLUE | DMA2D_BGCOLR_GREEN | \
                                                                   DMA2D_BGCOLR_RED));
    }
  }
  /* Configure the foreground DMA2D layer */
  else
  {

    if (pLayerCfg[LayerIdx].InputColorMode == DMA2D_INPUT_YCBCR)
    {
      regValue |= (pLayerCfg[LayerIdx].ChromaSubSampling << DMA2D_FGPFCCR_CSS_Pos);
      regMask  |= DMA2D_FGPFCCR_CSS;
    }

    /* Write DMA2D FGPFCCR register */
    MODIFY_REG(DMA2D->FGPFCCR, regMask, regValue);

    /* DMA2D FGOR register configuration -------------------------------------*/
    WRITE_REG(DMA2D->FGOR, pLayerCfg[LayerIdx].InputOffset);

    /* DMA2D FGCOLR register configuration -------------------------------------*/
    if ((pLayerCfg[LayerIdx].InputColorMode == DMA2D_INPUT_A4) || (pLayerCfg[LayerIdx].InputColorMode == DMA2D_INPUT_A8))
    {
      WRITE_REG(DMA2D->FGCOLR, pLayerCfg[LayerIdx].InputAlpha & (DMA2D_FGCOLR_BLUE | DMA2D_FGCOLR_GREEN | \
                                                                   DMA2D_FGCOLR_RED));
    }
  }

}