#!/bin/bash

# Exit on error
set -e

# Configuration
BOARD="esp32:esp32:esp32c3"
PORT="/dev/tty.usbserial-*"  # This will match any USB serial device
SKETCH="xiao-esp32c3.ino"

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo "🔧 Building and uploading XIAO ESP32C3 project..."

# Check if arduino-cli is installed
if ! command -v arduino-cli &> /dev/null; then
    echo -e "${RED}Error: arduino-cli is not installed${NC}"
    echo "Please install it first:"
    echo "  brew install arduino-cli"
    exit 1
fi

# Add ESP32 board manager URL
echo "📦 Adding ESP32 board manager URL..."
arduino-cli config add board_manager.additional_urls https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

# Update core and libraries
echo "📦 Updating Arduino core and libraries..."
arduino-cli core update-index
arduino-cli core install esp32:esp32

# Compile the sketch
echo "🔨 Compiling sketch..."
arduino-cli compile --fqbn $BOARD $SKETCH

# Find the USB port
PORT=$(ls $PORT 2>/dev/null | head -n 1)
if [ -z "$PORT" ]; then
    echo -e "${RED}Error: No USB serial device found${NC}"
    echo "Please make sure your XIAO ESP32C3 is connected"
    exit 1
fi

# Upload the sketch
echo "📤 Uploading to $PORT..."
arduino-cli upload -p $PORT --fqbn $BOARD $SKETCH

echo -e "${GREEN}✅ Upload complete!${NC}"
echo "You can now open the Serial Monitor to see the output" 