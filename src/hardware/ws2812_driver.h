#ifndef WS2812_DRIVER_H
#define WS2812_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

// Color structure (GRB order is typical for WS2812)
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} led_color_t;

// Initialize SPI for WS2812
bool ws2812_init(int num_leds);

// Update all LEDs with the array of colors
void ws2812_update(led_color_t* colors);

// Cleanup
void ws2812_close(void);

#endif
