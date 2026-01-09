#ifndef UI_H
#define UI_H

#include "lvgl.h"

void ui_init(void);

// Updated with new sensor arguments
void ui_update_data(int rpm, int speed, 
                    float boost, float oil_press, 
                    int coolant_temp, int oil_temp, int egt, int iat);

#endif
