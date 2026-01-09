#include "can_bus.h"
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- SHARED DATA STORE ---
static volatile int current_rpm = 0;
static volatile int current_speed = 0;
static volatile float current_boost = 0.0f;
static volatile int current_oil_press = 0;
static volatile int current_clt = 0;
static volatile int current_oil_t = 0;
static volatile int current_egt = 0;
static volatile int current_iat = 0;

static SDL_mutex* data_mutex = NULL;

// Helper to clamp values
static float clamp_f(float val, float min, float max) {
    if (val < min) return min;
    if (val > max) return max;
    return val;
}

static int clamp_i(int val, int min, int max) {
    if (val < min) return min;
    if (val > max) return max;
    return val;
}

// --- LINUX / SOCKETCAN ---
#ifdef __linux__
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

static int s_socket = -1;

bool can_init(const char* interface_name) {
    if (!data_mutex) data_mutex = SDL_CreateMutex();

    struct sockaddr_can addr;
    struct ifreq ifr;

    if ((s_socket = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        perror("CAN socket");
        return false;
    }

    strcpy(ifr.ifr_name, interface_name);
    ioctl(s_socket, SIOCGIFINDEX, &ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(s_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("CAN bind");
        return false;
    }
    printf("CAN: Connected to %s\n", interface_name);
    return true;
}

int can_thread_entry(void* data) {
    (void)data;
    struct can_frame frame;
    printf("CAN: Listener thread started.\n");

    while (1) {
        int nbytes = read(s_socket, &frame, sizeof(struct can_frame));
        if (nbytes < 0) {
            perror("CAN read");
            SDL_Delay(100);
            continue;
        }

        SDL_LockMutex(data_mutex);
        switch(frame.can_id) {
            case 0x600: {
                uint16_t raw_rpm = (uint16_t)frame.data[0] | ((uint16_t)frame.data[1] << 8);
                current_rpm = clamp_i((int)raw_rpm, 0, 12000);

                int8_t raw_iat = (int8_t)frame.data[3];
                current_iat = clamp_i((int)raw_iat, -40, 150);

                uint16_t raw_map = (uint16_t)frame.data[4] | ((uint16_t)frame.data[5] << 8);
                float boost = ((float)raw_map / 100.0f) - 1.0f;
                current_boost = clamp_f(boost, -1.0f, 4.0f);
                break;
            }
            case 0x602: {
                uint16_t raw_egt = (uint16_t)frame.data[3] | ((uint16_t)frame.data[4] << 8);
                current_egt = clamp_i((int)raw_egt, 0, 1200);

                uint16_t raw_speed = (uint16_t)frame.data[5] | ((uint16_t)frame.data[6] << 8);
                current_speed = clamp_i((int)raw_speed, 0, 400);
                break;
            }
            case 0x603: {
                current_clt = clamp_i((int)((int8_t)frame.data[0]), -40, 150);
                current_oil_t = clamp_i((int)((int8_t)frame.data[1]), -40, 180);
                current_oil_press = clamp_i((int)frame.data[2], 0, 12);
                break;
            }
        }
        SDL_UnlockMutex(data_mutex);
    }
    return 0;
}

#else
// --- WINDOWS SIMULATION ---
bool can_init(const char* interface_name) {
    (void)interface_name;
    if (!data_mutex) data_mutex = SDL_CreateMutex();
    printf("CAN: Windows detected - SIMULATION MODE.\n");
    return true;
}

int can_thread_entry(void* data) {
    (void)data;
    printf("CAN: Simulation thread started.\n");
    while (1) {
        SDL_LockMutex(data_mutex);
        static int dir = 1;
        current_rpm += (150 * dir);
        if (current_rpm > 8500) dir = -1;
        if (current_rpm < 800) dir = 1;

        current_speed = current_rpm / 100;
        current_boost = ((float)current_rpm / 8000.0f) * 2.5f - 1.0f;
        current_oil_press = clamp_i(2 + (current_rpm / 2000), 0, 10);
        current_egt = 300 + (current_rpm / 15);
        current_clt = 88 + (rand() % 3);
        current_oil_t = 95 + (rand() % 2);
        current_iat = 35;
        SDL_UnlockMutex(data_mutex);

        SDL_Delay(33);
    }
    return 0;
}
#endif

// --- THREAD-SAFE GETTERS ---
int can_get_rpm(void) { 
    SDL_LockMutex(data_mutex);
    int val = current_rpm;
    SDL_UnlockMutex(data_mutex);
    return val;
}
int can_get_speed(void) { 
    SDL_LockMutex(data_mutex);
    int val = current_speed;
    SDL_UnlockMutex(data_mutex);
    return val;
}
float can_get_boost(void) { 
    SDL_LockMutex(data_mutex);
    float val = current_boost;
    SDL_UnlockMutex(data_mutex);
    return val;
}
int can_get_oil_press(void) { 
    SDL_LockMutex(data_mutex);
    int val = current_oil_press;
    SDL_UnlockMutex(data_mutex);
    return val;
}
int can_get_coolant_temp(void) { 
    SDL_LockMutex(data_mutex);
    int val = current_clt;
    SDL_UnlockMutex(data_mutex);
    return val;
}
int can_get_oil_temp(void) { 
    SDL_LockMutex(data_mutex);
    int val = current_oil_t;
    SDL_UnlockMutex(data_mutex);
    return val;
}
int can_get_egt(void) { 
    SDL_LockMutex(data_mutex);
    int val = current_egt;
    SDL_UnlockMutex(data_mutex);
    return val;
}
int can_get_iat(void) { 
    SDL_LockMutex(data_mutex);
    int val = current_iat;
    SDL_UnlockMutex(data_mutex);
    return val;
}