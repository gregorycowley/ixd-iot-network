# ESP32-S2 MicroPython Button Input Project

This project demonstrates button input handling on an ESP32-S2FN4R2 using MicroPython.

## Hardware Requirements
- ESP32-S2FN4R2 development board
- 6 push buttons
- 6 10kΩ pull-up resistors
- Jumper wires
- USB Type-C cable

## Button Connections
Connect the buttons to the following GPIO pins:
- Button 1: GPIO 1
- Button 2: GPIO 2
- Button 3: GPIO 4
- Button 4: GPIO 6
- Button 5: GPIO 8
- Button 6: GPIO 10

Each button should be connected to ground through a 10kΩ pull-up resistor.

## Setup Instructions

1. Install MicroPython on ESP32-S2:
   - Download the latest MicroPython firmware for ESP32-S2 from [MicroPython Downloads](https://micropython.org/download/ESP32_GENERIC_S2/)
   - Use esptool to flash the firmware:
     ```bash
     esptool.py --chip esp32s2 --port /dev/tty.usbserial-* erase_flash
     esptool.py --chip esp32s2 --port /dev/tty.usbserial-* write_flash -z 0x1000 esp32s2-firmware.bin
     ```

2. Upload the code:
   - Use Thonny IDE or rshell to upload `main.py` to the ESP32-S2
   - The program will start automatically after upload

## Usage
- Press any of the connected buttons to see a message printed to the serial console
- Each button press will trigger a unique message
- The program uses internal pull-up resistors, so buttons should be connected to ground

## Board Test Functionality
The project includes two independent test features that can be used to verify basic functionality of the ESP32-S2 Mini board.

### Board Test Features
- Blinks the onboard LED (connected to GPIO 35)
- Sends test messages through serial communication (TX on GPIO 10)
- Can be easily enabled/disabled

### WiFi Test Features
- Connects to WiFi network and displays IP address
- Automatically attempts to reconnect if connection is lost
- Can be enabled/disabled independently from board test

### Web Server Features
- Provides a web interface accessible from any device on the same network
- Displays real-time system information:
  - Uptime
  - Free heap memory
  - WiFi connection status
  - IP address
  - Signal strength (RSSI)
- Auto-updates every second
- Mobile-responsive design

### How to Use
1. Open `SerialMessage.ino` in the Arduino IDE
2. Features can be enabled/disabled using these defines:
   ```cpp
   #define ENABLE_BOARD_TEST true  // Set to false to disable board test
   #define ENABLE_WIFI true        // Set to false to disable WiFi
   #define ENABLE_WEB_SERVER true  // Set to false to disable web server
   ```
3. When board test is enabled:
   - The onboard LED will blink every second (500ms on, 500ms off)
   - A "Hello from ESP32-S2!" message will be sent through the serial connection every second
4. When WiFi is enabled:
   - The board will attempt to connect to the configured WiFi network
   - If connected, the IP address will be displayed in the serial output
   - Connection status will be monitored and reconnection attempted if needed
5. When web server is enabled:
   - Open a web browser on any device connected to the same network
   - Enter the ESP32's IP address in the address bar
   - View real-time system status and information

### Pin Connections
- LED: GPIO 35
- Serial TX: GPIO 10

### WiFi Configuration
WiFi settings can be modified in `WiFiManager.h`:
```cpp
const char* ssid = "your_ssid";
const char* password = "your_password";
```

## Troubleshooting
- If buttons are not responding, check the connections and ensure proper grounding
- Verify that the correct GPIO pins are being used
- Make sure the ESP32-S2 is properly powered
- For board test issues:
  - Verify the LED is connected to GPIO 35
  - Check that the serial TX pin (GPIO 10) is properly connected
  - Ensure the correct baud rate (115200) is set in your serial monitor
- For WiFi issues:
  - Verify the SSID and password are correct in WiFiManager.h
  - Check that the WiFi network is in range
  - Ensure the ESP32-S2 has a stable power supply
- For web server issues:
  - Ensure WiFi is enabled and connected
  - Verify you can ping the ESP32's IP address
  - Check that no other device is using port 80
  - Try accessing the web interface from different devices

## Required Libraries
The project requires the following libraries:
1. ESPAsyncWebServer (included in the project's `libraries` directory)
2. AsyncTCP (included in the project's `libraries` directory)

These libraries are required for the web server functionality and are included in the project's `libraries` directory. No additional installation is needed.
