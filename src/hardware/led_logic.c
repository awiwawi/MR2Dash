#include "led_logic.h"
#include "../lv_conf.h" 
#include "lvgl.h" // For lv_tick_get

void calculate_shift_lights(int rpm, led_color_t* leds) {
    // Logic:
    // 0-4000: Off
    // 4000-6000: Green (LEDs 0-3)
    // 6000-7000: White (LEDs 4-5)
    // 7000-8000: Red   (LEDs 6-7)
    // >8000: Blink All White/Red

    // Clear all first
    for(int i=0; i<8; i++) {
        leds[i].r = 0; leds[i].g = 0; leds[i].b = 0;
    }

    if (rpm >= 8000) {
        // Redline Blink
        bool blink = (lv_tick_get() % 150 < 75); // Fast blink
        for(int i=0; i<8; i++) {
            if (blink) {
                // White
                leds[i].r = 255; leds[i].g = 255; leds[i].b = 255;
            } else {
                // Red
                leds[i].r = 255; leds[i].g = 0;   leds[i].b = 0;
            }
        }
        return;
    }

    // Normal progression
    int leds_active = 0;
    
    // Calculate how many LEDs should be ON
    if (rpm > 4000) leds_active = 1;
    if (rpm > 4500) leds_active = 2;
    if (rpm > 5000) leds_active = 3;
    if (rpm > 5500) leds_active = 4;
    if (rpm > 6000) leds_active = 5;
    if (rpm > 6500) leds_active = 6;
    if (rpm > 7000) leds_active = 7;
    if (rpm > 7500) leds_active = 8;

    for (int i=0; i < leds_active; i++) {
        if (i < 4) {
            // Green (Dimmed slightly to not blind driver)
            leds[i].r = 0; leds[i].g = 100; leds[i].b = 0;
        } else if (i < 6) {
            // White
            leds[i].r = 100; leds[i].g = 100; leds[i].b = 100;
        } else {
            // Red
            leds[i].r = 200; leds[i].g = 0;   leds[i].b = 0;
        }
    }
}
