/*
 * lvglAppMain.c
 *
 *  Created on: May 17, 2022
 *      Author: teodor
 */

#include "OTM8009A_wrapper.h"
#include "TouchC_ft6x06.h"
#include "System.h"
#include "stm32h7xx.h"
#include "lvgl/lvgl.h"
#include "ui.h"


static lv_disp_draw_buf_t disp_buf;

#define DISCOH747_DISP_WIDTH 800
#define DISCOH747_DISP_HIGH 480
#define BufferDivider 1


#define SDRAM_DEVICE_ADDR         0xD0000000U


static volatile lv_color_t *buffer_1 = (lv_color_t*)SDRAM_DEVICE_ADDR;
static volatile lv_color_t *buffer_2 = (lv_color_t*)(SDRAM_DEVICE_ADDR + (0x240000));

static lv_disp_drv_t disp_drv;
static lv_indev_drv_t indev_drv;


/*Drivers and all necessary peripherials are initialized in main.c file*/

static lv_disp_drv_t *LastDriver;

void OTM8009_flush(lv_disp_drv_t * drv, const lv_area_t * area,  lv_color_t * color_map)
{
	OTM8009A_DisplayRefreshCommandMode((void *)color_map);
	/*I dont need to pass area because the flag is activated: disp_drv.full_refresh = 1;*/
	LastDriver = drv;
	lv_disp_flush_ready(LastDriver);
}


void TouchCntrlFt6x06_Read(lv_indev_drv_t * drv, lv_indev_data_t*data)
{
	static TouchStateFt6x06_t PreviousTouchState;

	// if(BSP_TS_GetIT_State_OTM8009a() == Touch_IRQ_FlagSet_ft6x06 || PreviousTouchState == Touch_Touched_ft6x06 )
	{
		int16_t readX,readY;

		if(TS_GetTouchPointAndState(&readX,  &readY) == Touch_Touched_ft6x06)
		{
			 data->state = LV_INDEV_STATE_PRESSED;
			 data->point.x = readX;
			 data->point.y = readY;
			 PreviousTouchState = Touch_Touched_ft6x06;
		}
		else
		{
			 data->state = LV_INDEV_STATE_RELEASED;
			 data->point.x = readX;
			 data->point.y = readY;
			 PreviousTouchState = Touch_Released_ft6x06;
		}
	}
	// else
	// {
	//     data->state = LV_INDEV_STATE_RELEASED;
	// }

}

void LvglInitApp(void)
{
	lv_init();
	lv_disp_draw_buf_init(&disp_buf, (void *)buffer_1, (void *)(buffer_2), ((DISCOH747_DISP_WIDTH * DISCOH747_DISP_HIGH)));
	lv_disp_drv_init(&disp_drv);            /*Basic initialization*/
	disp_drv.draw_buf = &disp_buf;          /*Set an initialized buffer*/
	disp_drv.flush_cb = OTM8009_flush;        /*Set a flush callback to draw to the display*/
	disp_drv.hor_res = LCD_DEFAULT_WIDTH;                 /*Set the horizontal resolution in pixels*/ //800
	disp_drv.ver_res = LCD_DEFAULT_HEIGHT;                 /*Set the vertical resolution in pixels*/ //480
	disp_drv.full_refresh = 1;
	lv_disp_drv_register(&disp_drv); /*Register the driver and save the created display objects*/


	lv_indev_drv_init(&indev_drv);
	indev_drv.type =LV_INDEV_TYPE_POINTER;
	indev_drv.read_cb = TouchCntrlFt6x06_Read;
	lv_indev_drv_register(&indev_drv);

    //  lv_demo_stress();
	//  lv_demo_benchmark();
      ui_init();
    //  lv_example_get_started_3();
}




/*Called from main.c with period 5ms*/
void LvglProcesTask(void)
{
    lv_task_handler();
    lv_tick_inc(5);

}
