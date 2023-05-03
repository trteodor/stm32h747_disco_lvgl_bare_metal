// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.0
// LVGL version: 8.3.3
// Project name: SquareLine_Project

#include "../ui.h"

void ui_Screen2_screen_init(void)
{
    ui_Screen2 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen2, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Keyboard1 = lv_keyboard_create(ui_Screen2);
    lv_obj_set_width(ui_Keyboard1, 531);
    lv_obj_set_height(ui_Keyboard1, 236);
    lv_obj_set_x(ui_Keyboard1, -24);
    lv_obj_set_y(ui_Keyboard1, 47);
    lv_obj_set_align(ui_Keyboard1, LV_ALIGN_CENTER);

    ui_TextArea1 = lv_textarea_create(ui_Screen2);
    lv_obj_set_width(ui_TextArea1, 475);
    lv_obj_set_height(ui_TextArea1, 102);
    lv_obj_set_x(ui_TextArea1, 102);
    lv_obj_set_y(ui_TextArea1, -151);
    lv_obj_set_align(ui_TextArea1, LV_ALIGN_CENTER);
    lv_textarea_set_placeholder_text(ui_TextArea1, "Placeholder...");

    ui_Button4 = lv_btn_create(ui_Screen2);
    lv_obj_set_width(ui_Button4, 100);
    lv_obj_set_height(ui_Button4, 50);
    lv_obj_set_x(ui_Button4, -280);
    lv_obj_set_y(ui_Button4, -147);
    lv_obj_set_align(ui_Button4, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Button4, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Button4, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    lv_keyboard_set_textarea(ui_Keyboard1, ui_TextArea1);
    lv_obj_add_event_cb(ui_Button4, ui_event_Button4, LV_EVENT_ALL, NULL);

}