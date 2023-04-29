
#ifndef __DMA2D_H__
#define __DMA2D_H__

void DMA2D_Init(void);
void DMA2D_InitRefresh(
                        uint32_t InitMode,
                        uint32_t LineOffsetMode,
                        uint32_t AlfaInvMode,
                        uint32_t RedBlueSwap,
                        uint32_t OutputOffset,
                        uint32_t BytesSwap,
                        uint32_t ColorMode
                        );

void DMA2D_SetConfig(uint32_t pdata, uint32_t DstAddress, uint32_t Width,
                            uint32_t Height, uint32_t InitMode, uint32_t ColorMode);

/**
  * @brief DMA2D Layer structure definition
  */
typedef struct
{
  uint32_t             InputOffset;       /*!< Configures the DMA2D foreground or background offset.
                                               This parameter must be a number between
                                               Min_Data = 0x0000 and Max_Data = 0x3FFF. */

  uint32_t             InputColorMode;    /*!< Configures the DMA2D foreground or background color mode.
                                               This parameter can be one value of @ref DMA2D_Input_Color_Mode. */

  uint32_t             AlphaMode;         /*!< Configures the DMA2D foreground or background alpha mode.
                                               This parameter can be one value of @ref DMA2D_Alpha_Mode. */

  uint32_t             InputAlpha;        /*!< Specifies the DMA2D foreground or background alpha value and color value
                                               in case of A8 or A4 color mode.
                                               This parameter must be a number between Min_Data = 0x00
                                               and Max_Data = 0xFF except for the color modes detailed below.
                                               @note In case of A8 or A4 color mode (ARGB),
                                               this parameter must be a number between
                                               Min_Data = 0x00000000 and Max_Data = 0xFFFFFFFF where
                                               - InputAlpha[24:31] is the alpha value ALPHA[0:7]
                                               - InputAlpha[16:23] is the red value RED[0:7]
                                               - InputAlpha[8:15] is the green value GREEN[0:7]
                                               - InputAlpha[0:7] is the blue value BLUE[0:7]. */
  uint32_t             AlphaInverted;     /*!< Select regular or inverted alpha value.
                                               This parameter can be one value of @ref DMA2D_Alpha_Inverted. */

  uint32_t             RedBlueSwap;       /*!< Select regular mode (RGB or ARGB) or swap mode (BGR or ABGR).
                                               This parameter can be one value of @ref DMA2D_RB_Swap. */

  uint32_t             ChromaSubSampling; /*!< Configure the chroma sub-sampling mode for the YCbCr color mode
                                               This parameter can be one value of @ref DMA2D_Chroma_Sub_Sampling */

} DMA2D_LayerCfgTypeDef;




/** @defgroup DMA2D_Error_Code DMA2D Error Code
  * @{
  */
#define HAL_DMA2D_ERROR_NONE        0x00000000U  /*!< No error             */
#define HAL_DMA2D_ERROR_TE          0x00000001U  /*!< Transfer error       */
#define HAL_DMA2D_ERROR_CE          0x00000002U  /*!< Configuration error  */
#define HAL_DMA2D_ERROR_CAE         0x00000004U  /*!< CLUT access error    */
#define HAL_DMA2D_ERROR_TIMEOUT     0x00000020U  /*!< Timeout error        */
#if (USE_HAL_DMA2D_REGISTER_CALLBACKS == 1)
#define HAL_DMA2D_ERROR_INVALID_CALLBACK 0x00000040U  /*!< Invalid callback error  */
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */

/**
  * @}
  */

/** @defgroup DMA2D_Mode DMA2D Mode
  * @{
  */
#define DMA2D_M2M                   0x00000000U                         /*!< DMA2D memory to memory transfer mode */
#define DMA2D_M2M_PFC               DMA2D_CR_MODE_0                     /*!< DMA2D memory to memory with pixel format conversion transfer mode */
#define DMA2D_M2M_BLEND             DMA2D_CR_MODE_1                     /*!< DMA2D memory to memory with blending transfer mode */
#define DMA2D_R2M                   (DMA2D_CR_MODE_1 | DMA2D_CR_MODE_0) /*!< DMA2D register to memory transfer mode */
#define DMA2D_M2M_BLEND_FG          DMA2D_CR_MODE_2                     /*!< DMA2D memory to memory with blending transfer mode and fixed color FG */
#define DMA2D_M2M_BLEND_BG          (DMA2D_CR_MODE_2 | DMA2D_CR_MODE_0) /*!< DMA2D memory to memory with blending transfer mode and fixed color BG */
/**
  * @}
  */

/** @defgroup DMA2D_Output_Color_Mode DMA2D Output Color Mode
  * @{
  */
#define DMA2D_OUTPUT_ARGB8888       0x00000000U                           /*!< ARGB8888 DMA2D color mode */
#define DMA2D_OUTPUT_RGB888         DMA2D_OPFCCR_CM_0                     /*!< RGB888 DMA2D color mode   */
#define DMA2D_OUTPUT_RGB565         DMA2D_OPFCCR_CM_1                     /*!< RGB565 DMA2D color mode   */
#define DMA2D_OUTPUT_ARGB1555       (DMA2D_OPFCCR_CM_0|DMA2D_OPFCCR_CM_1) /*!< ARGB1555 DMA2D color mode */
#define DMA2D_OUTPUT_ARGB4444       DMA2D_OPFCCR_CM_2                     /*!< ARGB4444 DMA2D color mode */
/**
  * @}
  */

/** @defgroup DMA2D_Input_Color_Mode DMA2D Input Color Mode
  * @{
  */
#define DMA2D_INPUT_ARGB8888        0x00000000U  /*!< ARGB8888 color mode */
#define DMA2D_INPUT_RGB888          0x00000001U  /*!< RGB888 color mode   */
#define DMA2D_INPUT_RGB565          0x00000002U  /*!< RGB565 color mode   */
#define DMA2D_INPUT_ARGB1555        0x00000003U  /*!< ARGB1555 color mode */
#define DMA2D_INPUT_ARGB4444        0x00000004U  /*!< ARGB4444 color mode */
#define DMA2D_INPUT_L8              0x00000005U  /*!< L8 color mode       */
#define DMA2D_INPUT_AL44            0x00000006U  /*!< AL44 color mode     */
#define DMA2D_INPUT_AL88            0x00000007U  /*!< AL88 color mode     */
#define DMA2D_INPUT_L4              0x00000008U  /*!< L4 color mode       */
#define DMA2D_INPUT_A8              0x00000009U  /*!< A8 color mode       */
#define DMA2D_INPUT_A4              0x0000000AU  /*!< A4 color mode       */
#define DMA2D_INPUT_YCBCR           0x0000000BU  /*!< YCbCr color mode */
/**
  * @}
  */

/** @defgroup DMA2D_Alpha_Mode DMA2D Alpha Mode
  * @{
  */
#define DMA2D_NO_MODIF_ALPHA        0x00000000U  /*!< No modification of the alpha channel value                     */
#define DMA2D_REPLACE_ALPHA         0x00000001U  /*!< Replace original alpha channel value by programmed alpha value */
#define DMA2D_COMBINE_ALPHA         0x00000002U  /*!< Replace original alpha channel value by programmed alpha value
                                                      with original alpha channel value                              */
/**
  * @}
  */

/** @defgroup DMA2D_Alpha_Inverted DMA2D Alpha Inversion
  * @{
  */
#define DMA2D_REGULAR_ALPHA         0x00000000U  /*!< No modification of the alpha channel value */
#define DMA2D_INVERTED_ALPHA        0x00000001U  /*!< Invert the alpha channel value */
/**
  * @}
  */

/** @defgroup DMA2D_RB_Swap DMA2D Red and Blue Swap
  * @{
  */
#define DMA2D_RB_REGULAR            0x00000000U  /*!< Select regular mode (RGB or ARGB) */
#define DMA2D_RB_SWAP               0x00000001U  /*!< Select swap mode (BGR or ABGR) */
/**
  * @}
  */



/** @defgroup DMA2D_Line_Offset_Mode DMA2D Line Offset Mode
  * @{
  */
#define DMA2D_LOM_PIXELS            0x00000000U   /*!< Line offsets expressed in pixels */
#define DMA2D_LOM_BYTES             DMA2D_CR_LOM  /*!< Line offsets expressed in bytes */
/**
  * @}
  */

/** @defgroup DMA2D_Bytes_Swap DMA2D Bytes Swap
  * @{
  */
#define DMA2D_BYTES_REGULAR         0x00000000U      /*!< Bytes in regular order in output FIFO */
#define DMA2D_BYTES_SWAP            DMA2D_OPFCCR_SB  /*!< Bytes are swapped two by two in output FIFO */
/**
  * @}
  */

/** @defgroup DMA2D_Chroma_Sub_Sampling DMA2D Chroma Sub Sampling
  * @{
  */
#define DMA2D_NO_CSS               0x00000000U  /*!< No chroma sub-sampling 4:4:4 */
#define DMA2D_CSS_422              0x00000001U  /*!< chroma sub-sampling 4:2:2 */
#define DMA2D_CSS_420              0x00000002U  /*!< chroma sub-sampling 4:2:0 */
/**
  * @}
  */

/** @defgroup DMA2D_CLUT_CM DMA2D CLUT Color Mode
  * @{
  */
#define DMA2D_CCM_ARGB8888          0x00000000U  /*!< ARGB8888 DMA2D CLUT color mode */
#define DMA2D_CCM_RGB888            0x00000001U  /*!< RGB888 DMA2D CLUT color mode   */
/**
  * @}
  */

/** @defgroup DMA2D_Interrupts DMA2D Interrupts
  * @{
  */
#define DMA2D_IT_CE                 DMA2D_CR_CEIE            /*!< Configuration Error Interrupt */
#define DMA2D_IT_CTC                DMA2D_CR_CTCIE           /*!< CLUT Transfer Complete Interrupt */
#define DMA2D_IT_CAE                DMA2D_CR_CAEIE           /*!< CLUT Access Error Interrupt */
#define DMA2D_IT_TW                 DMA2D_CR_TWIE            /*!< Transfer Watermark Interrupt */
#define DMA2D_IT_TC                 DMA2D_CR_TCIE            /*!< Transfer Complete Interrupt */
#define DMA2D_IT_TE                 DMA2D_CR_TEIE            /*!< Transfer Error Interrupt */
/**
  * @}
  */

/** @defgroup DMA2D_Flags DMA2D Flags
  * @{
  */
#define DMA2D_FLAG_CE               DMA2D_ISR_CEIF           /*!< Configuration Error Interrupt Flag */
#define DMA2D_FLAG_CTC              DMA2D_ISR_CTCIF          /*!< CLUT Transfer Complete Interrupt Flag */
#define DMA2D_FLAG_CAE              DMA2D_ISR_CAEIF          /*!< CLUT Access Error Interrupt Flag */
#define DMA2D_FLAG_TW               DMA2D_ISR_TWIF           /*!< Transfer Watermark Interrupt Flag */
#define DMA2D_FLAG_TC               DMA2D_ISR_TCIF           /*!< Transfer Complete Interrupt Flag */
#define DMA2D_FLAG_TE               DMA2D_ISR_TEIF           /*!< Transfer Error Interrupt Flag */
/**
  * @}
  */


/** @defgroup DMA2D_Maximum_Line_WaterMark DMA2D Maximum Line Watermark
  * @{
  */
#define DMA2D_LINE_WATERMARK_MAX            DMA2D_LWR_LW       /*!< DMA2D maximum line watermark */
/**
  * @}
  */

/** @defgroup DMA2D_Color_Value DMA2D Color Value
  * @{
  */
#define DMA2D_COLOR_VALUE                 0x000000FFU  /*!< Color value mask */
/**
  * @}
  */

/** @defgroup DMA2D_Max_Layer DMA2D Maximum Number of Layers
  * @{
  */
#define DMA2D_MAX_LAYER         2U         /*!< DMA2D maximum number of layers */
/**
  * @}
  */

/** @defgroup DMA2D_Layers DMA2D Layers
  * @{
  */
#define DMA2D_BACKGROUND_LAYER             0x00000000U   /*!< DMA2D Background Layer (layer 0) */
#define DMA2D_FOREGROUND_LAYER             0x00000001U   /*!< DMA2D Foreground Layer (layer 1) */
/**
  * @}
  */

/** @defgroup DMA2D_Offset DMA2D Offset
  * @{
  */
#define DMA2D_OFFSET                DMA2D_FGOR_LO            /*!< maximum Line Offset */
/**
  * @}
  */

/** @defgroup DMA2D_Size DMA2D Size
  * @{
  */
#define DMA2D_PIXEL                 (DMA2D_NLR_PL >> 16U)    /*!< DMA2D maximum number of pixels per line */
#define DMA2D_LINE                  DMA2D_NLR_NL             /*!< DMA2D maximum number of lines           */
/**
  * @}
  */

/** @defgroup DMA2D_CLUT_Size DMA2D CLUT Size
  * @{
  */
#define DMA2D_CLUT_SIZE             (DMA2D_FGPFCCR_CS >> 8U)  /*!< DMA2D maximum CLUT size */
/**
  * @}
  */


#endif /*__DMA2D_H__*/