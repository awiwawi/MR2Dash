#ifndef CAN_BUS_H
#define CAN_BUS_H

#include <stdint.h>
#include <stdbool.h>

// Initialize CAN interface
bool can_init(const char* interface_name);

// Thread function for background reading
int can_thread_entry(void* data);

// Getters
int can_get_rpm(void);
int can_get_speed(void);
float can_get_boost(void);
float can_get_oil_press(void); // Changed to float
int can_get_coolant_temp(void);
int can_get_oil_temp(void);
int can_get_egt(void);
int can_get_iat(void);

#endif // CAN_BUS_H
