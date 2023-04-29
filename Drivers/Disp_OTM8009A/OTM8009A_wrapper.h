/*
 * DisplayOTM8009A.h
 *
 *  Created on: May 15, 2022
 *      Author: teodor
 */

#ifndef INC_DISPLAYOTM8009A_H_
#define INC_DISPLAYOTM8009A_H_


/* Includes ------------------------------------------------------------------*/

#include "otm8009a.h"

void OTM8009A_DisplayRefreshCommandMode(void* BuffPtr);


#define LCD_LAYER_0_ADDRESS                 0xD0000000U
#define LCD_LAYER_1_ADDRESS                 0xD0200000U

#define LCD_INSTANCES_NBR          1

#define LCD_ORIENTATION_PORTRAIT         0x00U /* Portrait orientation choice of LCD screen               */
#define LCD_ORIENTATION_LANDSCAPE        0x01U /* Landscape orientation choice of LCD screen              */

#define LCD_DEFAULT_WIDTH                800
#define LCD_DEFAULT_HEIGHT               480

#define DISP_LCD_RELOAD_NONE              0U                            /* No reload executed       */
#define DISP_LCD_RELOAD_IMMEDIATE         LTDC_RELOAD_IMMEDIATE         /* Immediate Reload         */
#define DISP_LCD_RELOAD_VERTICAL_BLANKING LTDC_RELOAD_VERTICAL_BLANKING /* Vertical Blanking Reload */
/**
  * @brief LCD special pins
  */
/* LCD reset pin */
#define LCD_RESET_PIN                    GPIO_PIN_3
#define LCD_RESET_PULL                   GPIO_NOPULL
#define LCD_RESET_GPIO_PORT              GPIOG
#define LCD_RESET_GPIO_CLK_ENABLE()      SET_BIT(RCC->AHB4ENR, RCC_AHB4ENR_GPIOGEN)

/* LCD tearing effect pin */
#define LCD_TE_PIN                       GPIO_PIN_2
#define LCD_TE_GPIO_PORT                 GPIOJ
#define LCD_TE_GPIO_CLK_ENABLE()         SET_BIT(RCC->AHB4ENR, RCC_AHB4ENR_GPIOJEN)


/* Back-light control pin */
#define LCD_BL_CTRL_PIN                  GPIO_PIN_12
#define LCD_BL_CTRL_GPIO_PORT            GPIOJ
#define LCD_BL_CTRL_GPIO_CLK_ENABLE()    SET_BIT(RCC->AHB4ENR, RCC_AHB4ENR_GPIOJEN)

/**
  * @}
  */
/**
* @brief  LCD color
*/
/* RGB565 colors definitions */
#define LCD_COLOR_RGB565_BLUE                 0x001FU
#define LCD_COLOR_RGB565_GREEN                0x07E0U
#define LCD_COLOR_RGB565_RED                  0xF800U
#define LCD_COLOR_RGB565_CYAN                 0x07FFU
#define LCD_COLOR_RGB565_MAGENTA              0xF81FU
#define LCD_COLOR_RGB565_YELLOW               0xFFE0U
#define LCD_COLOR_RGB565_LIGHTBLUE            0x841FU
#define LCD_COLOR_RGB565_LIGHTGREEN           0x87F0U
#define LCD_COLOR_RGB565_LIGHTRED             0xFC10U
#define LCD_COLOR_RGB565_LIGHTCYAN            0x87FFU
#define LCD_COLOR_RGB565_LIGHTMAGENTA         0xFC1FU
#define LCD_COLOR_RGB565_LIGHTYELLOW          0xFFF0U
#define LCD_COLOR_RGB565_DARKBLUE             0x0010U
#define LCD_COLOR_RGB565_DARKGREEN            0x0400U
#define LCD_COLOR_RGB565_DARKRED              0x8000U
#define LCD_COLOR_RGB565_DARKCYAN             0x0410U
#define LCD_COLOR_RGB565_DARKMAGENTA          0x8010U
#define LCD_COLOR_RGB565_DARKYELLOW           0x8400U
#define LCD_COLOR_RGB565_WHITE                0xFFFFU
#define LCD_COLOR_RGB565_LIGHTGRAY            0xD69AU
#define LCD_COLOR_RGB565_GRAY                 0x8410U
#define LCD_COLOR_RGB565_DARKGRAY             0x4208U
#define LCD_COLOR_RGB565_BLACK                0x0000U
#define LCD_COLOR_RGB565_BROWN                0xA145U
#define LCD_COLOR_RGB565_ORANGE               0xFD20U
/* Definition of Official ST COLOR */
#define LCD_COLOR_RGB565_ST_BLUE_DARK         0x0001U
#define LCD_COLOR_RGB565_ST_BLUE              0x01EBU
#define LCD_COLOR_RGB565_ST_BLUE_LIGHT        0x06A7U
#define LCD_COLOR_RGB565_ST_GREEN_LIGHT       0x05ECU
#define LCD_COLOR_RGB565_ST_GREEN_DARK        0x001CU
#define LCD_COLOR_RGB565_ST_YELLOW            0x07F0U
#define LCD_COLOR_RGB565_ST_BROWN             0x02C8U
#define LCD_COLOR_RGB565_ST_PINK              0x0681U
#define LCD_COLOR_RGB565_ST_PURPLE            0x02CDU
#define LCD_COLOR_RGB565_ST_GRAY_DARK         0x0251U
#define LCD_COLOR_RGB565_ST_GRAY              0x04BAU
#define LCD_COLOR_RGB565_ST_GRAY_LIGHT        0x05E7U

/* ARGB8888 colors definitions */
#define LCD_COLOR_ARGB8888_BLUE               0xFF0000FFUL
#define LCD_COLOR_ARGB8888_GREEN              0xFF00FF00UL
#define LCD_COLOR_ARGB8888_RED                0xFFFF0000UL
#define LCD_COLOR_ARGB8888_CYAN               0xFF00FFFFUL
#define LCD_COLOR_ARGB8888_MAGENTA            0xFFFF00FFUL
#define LCD_COLOR_ARGB8888_YELLOW             0xFFFFFF00UL
#define LCD_COLOR_ARGB8888_LIGHTBLUE          0xFF8080FFUL
#define LCD_COLOR_ARGB8888_LIGHTGREEN         0xFF80FF80UL
#define LCD_COLOR_ARGB8888_LIGHTRED           0xFFFF8080UL
#define LCD_COLOR_ARGB8888_LIGHTCYAN          0xFF80FFFFUL
#define LCD_COLOR_ARGB8888_LIGHTMAGENTA       0xFFFF80FFUL
#define LCD_COLOR_ARGB8888_LIGHTYELLOW        0xFFFFFF80UL
#define LCD_COLOR_ARGB8888_DARKBLUE           0xFF000080UL
#define LCD_COLOR_ARGB8888_DARKGREEN          0xFF008000UL
#define LCD_COLOR_ARGB8888_DARKRED            0xFF800000UL
#define LCD_COLOR_ARGB8888_DARKCYAN           0xFF008080UL
#define LCD_COLOR_ARGB8888_DARKMAGENTA        0xFF800080UL
#define LCD_COLOR_ARGB8888_DARKYELLOW         0xFF808000UL
#define LCD_COLOR_ARGB8888_WHITE              0xFFFFFFFFUL
#define LCD_COLOR_ARGB8888_LIGHTGRAY          0xFFD3D3D3UL
#define LCD_COLOR_ARGB8888_GRAY               0xFF808080UL
#define LCD_COLOR_ARGB8888_DARKGRAY           0xFF404040UL
#define LCD_COLOR_ARGB8888_BLACK              0xFF000000UL
#define LCD_COLOR_ARGB8888_BROWN              0xFFA52A2AUL
#define LCD_COLOR_ARGB8888_ORANGE             0xFFFFA500UL
/* Definition of Official ST Colors */
#define LCD_COLOR_ARGB8888_ST_BLUE_DARK       0xFF002052UL
#define LCD_COLOR_ARGB8888_ST_BLUE            0xFF39A9DCUL
#define LCD_COLOR_ARGB8888_ST_BLUE_LIGHT      0xFFD1E4F3UL
#define LCD_COLOR_ARGB8888_ST_GREEN_LIGHT     0xFFBBCC01UL
#define LCD_COLOR_ARGB8888_ST_GREEN_DARK      0xFF003D14UL
#define LCD_COLOR_ARGB8888_ST_YELLOW          0xFFFFD300UL
#define LCD_COLOR_ARGB8888_ST_BROWN           0xFF5C0915UL
#define LCD_COLOR_ARGB8888_ST_PINK            0xFFD4007AUL
#define LCD_COLOR_ARGB8888_ST_PURPLE          0xFF590D58UL
#define LCD_COLOR_ARGB8888_ST_GRAY_DARK       0xFF4F5251UL
#define LCD_COLOR_ARGB8888_ST_GRAY            0xFF90989EUL
#define LCD_COLOR_ARGB8888_ST_GRAY_LIGHT      0xFFB9C4CAUL



#define LCD_PIXEL_FORMAT_ARGB8888        0x00000000U   /*!< ARGB8888 LTDC pixel format */
#define LCD_PIXEL_FORMAT_RGB888          0x00000001U   /*!< RGB888 LTDC pixel format   */
#define LCD_PIXEL_FORMAT_RGB565          0x00000002U   /*!< RGB565 LTDC pixel format   */
#define LCD_PIXEL_FORMAT_ARGB1555        0x00000003U   /*!< ARGB1555 LTDC pixel format */
#define LCD_PIXEL_FORMAT_ARGB4444        0x00000004U   /*!< ARGB4444 LTDC pixel format */
#define LCD_PIXEL_FORMAT_L8              0x00000005U   /*!< L8 LTDC pixel format       */
#define LCD_PIXEL_FORMAT_AL44            0x00000006U   /*!< AL44 LTDC pixel format     */
#define LCD_PIXEL_FORMAT_AL88            0x00000007U   /*!< AL88 LTDC pixel format     */




typedef uint32_t(*ConvertColor_Func)(uint32_t);

typedef struct
{
  uint32_t XSize;
  uint32_t YSize;
  uint32_t ActiveLayer;
  uint32_t PixelFormat;
  uint32_t BppFactor;
  uint32_t IsMspCallbacksValid;
  uint32_t ReloadEnable;
} DISP_LCD_Ctx_t;

typedef struct
{
  uint32_t X0;
  uint32_t X1;
  uint32_t Y0;
  uint32_t Y1;
  uint32_t PixelFormat;
  uint32_t Address;
}MX_LTDC_LayerConfig_t;


extern DISP_LCD_Ctx_t       Lcd_Ctx[];
extern void               *Lcd_CompObj;
extern OTM8009A_Object_t   OTM8009AObj;


/* Initialization APIs */
void OTM8009A_DISP_LCD_Init(uint32_t Instance, uint32_t Orientation);

void DISP_LCD_LL_FlushBufferDMA2D(
                  uint32_t Instance,
									uint32_t Xpos,
									uint32_t Ypos,
									uint32_t Width,
									uint32_t Height,
									uint32_t *Colormap,
                  void(*DMAtrEndCb)(void)
									);




#endif /* INC_DISPLAYOTM8009A_H_ */
