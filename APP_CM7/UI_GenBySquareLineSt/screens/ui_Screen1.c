// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.0
// LVGL version: 8.3.3
// Project name: SquareLine_Project

#include "../ui.h"

void ui_Screen1_screen_init(void)
{
    ui_Screen1 = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(ui_Screen1, lv_color_hex(0xFF00FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Screen1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Arc1 = lv_arc_create(ui_Screen1);
    lv_obj_set_width(ui_Arc1, 150);
    lv_obj_set_height(ui_Arc1, 150);
    lv_obj_set_x(ui_Arc1, 81);
    lv_obj_set_y(ui_Arc1, -226);
    lv_obj_set_align(ui_Arc1, LV_ALIGN_CENTER);

    ui_Button2 = lv_btn_create(ui_Screen1);
    lv_obj_set_width(ui_Button2, 100);
    lv_obj_set_height(ui_Button2, 50);
    lv_obj_set_x(ui_Button2, -287);
    lv_obj_set_y(ui_Button2, 323);
    lv_obj_set_align(ui_Button2, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Button2, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Button2, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Image2 = lv_img_create(ui_Screen1);
    lv_obj_set_width(ui_Image2, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Image2, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Image2, -337);
    lv_obj_set_y(ui_Image2, -26);
    lv_obj_set_align(ui_Image2, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Image2, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_Image2, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Label2 = lv_label_create(ui_Screen1);
    lv_obj_set_width(ui_Label2, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label2, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label2, -305);
    lv_obj_set_y(ui_Label2, 67);
    lv_obj_set_align(ui_Label2, LV_ALIGN_CENTER);

    ui_Keyboard2 = lv_keyboard_create(ui_Screen1);
    lv_obj_set_width(ui_Keyboard2, 411);
    lv_obj_set_height(ui_Keyboard2, 240);
    lv_obj_set_x(ui_Keyboard2, 162);
    lv_obj_set_y(ui_Keyboard2, 113);
    lv_obj_set_align(ui_Keyboard2, LV_ALIGN_CENTER);

    ui_Chart2 = lv_chart_create(ui_Screen1);
    lv_obj_set_width(ui_Chart2, 392);
    lv_obj_set_height(ui_Chart2, 192);
    lv_obj_set_x(ui_Chart2, 215);
    lv_obj_set_y(ui_Chart2, -109);
    lv_obj_set_align(ui_Chart2, LV_ALIGN_CENTER);
    lv_chart_set_type(ui_Chart2, LV_CHART_TYPE_LINE);
    lv_chart_set_axis_tick(ui_Chart2, LV_CHART_AXIS_PRIMARY_X, 10, 5, 5, 2, true, 50);
    lv_chart_set_axis_tick(ui_Chart2, LV_CHART_AXIS_PRIMARY_Y, 10, 5, 5, 2, true, 50);
    lv_chart_set_axis_tick(ui_Chart2, LV_CHART_AXIS_SECONDARY_Y, 10, 5, 5, 2, true, 25);
    lv_chart_series_t * ui_Chart2_series_1 = lv_chart_add_series(ui_Chart2, lv_color_hex(0x808080),
                                                                 LV_CHART_AXIS_PRIMARY_Y);
    static lv_coord_t ui_Chart2_series_1_array[] = { 0, 10, 20, 40, 80, 80, 40, 20, 10, 0 };
    lv_chart_set_ext_y_array(ui_Chart2, ui_Chart2_series_1, ui_Chart2_series_1_array);

    ui_Roller2 = lv_roller_create(ui_Screen1);
    lv_roller_set_options(ui_Roller2, "Option 1\nOption 2\nOption 3", LV_ROLLER_MODE_NORMAL);
    lv_obj_set_height(ui_Roller2, 100);
    lv_obj_set_width(ui_Roller2, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_x(ui_Roller2, -241);
    lv_obj_set_y(ui_Roller2, -141);
    lv_obj_set_align(ui_Roller2, LV_ALIGN_CENTER);

    ui_Slider2 = lv_slider_create(ui_Screen1);
    lv_obj_set_width(ui_Slider2, 150);
    lv_obj_set_height(ui_Slider2, 10);
    lv_obj_set_x(ui_Slider2, -273);
    lv_obj_set_y(ui_Slider2, 161);
    lv_obj_set_align(ui_Slider2, LV_ALIGN_CENTER);

    ui_Switch2 = lv_switch_create(ui_Screen1);
    lv_obj_set_width(ui_Switch2, 50);
    lv_obj_set_height(ui_Switch2, 25);
    lv_obj_set_x(ui_Switch2, -265);
    lv_obj_set_y(ui_Switch2, 15);
    lv_obj_set_align(ui_Switch2, LV_ALIGN_CENTER);

    ui_Colorwheel2 = lv_colorwheel_create(ui_Screen1, true);
    lv_obj_set_width(ui_Colorwheel2, 150);
    lv_obj_set_height(ui_Colorwheel2, 150);
    lv_obj_set_x(ui_Colorwheel2, -143);
    lv_obj_set_y(ui_Colorwheel2, 2);
    lv_obj_set_align(ui_Colorwheel2, LV_ALIGN_CENTER);

    ui_TextArea2 = lv_textarea_create(ui_Screen1);
    lv_obj_set_width(ui_TextArea2, 150);
    lv_obj_set_height(ui_TextArea2, 70);
    lv_obj_set_x(ui_TextArea2, -88);
    lv_obj_set_y(ui_TextArea2, -162);
    lv_obj_set_align(ui_TextArea2, LV_ALIGN_CENTER);
    lv_textarea_set_placeholder_text(ui_TextArea2, "Placeholder...");

    ui_Button1 = lv_btn_create(ui_Screen1);
    lv_obj_set_width(ui_Button1, 100);
    lv_obj_set_height(ui_Button1, 50);
    lv_obj_set_x(ui_Button1, -119);
    lv_obj_set_y(ui_Button1, 172);
    lv_obj_set_align(ui_Button1, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Button1, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Button1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    lv_keyboard_set_textarea(ui_Keyboard2, ui_TextArea2);
    lv_obj_add_event_cb(ui_Button1, ui_event_Button1, LV_EVENT_ALL, NULL);

}
