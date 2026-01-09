#include "ws2812_driver.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static int led_count = 0;

#ifdef __linux__
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

static int spi_fd = -1;
static uint8_t* spi_buffer = NULL;
static int spi_buffer_len = 0;

// SPI Configuration
static const char *device = "/dev/spidev0.0";
static uint32_t mode = 0;
static uint8_t bits = 8;
static uint32_t speed = 2400000; // 2.4 MHz matches WS2812 timing with 3 bits per bit
static uint16_t delay = 0;

bool ws2812_init(int num_leds) {
    led_count = num_leds;
    
    // Open SPI
    spi_fd = open(device, O_RDWR);
    if (spi_fd < 0) {
        perror("WS2812: Failed to open SPI device (Run raspi-config to enable SPI?)");
        return false;
    }

    // Setup SPI
    if (ioctl(spi_fd, SPI_IOC_WR_MODE, &mode) == -1) return false;
    if (ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits) == -1) return false;
    if (ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1) return false;

    // Buffer allocation:
    // Each LED has 24 bits (G,R,B).
    // Each logical bit needs 3 SPI bits (to emulate timing).
    // 24 * 3 = 72 bytes per LED.
    // + Reset signal (roughly 50us of zeros). At 2.4MHz, 1 byte is ~3.3us. So ~20 bytes zero.
    spi_buffer_len = (led_count * 72) + 64; 
    spi_buffer = malloc(spi_buffer_len);
    if (!spi_buffer) {
        perror("WS2812: Failed to allocate SPI buffer");
        close(spi_fd);
        return false;
    }
    memset(spi_buffer, 0, spi_buffer_len); // Reset signal at end/start

    printf("WS2812: Hardware initialized on %s\n", device);
    return true;
}

void ws2812_update(led_color_t* colors) {
    if (spi_fd < 0 || !spi_buffer) return;

    int buf_idx = 0;

    for (int i = 0; i < led_count; i++) {
        // WS2812 expects GRB order
        uint32_t color = (colors[i].g << 16) | (colors[i].r << 8) | colors[i].b;

        // Process 24 bits (MSB first)
        for (int bit = 23; bit >= 0; bit--) {
            if ((color >> bit) & 1) {
                // '1' code: High, High, Low (110 binary = 0x6)
                // Actually we map to bytes. 3 SPI bits = 1 data bit.
                // It's easier to pack byte-by-byte.
                // Let's use a byte for every 3 bits? No, 1 byte = 8 bits.
                // 3 bits * 8 = 24 bits. So 1 byte of color = 3 bytes of SPI.
                // Wait, logic check:
                // 1 LED bit = 3 SPI bits.
                // 1 Byte (8 LED bits) = 24 SPI bits = 3 Bytes SPI.
                // Perfect.
            }
        }
        
        // Simpler loop: Byte by Byte (G, R, B)
        uint8_t bytes[3] = {colors[i].g, colors[i].r, colors[i].b};
        
        for (int b = 0; b < 3; b++) {
            uint8_t val = bytes[b];
            for (int bit = 7; bit >= 0; bit--) {
                // SPI encoding for 2.4MHz:
                // '0' = 100 (High 0.41us, Low 0.83us) -> 0b100xxxxx (Top 3 bits of byte? No)
                // We need to construct the SPI stream bit by bit.
                // This is complex to pack into bytes.
                // Easier method: Use 6.4MHz? 
                // Let's use the byte-expansion method.
                // 1 LED Data Bit -> 1 SPI Byte (wasteful but easy).
                // '0' = 0xC0 (11000000) at higher freq? 
                // Standard 2.4MHz method:
                // 1 data bit = 3 SPI bits.
                // 8 data bits = 24 SPI bits = 3 SPI Bytes.
                
                // Let's pack 8 data bits into 3 SPI bytes.
                // Bit 7 (MSB): 1xx
                // Bit 6:       1xx
                // ...
                
                // For simplicity in this generated code, I'll use a lookup table
                // mapping 4 bits of color to 12 bits of SPI (1.5 bytes? awkward).
                
                // Let's just do the "1 bit = 1 byte" approach at **6.0 MHz**.
                // '0' = 0xF0 (High 0.33us? No, 6MHz is 0.16us period).
                // T0H = 0.4us = ~2.4 bits. T0L = 0.85us = ~5 bits.
                // Pattern 11100000 (0xE0) = 3 high (0.5us), 5 low (0.8us). Close enough.
                // T1H = 0.8us = ~5 bits. T1L = 0.45us = ~2.5 bits.
                // Pattern 11111000 (0xF8) = 5 high (0.8us), 3 low (0.5us).
                
                // So at 6.0 MHz: 
                // '0' -> 0xE0
                // '1' -> 0xF8
                
                spi_buffer[buf_idx++] = ((val >> bit) & 1) ? 0xF8 : 0xE0;
            }
        }
    }

    // Reset signal (60 bytes of 0 at end)
    for(int k=0; k<60; k++) spi_buffer[buf_idx++] = 0x00;

    // Send
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)spi_buffer,
        .rx_buf = 0,
        .len = buf_idx,
        .speed_hz = 6000000, // 6 MHz
        .delay_usecs = 0,
        .bits_per_word = 8,
    };

    ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr);
}

void ws2812_close(void) {
    if (spi_buffer) free(spi_buffer);
    if (spi_fd >= 0) close(spi_fd);
}

#else
// --- WINDOWS DUMMY DRIVER ---
bool ws2812_init(int num_leds) {
    led_count = num_leds;
    printf("WS2812: Windows Simulation Initialized (%d LEDs)\n", num_leds);
    return true;
}

void ws2812_update(led_color_t* colors) {
    // Debug print only occasionally to not spam
    static int skip = 0;
    if (skip++ % 30 == 0) {
        printf("LEDs: [%d %d %d] ...\n", colors[0].r, colors[0].g, colors[0].b);
    }
}

void ws2812_close(void) {
    printf("WS2812: Closed.\n");
}
#endif
