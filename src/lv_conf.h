#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

#define LV_COLOR_DEPTH 32

// Use standard C library functions
#define LV_USE_STDLIB_MALLOC    LV_STDLIB_CLIB
#define LV_USE_STDLIB_STRING    LV_STDLIB_CLIB
#define LV_USE_STDLIB_SPRINTF   LV_STDLIB_CLIB

// Memory
#define LV_MEM_SIZE (128 * 1024U) // 128kB

// Tick interface
#define LV_TICK_CUSTOM 1
#if LV_TICK_CUSTOM
    #define LV_TICK_CUSTOM_INCLUDE <SDL.h>         /*Header for the system time function*/
    #define LV_TICK_CUSTOM_SYS_TIME_EXPR (SDL_GetTicks())     /*Expression evaluating to current system time in ms*/
#endif

// Enable Widgets
#define LV_USE_LABEL 1
#define LV_USE_BAR   1
#define LV_USE_ARC   1

// Fonts
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_20 1
#define LV_FONT_MONTSERRAT_24 1
#define LV_FONT_MONTSERRAT_28 1
#define LV_FONT_MONTSERRAT_32 1
#define LV_FONT_MONTSERRAT_48 1

#endif
