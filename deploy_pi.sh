#!/bin/bash

# Configuration
APP_PATH="$(pwd)/build/MR2_Dash"
USER_NAME=$(whoami)

echo "Setting up MR2 Dash auto-start..."

# 1. Create CAN Setup Script
cat <<EOF > ./setup_can.sh
#!/bin/bash
sudo ip link set can0 up type can bitrate 500000
EOF
chmod +x ./setup_can.sh

# 2. Create Systemd Service for CAN
sudo bash -c "cat <<EOF > /etc/systemd/system/can-setup.service
[Unit]
Description=Setup CAN Interface
After=multi-user.target

[Service]
Type=oneshot
ExecStart=$(pwd)/setup_can.sh
RemainAfterExit=yes

[Install]
WantedBy=multi-user.target
EOF"

# 3. Create Systemd Service for Dashboard
# Note: SDL_VIDEODRIVER=kmsdrm allows it to run without a desktop environment (faster)
sudo bash -c "cat <<EOF > /etc/systemd/system/mr2dash.service
[Unit]
Description=MR2 LVGL Dashboard
After=can-setup.service

[Service]
ExecStart=$APP_PATH
WorkingDirectory=$(pwd)
User=$USER_NAME
Restart=always
RestartSec=5
# Environment variables for SDL2 performance on Pi
Environment=SDL_VIDEODRIVER=kmsdrm
Environment=SDL_FBDEV=/dev/fb0

[Install]
WantedBy=multi-user.target
EOF"

# 4. Enable Services
sudo systemctl daemon-reload
sudo systemctl enable can-setup.service
sudo systemctl enable mr2dash.service

echo "Done! Dashboard will start on next boot."
echo "You can start it now with: sudo systemctl start mr2dash"
