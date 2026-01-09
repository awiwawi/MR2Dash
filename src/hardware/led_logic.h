#ifndef LED_LOGIC_H
#define LED_LOGIC_H

#include "ws2812_driver.h"

// Calculate colors for the 8 LEDs based on RPM
void calculate_shift_lights(int rpm, led_color_t* leds);

#endif
