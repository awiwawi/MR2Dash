#include "ui.h"
#include <stdio.h>
#include <math.h>

// --- Configuration ---
#define COLOR_TEAL      lv_color_hex(0x008080)
#define COLOR_BURGUNDY  lv_color_hex(0x800020)
#define COLOR_SCREEN_BG lv_color_hex(0x222222) 
#define COLOR_BOX_BG    lv_color_hex(0x000000) 
#define COLOR_TEXT      lv_color_hex(0xFFFFFF)

// --- Custom Fonts ---
LV_FONT_DECLARE(carbon_100);
LV_FONT_DECLARE(carbon_80);
LV_FONT_DECLARE(carbon_42);
LV_FONT_DECLARE(carbon_20);

// --- Widgets ---
static lv_obj_t * label_rpm_digit;
static lv_obj_t * label_speed;

static lv_obj_t * arc_boost;
static lv_obj_t * label_boost_val;
static lv_obj_t * container_egt;
static lv_obj_t * label_egt_val;
static lv_obj_t * container_iat;
static lv_obj_t * label_iat_val;

static lv_obj_t * arc_oilp;
static lv_obj_t * label_oilp_val;
static lv_obj_t * container_oilt;
static lv_obj_t * label_oilt_val;
static lv_obj_t * container_clt;
static lv_obj_t * label_clt_val;

// --- Helpers ---

static lv_obj_t * create_stat_box(lv_obj_t * parent, const char * title, int x, int y, lv_obj_t ** out_val_label) {
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont, 180, 110);
    lv_obj_align(cont, LV_ALIGN_CENTER, x, y);
    lv_obj_set_style_bg_color(cont, COLOR_BOX_BG, 0);
    lv_obj_set_style_radius(cont, 8, 0); 
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);

    *out_val_label = lv_label_create(cont);
    lv_obj_align(*out_val_label, LV_ALIGN_CENTER, 0, -10);
    lv_obj_set_style_text_font(*out_val_label, &carbon_42, 0); 
    lv_obj_set_style_text_color(*out_val_label, COLOR_TEXT, 0);
    lv_label_set_text(*out_val_label, "0");

    lv_obj_t * lbl = lv_label_create(cont);
    lv_obj_align(lbl, LV_ALIGN_BOTTOM_MID, 0, -5);
    lv_obj_set_style_text_font(lbl, &carbon_20, 0); 
    lv_obj_set_style_text_color(lbl, lv_color_hex(0xAAAAAA), 0);
    lv_label_set_text(lbl, title);

    return cont;
}

void ui_init(void) {
    lv_obj_t * scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, COLOR_SCREEN_BG, LV_PART_MAIN);

    // --- 1. Center Stack ---
    label_rpm_digit = lv_label_create(scr);
    lv_obj_align(label_rpm_digit, LV_ALIGN_CENTER, 0, -220); 
    lv_obj_set_style_text_color(label_rpm_digit, COLOR_TEXT, 0);
    lv_obj_set_style_text_font(label_rpm_digit, &carbon_100, 0); 
    lv_label_set_text(label_rpm_digit, "0");

    lv_obj_t * lbl_rpm = lv_label_create(scr);
    lv_obj_align_to(lbl_rpm, label_rpm_digit, LV_ALIGN_OUT_BOTTOM_MID, 0, -15);
    lv_obj_set_style_text_color(lbl_rpm, lv_color_hex(0x666666), 0);
    lv_obj_set_style_text_font(lbl_rpm, &carbon_20, 0);
    lv_label_set_text(lbl_rpm, "RPM");

    label_speed = lv_label_create(scr);
    lv_obj_align(label_speed, LV_ALIGN_CENTER, 0, 220); 
    lv_obj_set_style_text_color(label_speed, COLOR_TEXT, 0);
    lv_obj_set_style_text_font(label_speed, &carbon_80, 0); 
    lv_label_set_text(label_speed, "0");
    
    lv_obj_t * lbl_kmh = lv_label_create(scr);
    lv_obj_align_to(lbl_kmh, label_speed, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_obj_set_style_text_color(lbl_kmh, lv_color_hex(0x666666), 0);
    lv_obj_set_style_text_font(lbl_kmh, &carbon_20, 0);
    lv_label_set_text(lbl_kmh, "km/h");

    // --- 2. Side Arcs ---
    
    // Boost (Left)
    arc_boost = lv_arc_create(scr);
    lv_obj_set_size(arc_boost, 700, 700); 
    lv_obj_align(arc_boost, LV_ALIGN_CENTER, 0, 0);
    lv_arc_set_bg_angles(arc_boost, 130, 230);
    lv_arc_set_rotation(arc_boost, 0);
    lv_obj_set_style_arc_width(arc_boost, 30, LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc_boost, lv_color_hex(0x111111), LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(arc_boost, false, LV_PART_MAIN); 
    lv_obj_set_style_arc_width(arc_boost, 30, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc_boost, COLOR_TEAL, LV_PART_INDICATOR); 
    lv_obj_set_style_arc_rounded(arc_boost, false, LV_PART_INDICATOR); 
    lv_obj_remove_style(arc_boost, NULL, LV_PART_KNOB);

    lv_obj_t * lbl_boost = lv_label_create(scr);
    lv_obj_align(lbl_boost, LV_ALIGN_CENTER, -270, 0); 
    lv_label_set_text(lbl_boost, "BST");
    lv_obj_set_style_text_font(lbl_boost, &carbon_20, 0);
    lv_obj_set_style_text_color(lbl_boost, lv_color_hex(0x888888), 0);
    
    label_boost_val = lv_label_create(scr);
    lv_obj_align_to(label_boost_val, lbl_boost, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_obj_set_style_text_font(label_boost_val, &carbon_42, 0);
    lv_obj_set_style_text_color(label_boost_val, COLOR_TEXT, 0);
    lv_label_set_text(label_boost_val, "0.0");


    // Oil Pressure (Right)
    arc_oilp = lv_arc_create(scr);
    lv_obj_set_size(arc_oilp, 700, 700);
    lv_obj_align(arc_oilp, LV_ALIGN_CENTER, 0, 0);
    lv_arc_set_bg_angles(arc_oilp, 310, 50); 
    lv_arc_set_rotation(arc_oilp, 0);
    lv_obj_set_style_arc_width(arc_oilp, 30, LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc_oilp, lv_color_hex(0x111111), LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(arc_oilp, false, LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc_oilp, 30, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc_oilp, COLOR_TEAL, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(arc_oilp, false, LV_PART_INDICATOR);
    lv_obj_remove_style(arc_oilp, NULL, LV_PART_KNOB);

    lv_obj_t * lbl_oil = lv_label_create(scr);
    lv_obj_align(lbl_oil, LV_ALIGN_CENTER, 250, 0); 
    lv_label_set_text(lbl_oil, "OIL");
    lv_obj_set_style_text_font(lbl_oil, &carbon_20, 0);
    lv_obj_set_style_text_color(lbl_oil, lv_color_hex(0x888888), 0);

    label_oilp_val = lv_label_create(scr);
    lv_obj_align_to(label_oilp_val, lbl_oil, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_obj_set_style_text_font(label_oilp_val, &carbon_42, 0);
    lv_obj_set_style_text_color(label_oilp_val, COLOR_TEXT, 0);
    lv_label_set_text(label_oilp_val, "0.0");

    // --- 3. Central Grid Stats ---
    container_egt = create_stat_box(scr, "EGT", -92, -57, &label_egt_val);
    container_oilt = create_stat_box(scr, "OIL T", 92, -57, &label_oilt_val);
    container_iat = create_stat_box(scr, "IAT", -92, 57, &label_iat_val);
    container_clt = create_stat_box(scr, "CLT", 92, 57, &label_clt_val);
}

void ui_update_data(int rpm, int speed, float boost, float oil_press, int coolant_temp, int oil_temp, int egt, int iat) {
    if (label_rpm_digit) lv_label_set_text_fmt(label_rpm_digit, "%d", rpm);
    if (label_speed) lv_label_set_text_fmt(label_speed, "%d", speed);

    if (arc_boost) {
        float boost_norm = (boost + 1.0f) / 3.0f; 
        if(boost_norm < 0) boost_norm = 0;
        if(boost_norm > 1) boost_norm = 1;
        int start = 130;
        int end = 130 + (int)(100 * boost_norm);
        lv_arc_set_angles(arc_boost, start, end);
        if(label_boost_val) lv_label_set_text_fmt(label_boost_val, "%.1f", boost);
        if (boost > 1.6f) lv_obj_set_style_arc_color(arc_boost, COLOR_BURGUNDY, LV_PART_INDICATOR);
        else              lv_obj_set_style_arc_color(arc_boost, COLOR_TEAL, LV_PART_INDICATOR);
    }

    if (arc_oilp) {
        float oil_norm = oil_press / 10.0f;
        if(oil_norm > 1) oil_norm = 1;
        if(oil_norm < 0) oil_norm = 0;
        int end_fixed = 50; 
        int start_dynamic = 50 - (int)(100 * oil_norm);
        if (start_dynamic < 0) start_dynamic += 360;
        lv_arc_set_angles(arc_oilp, start_dynamic, end_fixed);
        if(label_oilp_val) lv_label_set_text_fmt(label_oilp_val, "%.1f", oil_press);
        if (oil_press < 1.5f) lv_obj_set_style_arc_color(arc_oilp, COLOR_BURGUNDY, LV_PART_INDICATOR);
        else                  lv_obj_set_style_arc_color(arc_oilp, COLOR_TEAL, LV_PART_INDICATOR);
    }

    if (label_egt_val) lv_label_set_text_fmt(label_egt_val, "%d", egt);
    if (label_iat_val) lv_label_set_text_fmt(label_iat_val, "%d", iat);
    if (label_oilt_val) lv_label_set_text_fmt(label_oilt_val, "%d", oil_temp);
    if (label_clt_val) lv_label_set_text_fmt(label_clt_val, "%d", coolant_temp);

    if (coolant_temp > 105) lv_obj_set_style_bg_color(container_clt, COLOR_BURGUNDY, 0);
    else lv_obj_set_style_bg_color(container_clt, COLOR_BOX_BG, 0);

    if (oil_temp > 130) lv_obj_set_style_bg_color(container_oilt, COLOR_BURGUNDY, 0);
    else lv_obj_set_style_bg_color(container_oilt, COLOR_BOX_BG, 0);
}