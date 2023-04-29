

#ifndef __LTDC_H_
#define __LTDC_H_

#include "stm32h747xx.h"

#define LTDC_HORIZONTAL_SYNC       1
#define LTDC_VERTICAL_SYNC         0
#define LTDC_ACCUMULATED_HBP       35
#define LTDC_ACCUMULATED_VBP       15
#define LTDC_ACCUMULATED_ACTIV_W   835
#define LTDC_ACCUMULATED_ACTIV_H   495
#define LTDC_TOTAL_WIDTH           869
#define LTDC_TOTAL_HEIGH           511
#define LTDC_BACK_COLOR_BLUE       0
#define LTDC_BACK_COLOR_GREEN      0
#define LTDC_BACK_COLOR_RED        20




void LTDC_Init(void);






typedef struct
{
  uint8_t Blue;                    /*!< Configures the blue value.
                                        This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF. */

  uint8_t Green;                   /*!< Configures the green value.
                                        This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF. */

  uint8_t Red;                     /*!< Configures the red value.
                                        This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF. */

  uint8_t Reserved;                /*!< Reserved 0xFF */
} LTDC_ColorTypeDef;


/**
  * @brief  LTDC Layer structure definition
  */
typedef struct
{
  uint32_t WindowX0;                   /*!< Configures the Window Horizontal Start Position.
                                            This parameter must be a number between
                                            Min_Data = 0x000 and Max_Data = 0xFFF. */

  uint32_t WindowX1;                   /*!< Configures the Window Horizontal Stop Position.
                                            This parameter must be a number between
                                            Min_Data = 0x000 and Max_Data = 0xFFF. */

  uint32_t WindowY0;                   /*!< Configures the Window vertical Start Position.
                                            This parameter must be a number between
                                            Min_Data = 0x000 and Max_Data = 0x7FF. */

  uint32_t WindowY1;                   /*!< Configures the Window vertical Stop Position.
                                            This parameter must be a number between
                                            Min_Data = 0x0000 and Max_Data = 0x7FF. */

  uint32_t PixelFormat;                /*!< Specifies the pixel format.
                                            This parameter can be one of value of @ref LTDC_Pixelformat */

  uint32_t Alpha;                      /*!< Specifies the constant alpha used for blending.
                                            This parameter must be a number between
                                            Min_Data = 0x00 and Max_Data = 0xFF. */

  uint32_t Alpha0;                     /*!< Configures the default alpha value.
                                            This parameter must be a number between
                                            Min_Data = 0x00 and Max_Data = 0xFF. */

  uint32_t BlendingFactor1;            /*!< Select the blending factor 1.
                                            This parameter can be one of value of @ref LTDC_BlendingFactor1 */

  uint32_t BlendingFactor2;            /*!< Select the blending factor 2.
                                            This parameter can be one of value of @ref LTDC_BlendingFactor2 */

  uint32_t FBStartAdress;              /*!< Configures the color frame buffer address */

  uint32_t ImageWidth;                 /*!< Configures the color frame buffer line length.
                                            This parameter must be a number between
                                            Min_Data = 0x0000 and Max_Data = 0x1FFF. */

  uint32_t ImageHeight;                /*!< Specifies the number of line in frame buffer.
                                            This parameter must be a number between
                                            Min_Data = 0x000 and Max_Data = 0x7FF. */

  LTDC_ColorTypeDef   Backcolor;       /*!< Configures the layer background color. */
} LTDC_LayerCfgTypeDef;




/** @defgroup LTDC_Layer LTDC Layer
  * @{
  */
#define LTDC_LAYER_1                      0x00000000U   /*!< LTDC Layer 1 */
#define LTDC_LAYER_2                      0x00000001U   /*!< LTDC Layer 2 */
/**
  * @}
  */

/** @defgroup LTDC_HS_POLARITY LTDC HS POLARITY
  * @{
  */
#define LTDC_HSPOLARITY_AL                0x00000000U   /*!< Horizontal Synchronization is active low. */
#define LTDC_HSPOLARITY_AH                LTDC_GCR_HSPOL            /*!< Horizontal Synchronization is active high. */
/**
  * @}
  */

/** @defgroup LTDC_VS_POLARITY LTDC VS POLARITY
  * @{
  */
#define LTDC_VSPOLARITY_AL                0x00000000U   /*!< Vertical Synchronization is active low. */
#define LTDC_VSPOLARITY_AH                LTDC_GCR_VSPOL            /*!< Vertical Synchronization is active high. */
/**
  * @}
  */

/** @defgroup LTDC_DE_POLARITY LTDC DE POLARITY
  * @{
  */
#define LTDC_DEPOLARITY_AL                0x00000000U   /*!< Data Enable, is active low. */
#define LTDC_DEPOLARITY_AH                LTDC_GCR_DEPOL            /*!< Data Enable, is active high. */
/**
  * @}
  */

/** @defgroup LTDC_PC_POLARITY LTDC PC POLARITY
  * @{
  */
#define LTDC_PCPOLARITY_IPC               0x00000000U   /*!< input pixel clock. */
#define LTDC_PCPOLARITY_IIPC              LTDC_GCR_PCPOL            /*!< inverted input pixel clock. */
/**
  * @}
  */

/** @defgroup LTDC_SYNC LTDC SYNC
  * @{
  */
#define LTDC_HORIZONTALSYNC               (LTDC_SSCR_HSW >> 16U)    /*!< Horizontal synchronization width. */
#define LTDC_VERTICALSYNC                 LTDC_SSCR_VSH             /*!< Vertical synchronization height. */
/**
  * @}
  */

/** @defgroup LTDC_BACK_COLOR LTDC BACK COLOR
  * @{
  */
#define LTDC_COLOR                        0x000000FFU   /*!< Color mask */
/**
  * @}
  */

/** @defgroup LTDC_BlendingFactor1 LTDC Blending Factor1
  * @{
  */
#define LTDC_BLENDING_FACTOR1_CA          0x00000400U   /*!< Blending factor : Cte Alpha */
#define LTDC_BLENDING_FACTOR1_PAxCA       0x00000600U   /*!< Blending factor : Cte Alpha x Pixel Alpha*/
/**
  * @}
  */

/** @defgroup LTDC_BlendingFactor2 LTDC Blending Factor2
  * @{
  */
#define LTDC_BLENDING_FACTOR2_CA          0x00000005U   /*!< Blending factor : Cte Alpha */
#define LTDC_BLENDING_FACTOR2_PAxCA       0x00000007U   /*!< Blending factor : Cte Alpha x Pixel Alpha*/

#define LTDC_PIXEL_FORMAT_ARGB8888        0x00000000U   /*!< ARGB8888 LTDC pixel format */
#define LTDC_PIXEL_FORMAT_RGB888          0x00000001U   /*!< RGB888 LTDC pixel format   */
#define LTDC_PIXEL_FORMAT_RGB565          0x00000002U   /*!< RGB565 LTDC pixel format   */
#define LTDC_PIXEL_FORMAT_ARGB1555        0x00000003U   /*!< ARGB1555 LTDC pixel format */
#define LTDC_PIXEL_FORMAT_ARGB4444        0x00000004U   /*!< ARGB4444 LTDC pixel format */
#define LTDC_PIXEL_FORMAT_L8              0x00000005U   /*!< L8 LTDC pixel format       */
#define LTDC_PIXEL_FORMAT_AL44            0x00000006U   /*!< AL44 LTDC pixel format     */
#define LTDC_PIXEL_FORMAT_AL88            0x00000007U   /*!< AL88 LTDC pixel format     */





/** @defgroup LTDC_Pixelformat LTDC Pixel format
  * @{
  */
#define LTDC_PIXEL_FORMAT_ARGB8888        0x00000000U   /*!< ARGB8888 LTDC pixel format */
#define LTDC_PIXEL_FORMAT_RGB888          0x00000001U   /*!< RGB888 LTDC pixel format   */
#define LTDC_PIXEL_FORMAT_RGB565          0x00000002U   /*!< RGB565 LTDC pixel format   */
#define LTDC_PIXEL_FORMAT_ARGB1555        0x00000003U   /*!< ARGB1555 LTDC pixel format */
#define LTDC_PIXEL_FORMAT_ARGB4444        0x00000004U   /*!< ARGB4444 LTDC pixel format */
#define LTDC_PIXEL_FORMAT_L8              0x00000005U   /*!< L8 LTDC pixel format       */
#define LTDC_PIXEL_FORMAT_AL44            0x00000006U   /*!< AL44 LTDC pixel format     */
#define LTDC_PIXEL_FORMAT_AL88            0x00000007U   /*!< AL88 LTDC pixel format     */


#define RCC_FLAG_MASK  ((uint8_t)0x1F)

#define ___RCC_GET_FLAG(__FLAG__) (((((((__FLAG__) >> 5U) == 1U)? RCC->CR :((((__FLAG__) >> 5U) == 2U) ? RCC->BDCR : \
((((__FLAG__) >> 5U) == 3U)? RCC->CSR : ((((__FLAG__) >> 5U) == 4U)? RCC->RSR :RCC->CIFR))))  & (1UL << ((__FLAG__) & RCC_FLAG_MASK)))!= 0U)? 1U : 0U)

#define ___RCC_PLL3_CONFIG(__PLL3M__, __PLL3N__, __PLL3P__, __PLL3Q__,__PLL3R__ ) \
                  do{ MODIFY_REG(RCC->PLLCKSELR, ( RCC_PLLCKSELR_DIVM3) , ( (__PLL3M__) <<20U));  \
                         WRITE_REG (RCC->PLL3DIVR , ( (((__PLL3N__) - 1U ) & RCC_PLL3DIVR_N3) | ((((__PLL3P__) -1U ) << 9U) & RCC_PLL3DIVR_P3) | \
                                   ((((__PLL3Q__) -1U) << 16U) & RCC_PLL3DIVR_Q3) | ((((__PLL3R__) - 1U) << 24U) & RCC_PLL3DIVR_R3))); \
                       } while(0)



#define RCC_PLL3VCOWIDE                 (0x00000000U)
#define RCC_PLL3VCOMEDIUM               RCC_PLLCFGR_PLL3VCOSEL

#define DIVIDER_P_UPDATE          0U
#define DIVIDER_Q_UPDATE          1U
#define DIVIDER_R_UPDATE          2U

#define RCC_FLAG_PLL3RDY               ((uint8_t)0x3D)








/*TODO: Macro to verification*/
#define LTDC_LAYER(__LAYER__)              ((LTDC_Layer_TypeDef *)((uint32_t)(\
                                                                               ((uint32_t)(LTDC))\
                                                                               + 0x84U + (0x80U*(__LAYER__)))))

#define IS_LTDC_LAYER(__LAYER__)                       ((__LAYER__) < MAX_LAYER)
#define IS_LTDC_HSPOL(__HSPOL__)                       (((__HSPOL__) == LTDC_HSPOLARITY_AL)\
                                                        || ((__HSPOL__) == LTDC_HSPOLARITY_AH))
#define IS_LTDC_VSPOL(__VSPOL__)                       (((__VSPOL__) == LTDC_VSPOLARITY_AL)\
                                                        || ((__VSPOL__) == LTDC_VSPOLARITY_AH))
#define IS_LTDC_DEPOL(__DEPOL__)                       (((__DEPOL__) == LTDC_DEPOLARITY_AL)\
                                                        || ((__DEPOL__) ==  LTDC_DEPOLARITY_AH))
#define IS_LTDC_PCPOL(__PCPOL__)                       (((__PCPOL__) == LTDC_PCPOLARITY_IPC)\
                                                        || ((__PCPOL__) ==  LTDC_PCPOLARITY_IIPC))
#define IS_LTDC_HSYNC(__HSYNC__)                       ((__HSYNC__)  <= LTDC_HORIZONTALSYNC)
#define IS_LTDC_VSYNC(__VSYNC__)                       ((__VSYNC__)  <= LTDC_VERTICALSYNC)
#define IS_LTDC_AHBP(__AHBP__)                         ((__AHBP__)   <= LTDC_HORIZONTALSYNC)
#define IS_LTDC_AVBP(__AVBP__)                         ((__AVBP__)   <= LTDC_VERTICALSYNC)
#define IS_LTDC_AAW(__AAW__)                           ((__AAW__)    <= LTDC_HORIZONTALSYNC)
#define IS_LTDC_AAH(__AAH__)                           ((__AAH__)    <= LTDC_VERTICALSYNC)
#define IS_LTDC_TOTALW(__TOTALW__)                     ((__TOTALW__) <= LTDC_HORIZONTALSYNC)
#define IS_LTDC_TOTALH(__TOTALH__)                     ((__TOTALH__) <= LTDC_VERTICALSYNC)
#define IS_LTDC_BLUEVALUE(__BBLUE__)                   ((__BBLUE__)  <= LTDC_COLOR)
#define IS_LTDC_GREENVALUE(__BGREEN__)                 ((__BGREEN__) <= LTDC_COLOR)
#define IS_LTDC_REDVALUE(__BRED__)                     ((__BRED__)   <= LTDC_COLOR)
#define IS_LTDC_BLENDING_FACTOR1(__BLENDING_FACTOR1__) (((__BLENDING_FACTOR1__) == LTDC_BLENDING_FACTOR1_CA) || \
                                                        ((__BLENDING_FACTOR1__) == LTDC_BLENDING_FACTOR1_PAxCA))
#define IS_LTDC_BLENDING_FACTOR2(__BLENDING_FACTOR1__) (((__BLENDING_FACTOR1__) == LTDC_BLENDING_FACTOR2_CA) || \
                                                        ((__BLENDING_FACTOR1__) == LTDC_BLENDING_FACTOR2_PAxCA))
#define IS_LTDC_PIXEL_FORMAT(__PIXEL_FORMAT__)         (((__PIXEL_FORMAT__) == LTDC_PIXEL_FORMAT_ARGB8888) || \
                                                        ((__PIXEL_FORMAT__) == LTDC_PIXEL_FORMAT_RGB888)   || \
                                                        ((__PIXEL_FORMAT__) == LTDC_PIXEL_FORMAT_RGB565)   || \
                                                        ((__PIXEL_FORMAT__) == LTDC_PIXEL_FORMAT_ARGB1555) || \
                                                        ((__PIXEL_FORMAT__) == LTDC_PIXEL_FORMAT_ARGB4444) || \
                                                        ((__PIXEL_FORMAT__) == LTDC_PIXEL_FORMAT_L8)       || \
                                                        ((__PIXEL_FORMAT__) == LTDC_PIXEL_FORMAT_AL44)     || \
                                                        ((__PIXEL_FORMAT__) == LTDC_PIXEL_FORMAT_AL88))
#define IS_LTDC_ALPHA(__ALPHA__)                       ((__ALPHA__) <= LTDC_ALPHA)
#define IS_LTDC_HCONFIGST(__HCONFIGST__)               ((__HCONFIGST__) <= LTDC_STARTPOSITION)
#define IS_LTDC_HCONFIGSP(__HCONFIGSP__)               ((__HCONFIGSP__) <= LTDC_STOPPOSITION)
#define IS_LTDC_VCONFIGST(__VCONFIGST__)               ((__VCONFIGST__) <= LTDC_STARTPOSITION)
#define IS_LTDC_VCONFIGSP(__VCONFIGSP__)               ((__VCONFIGSP__) <= LTDC_STOPPOSITION)
#define IS_LTDC_CFBP(__CFBP__)                         ((__CFBP__) <= LTDC_COLOR_FRAME_BUFFER)
#define IS_LTDC_CFBLL(__CFBLL__)                       ((__CFBLL__) <= LTDC_COLOR_FRAME_BUFFER)
#define IS_LTDC_CFBLNBR(__CFBLNBR__)                   ((__CFBLNBR__) <= LTDC_LINE_NUMBER)
#define IS_LTDC_LIPOS(__LIPOS__)                       ((__LIPOS__) <= 0x7FFU)
#define IS_LTDC_RELOAD(__RELOADTYPE__)                 (((__RELOADTYPE__) == LTDC_RELOAD_IMMEDIATE) || \
                                                        ((__RELOADTYPE__) == LTDC_RELOAD_VERTICAL_BLANKING))



#endif /*__LTDC_H_*/