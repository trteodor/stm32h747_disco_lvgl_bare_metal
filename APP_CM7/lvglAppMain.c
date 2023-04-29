/*
 * lvglAppMain.c
 *
 *  Created on: May 17, 2022
 *      Author: teodor
 */

#include "lvgl.h"
#include "lv_examples.h"
#include "OTM8009A_wrapper.h"

static lv_disp_draw_buf_t disp_buf;

#define DISCOH747_DISP_WIDTH 800
#define DISCOH747_DISP_HIGH 480
#define BufferDivider 1

void BuffTransmitCpltCb(void);

#define SDRAM_DEVICE_ADDR         0xD0000000U


static volatile lv_color_t *buf_1 = (lv_color_t*)SDRAM_DEVICE_ADDR + (2*0x240000);
static volatile lv_color_t *buf_2 = (lv_color_t*)SDRAM_DEVICE_ADDR + (3*0x240000);
//static lv_color_t buf_1[ (DISCOH747_DISP_WIDTH * DISCOH747_DISP_HIGH) /BufferDivider ] ;
//static lv_color_t buf_2[ (DISCOH747_DISP_WIDTH * DISCOH747_DISP_HIGH) /BufferDivider ] ;

static lv_disp_drv_t disp_drv;
static lv_indev_drv_t indev_drv;


/*Drivers and all necessary peripherials are initialized in main.c file*/

static lv_disp_drv_t *LastDriver;

void OTM8009_flush(lv_disp_drv_t * drv, const lv_area_t * area,  lv_color_t * color_map)
{
	LastDriver = drv;

		LastDriver = drv;

		DISP_LCD_LL_FlushBufferDMA2D(0,
									area->x1,
									area->y1,
									area->x2 - area->x1 +1,
									area->y2 - area->y1 +1,
									(uint32_t *)color_map,
									BuffTransmitCpltCb);

//	LTDC_Layer1->CFBAR = (uint32_t)color_map;
//	LTDC->SRCR = LTDC_SRCR_IMR;
//	lv_disp_flush_ready(drv);
}

void BuffTransmitCpltCb(void)
{
	lv_disp_flush_ready(LastDriver);
}


void LvglInitApp(void)
{
	  lv_init();
	  lv_disp_draw_buf_init(&disp_buf, buf_1, buf_2, (4* DISCOH747_DISP_WIDTH * DISCOH747_DISP_HIGH)/ BufferDivider);
	  lv_disp_drv_init(&disp_drv);            /*Basic initialization*/
	  disp_drv.draw_buf = &disp_buf;          /*Set an initialized buffer*/
	  disp_drv.flush_cb = OTM8009_flush;        /*Set a flush callback to draw to the display*/
	  disp_drv.hor_res = LCD_DEFAULT_WIDTH;                 /*Set the horizontal resolution in pixels*/ //800
	  disp_drv.ver_res = LCD_DEFAULT_HEIGHT;                 /*Set the vertical resolution in pixels*/ //480
      lv_disp_drv_register(&disp_drv); /*Register the driver and save the created display objects*/


     lv_example_get_started_1();
    //   ui_init();
//      lv_example_chart_5();
}




/*Called from main.c with period 5ms*/
void LvglProcesTask(void)
{
    lv_task_handler();
    lv_tick_inc(5);
}
