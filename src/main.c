#include <SDL.h>
#include "lvgl.h"
#include "ui/ui.h"
#include "can/can_bus.h"
#include "hardware/ws2812_driver.h"
#include "hardware/led_logic.h"
#include <stdio.h>

#define WINDOW_WIDTH 720
#define WINDOW_HEIGHT 720

// --- SDL Driver for LVGL ---
static SDL_Window * window;
static SDL_Renderer * renderer;
static SDL_Texture * texture;

static void display_flush_cb(lv_display_t * display, const lv_area_t * area, uint8_t * px_map) {
    int32_t width = lv_area_get_width(area);
    int32_t height = lv_area_get_height(area);

    SDL_Rect rect;
    rect.x = area->x1;
    rect.y = area->y1;
    rect.w = width;
    rect.h = height;

    SDL_UpdateTexture(texture, &rect, px_map, width * 4); 
    lv_display_flush_ready(display);
}

int main(int argc, char **argv) {
    (void)argc; (void)argv;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) return 1;

    window = SDL_CreateWindow("MR2 Dashboard", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
        
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, 
        SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);

    lv_init();

    lv_display_t * display = lv_display_create(WINDOW_WIDTH, WINDOW_HEIGHT);
    lv_display_set_flush_cb(display, display_flush_cb);
    
    #define BUF_SIZE (WINDOW_WIDTH * WINDOW_HEIGHT) 
    static uint32_t buf1[BUF_SIZE];
    lv_display_set_buffers(display, buf1, NULL, BUF_SIZE * 4, LV_DISPLAY_RENDER_MODE_FULL);

    if (!can_init("can0")) printf("Warning: CAN init failed.\n");
    
    // Initialize Hardware LEDs (8 LEDs)
    if (!ws2812_init(8)) printf("Warning: LED init failed (SPI disabled?).\n");

    SDL_Thread *thread = SDL_CreateThread(can_thread_entry, "CANThread", NULL);

    ui_init();

    bool quit = false;
    SDL_Event event;
    
    led_color_t leds[8];

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) quit = true;
        }

        // 1. Get Data
        int rpm = can_get_rpm();
        int speed = can_get_speed();
        float boost = can_get_boost();
        float oil_press = can_get_oil_press();
        int clt = can_get_coolant_temp();
        int oil_t = can_get_oil_temp();
        int egt = can_get_egt();
        int iat = can_get_iat();

        // 2. Update UI
        ui_update_data(rpm, speed, boost, oil_press, clt, oil_t, egt, iat);

        // 3. Update Hardware LEDs
        calculate_shift_lights(rpm, leds);
        ws2812_update(leds);

        lv_tick_inc(5);
        lv_timer_handler();

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        SDL_Delay(5); 
    }

    ws2812_close();
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
