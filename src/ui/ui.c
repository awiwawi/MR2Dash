#include "ui.h"
#include <stdio.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// --- Widgets ---
static lv_obj_t * label_rpm_digit;
static lv_obj_t * label_speed;

// Sensor Objects
static lv_obj_t * arc_boost;
static lv_obj_t * label_boost_val;
static lv_obj_t * label_egt_val;
static lv_obj_t * label_iat_val;

static lv_obj_t * arc_oilp;
static lv_obj_t * label_oilp_val;
static lv_obj_t * label_oilt_val;
static lv_obj_t * label_clt_val;

// --- Helpers ---
static lv_obj_t * create_data_label(lv_obj_t * parent, const char * title, int x, int y) {
    lv_obj_t * val = lv_label_create(parent);
    lv_obj_align(val, LV_ALIGN_CENTER, x, y);
    lv_obj_set_style_text_font(val, &lv_font_montserrat_32, 0);
    lv_obj_set_style_text_color(val, lv_color_hex(0xFFFFFF), 0);
    lv_label_set_text(val, "0");

    lv_obj_t * lbl = lv_label_create(parent);
    lv_obj_align_to(lbl, val, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(lbl, lv_color_hex(0x888888), 0);
    lv_label_set_text(lbl, title);

    return val;
}

void ui_init(void) {
    lv_obj_t * scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x000000), LV_PART_MAIN);

    // --- 1. Center Stack ---
    label_rpm_digit = lv_label_create(scr);
    lv_obj_align(label_rpm_digit, LV_ALIGN_CENTER, 0, -80);
    lv_obj_set_style_text_color(label_rpm_digit, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(label_rpm_digit, &lv_font_montserrat_48, 0); 
    lv_label_set_text(label_rpm_digit, "0");

    lv_obj_t * lbl_rpm = lv_label_create(scr);
    lv_obj_align_to(lbl_rpm, label_rpm_digit, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_obj_set_style_text_color(lbl_rpm, lv_color_hex(0x666666), 0);
    lv_label_set_text(lbl_rpm, "RPM");

    label_speed = lv_label_create(scr);
    lv_obj_align(label_speed, LV_ALIGN_CENTER, 0, 40);
    lv_obj_set_style_text_color(label_speed, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(label_speed, &lv_font_montserrat_48, 0);
    lv_label_set_text(label_speed, "0");
    
    lv_obj_t * lbl_kmh = lv_label_create(scr);
    lv_obj_align_to(lbl_kmh, label_speed, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_obj_set_style_text_color(lbl_kmh, lv_color_hex(0x666666), 0);
    lv_label_set_text(lbl_kmh, "km/h");

    // --- 2. Side Arcs ---
    // Boost (Left)
    arc_boost = lv_arc_create(scr);
    lv_obj_set_size(arc_boost, 700, 700); 
    lv_obj_align(arc_boost, LV_ALIGN_CENTER, 0, 0);
    lv_arc_set_bg_angles(arc_boost, 140, 220);
    lv_arc_set_rotation(arc_boost, 0);
    lv_obj_set_style_arc_width(arc_boost, 25, LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc_boost, lv_color_hex(0x111111), LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc_boost, 25, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc_boost, lv_color_hex(0x00A8FF), LV_PART_INDICATOR); 
    lv_obj_remove_style(arc_boost, NULL, LV_PART_KNOB);

    label_boost_val = create_data_label(scr, "BOOST", -220, 0);

    // Oil (Right)
    arc_oilp = lv_arc_create(scr);
    lv_obj_set_size(arc_oilp, 700, 700);
    lv_obj_align(arc_oilp, LV_ALIGN_CENTER, 0, 0);
    lv_arc_set_bg_angles(arc_oilp, 320, 40); 
    lv_arc_set_rotation(arc_oilp, 0);
    lv_obj_set_style_arc_width(arc_oilp, 25, LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc_oilp, lv_color_hex(0x111111), LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc_oilp, 25, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc_oilp, lv_color_hex(0xFFA800), LV_PART_INDICATOR);
    lv_obj_remove_style(arc_oilp, NULL, LV_PART_KNOB);

    label_oilp_val = create_data_label(scr, "OIL P", 220, 0);

    // --- 3. Secondary Data ---
    label_egt_val = create_data_label(scr, "EGT", -140, 120);
    label_iat_val = create_data_label(scr, "IAT", -140, -120);
    label_oilt_val = create_data_label(scr, "OIL T", 140, -120);
    label_clt_val = create_data_label(scr, "CLT", 140, 120);
}

void ui_update_data(int rpm, int speed, float boost, int oil_press, int coolant_temp, int oil_temp, int egt, int iat) {
    if (label_rpm_digit) lv_label_set_text_fmt(label_rpm_digit, "%d", rpm);
    if (label_speed) lv_label_set_text_fmt(label_speed, "%d", speed);

    if (arc_boost) {
        float boost_norm = (boost + 1.0f) / 3.0f; 
        if(boost_norm < 0) boost_norm = 0;
        if(boost_norm > 1) boost_norm = 1;
        
        // Grow from 140 (Bottom) Clockwise to 220 (Top)
        int end_angle = 140 + (int)(80 * boost_norm);
        lv_arc_set_angles(arc_boost, 140, end_angle);
        
        lv_label_set_text_fmt(label_boost_val, "%.1f", boost);
    }

    if (arc_oilp) {
        float oil_norm = (float)oil_press / 10.0f;
        if(oil_norm > 1) oil_norm = 1;
        if(oil_norm < 0) oil_norm = 0;
        
        // Grow from 40 (Bottom) Counter-Clockwise to 320 (Top)
        // In LVGL CW terms: Indicator is moving_start to 40 (fixed end)
        int start_angle = 40 - (int)(80 * oil_norm);
        if (start_angle < 0) start_angle += 360;
        lv_arc_set_angles(arc_oilp, start_angle, 40);
        
        lv_label_set_text_fmt(label_oilp_val, "%d.0", oil_press);
    }

    if (label_egt_val) lv_label_set_text_fmt(label_egt_val, "%d", egt);
    if (label_iat_val) lv_label_set_text_fmt(label_iat_val, "%d", iat);
    if (label_oilt_val) lv_label_set_text_fmt(label_oilt_val, "%d", oil_temp);
    if (label_clt_val) lv_label_set_text_fmt(label_clt_val, "%d", coolant_temp);

    if (coolant_temp > 105) lv_obj_set_style_text_color(label_clt_val, lv_color_hex(0xFF0000), 0);
    else lv_obj_set_style_text_color(label_clt_val, lv_color_hex(0xFFFFFF), 0);
}