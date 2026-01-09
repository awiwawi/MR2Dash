# MR2 Dashboard - Project Overview

## Project Summary
This project is a digital instrument cluster designed for a Toyota MR2, running on a Raspberry Pi 5. It interfaces with an Ecumaster Black ECU via CAN bus to display real-time vehicle telemetry on a 4-inch circular HDMI display (720x720). It also drives a set of hardware shift lights (WS2812B LEDs) via SPI.

**Key Technologies:**
*   **Language:** C/C++
*   **UI Framework:** LVGL v9 (Light and Versatile Graphics Library)
*   **HAL:** SDL2 (Simple DirectMedia Layer) for display/input abstraction.
*   **Build System:** CMake
*   **Communication:** SocketCAN (Linux), SPI (LEDs).

## Architecture

The application is multi-threaded to ensure UI smoothness regardless of CAN bus traffic.

1.  **Main Thread (`src/main.c`):**
    *   Initializes SDL2, LVGL, and Hardware drivers.
    *   Runs the main event loop.
    *   Polls data from the CAN module (thread-safe).
    *   Updates the UI and Hardware LEDs.
    *   Renders the frame.

2.  **CAN Thread (`src/can/can_bus.c`):**
    *   Runs strictly in the background.
    *   Reads frames from the CAN interface (`can0`).
    *   Parses Ecumaster Black protocol (Base ID 0x600).
    *   Updates shared state protected by `SDL_mutex`.

3.  **Hardware Abstraction:**
    *   **Linux/RPi:** Uses native `SocketCAN` and `/dev/spidev0.0`.
    *   **Windows/Dev:** Uses simulated data generators and dummy drivers for testing without hardware.

## Building and Running

### Prerequisites
*   **Linux (RPi):** `sudo apt install libsdl2-dev cmake build-essential libcan-utils`
*   **Windows:** Visual Studio with C++ CMake tools, SDL2 development libraries.

### Build Commands
```bash
mkdir -p build
cd build
cmake ..
make -j$(nproc)  # or 'cmake --build .' on Windows
```

### Running
*   **Manual:** `./build/MR2_Dash`
*   **Auto-Start:** Use the provided `deploy_pi.sh` script to install systemd services.

## Key Files and Directories

*   `src/main.c`: Application entry point and coordination logic.
*   `src/ui/ui.c`: LVGL widget definitions (Gauges, Arcs, Text).
*   `src/can/can_bus.c`: CAN reading, parsing, and thread-safe data storage.
*   `src/hardware/ws2812_driver.c`: SPI driver for WS2812B LEDs.
*   `src/hardware/led_logic.c`: Logic mapping RPM to LED colors/patterns.
*   `deploy_pi.sh`: Script to automate systemd service creation for auto-boot.
*   `setup.txt`: Detailed wiring and deployment instructions.
*   `Audit.txt`: Security audit report and hardening details.

## Development Conventions

*   **Thread Safety:** **CRITICAL**. Never access global sensor variables directly. Use the getter functions in `can_bus.h` which lock the `data_mutex`.
*   **Input Sanitization:** All CAN data is clamped to physical limits before storage to prevent UI glitches.
*   **Resolution:** Targeted for 720x720 circular display.
*   **Coding Style:** C11 standard. Explicit casing for bitwise operations.
