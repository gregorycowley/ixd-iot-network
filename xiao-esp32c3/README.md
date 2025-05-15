# XIAO ESP32C3 Button and LED Project

This project demonstrates basic input/output functionality using a XIAO ESP32C3 microcontroller. It implements a button input with debouncing, LED output control, WiFi connectivity, web server interface, and MQTT communication.

## Hardware Setup

### Components
- XIAO ESP32C3 microcontroller
- Push button
- LED
- 10kΩ pull-down resistor
- Jumper wires

### Connections
- Button: 
  - One terminal to 3.3V
  - Other terminal to GPIO D10
  - 10kΩ pull-down resistor between D10 and GND
- LED:
  - Anode (longer leg) to GPIO D9
  - Cathode (shorter leg) to GND through a current-limiting resistor (220Ω recommended)

## Software Features
- Button debouncing to prevent false triggers
- LED control based on button state
- Serial monitoring for debugging
- Button is normally LOW, goes HIGH when pressed
- LED is normally OFF, turns ON when button is pressed
- WiFi connectivity with automatic reconnection
- Modular WiFi management system
- Web server interface
- Configuration management system
- MQTT communication for button events

## Project Structure
- `xiao-esp32c3.ino`: Main Arduino sketch
- `WiFiManager.h`: WiFi management class header
- `WiFiManager.cpp`: WiFi management class implementation
- `WebServerManager.h`: Web server management class header
- `WebServerManager.cpp`: Web server management class implementation
- `MQTTManager.h`: MQTT management class header
- `MQTTManager.cpp`: MQTT management class implementation
- `Config.h`: Configuration declarations
- `Config.cpp`: Configuration definitions

## Usage
1. Upload the code to your XIAO ESP32C3
2. Open Serial Monitor at 115200 baud rate
3. The device will attempt to connect to the configured WiFi network
4. Once connected, the web server will start and MQTT connection will be established
5. Press the button to toggle the LED state and publish MQTT message
6. Monitor button, LED, WiFi, and MQTT state changes in the Serial Monitor

## Web Server Interface
The device exposes a web server on port 80 with the following endpoints:
- `GET /`: Returns device status in JSON format
  ```json
  {
    "device_id": "wolf",
    "series_id": "101",
    "status": "online",
    "ip": "192.168.1.xxx"
  }
  ```

## MQTT Communication
The device connects to an MQTT broker at 192.168.100.1:1883 and:
- Publishes button press events to topic `/cca/101/wolf/pub`
- Message format:
  ```json
  {
    "device_id": "wolf",
    "series_id": "101",
    "event": "button_press"
  }
  ```
- Uses a unique client ID based on device and series ID
- Automatically reconnects if connection is lost

## Pin Configuration
- Button: GPIO D10
- LED: GPIO D9

## WiFi Configuration
The device is configured to connect to the "IXD-Studio-LS" network. The WiFi management system includes:
- Automatic connection on startup
- Connection status monitoring
- Automatic reconnection if connection is lost
- Connection timeout handling

## Configuration
Device settings are managed through `Config.h` and `Config.cpp`:
- Device ID: "wolf"
- Series ID: "101"
- WiFi credentials
- Web server port
- MQTT broker settings
