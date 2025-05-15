# Basic D1 IoT Device

This project implements a basic IoT device using either ESP32 or ESP8266 with WiFi and MQTT capabilities.

## Hardware Requirements

- ESP32 or ESP8266 board
- Button (with pull-down resistor)
- LED
- NeoPixel LED
- External pull-down resistor for the button

## Pin Configuration

### ESP32 Configuration
- Button: GPIO 32
- LED: GPIO 27
- NeoPixel: GPIO 16

### ESP8266 Configuration
- Button: GPIO 15
- LED: GPIO 13
- NeoPixel: GPIO 12

## Command Line Instructions

### Prerequisites
- Install Arduino CLI: https://arduino.github.io/arduino-cli/latest/installation/
- Install ESP8266 board support:
  ```bash
  arduino-cli core update-index
  arduino-cli core install esp8266:esp8266
  ```

### Compile and Upload
To compile and upload the code to your ESP8266 board:

```bash
# Compile the code
arduino-cli compile -b esp8266:esp8266:d1 basic-d1-6button/basic-d1-6button.ino

# Upload to the board (replace /dev/cu.usbserial-2110 with your actual port)
arduino-cli upload -p /dev/cu.usbserial-2110 -b esp8266:esp8266:d1 basic-d1-6button/basic-d1-6button.ino
```

### Monitor Serial Output
To monitor the serial output from the board:

```bash
# Start serial monitor (replace /dev/cu.usbserial-2110 with your actual port)
arduino-cli monitor -p /dev/cu.usbserial-2110 -c baudrate=115200
```

### Finding the Correct Serial Port
To find the correct serial port for your ESP8266 board:

```bash
# On macOS/Linux
ls /dev/cu.*

# On Windows
# Check Device Manager for COM ports
```

## WiFi Setup

The device uses WiFi to connect to your local network. To configure WiFi:

1. Open `wifi_setup.h`
2. Replace the following placeholders with your network credentials:
   ```cpp
   const char* ssid = "YOUR_WIFI_SSID";
   const char* password = "YOUR_WIFI_PASSWORD";
   ```

The device will automatically:
- Connect to WiFi on startup
- Attempt to reconnect if the connection is lost
- Print connection status and IP address to Serial monitor

## MQTT Setup

The device uses MQTT for communication with a broker. To configure MQTT:

1. Open `mqtt_handler.h`
2. Replace the following placeholders with your MQTT broker details:
   ```cpp
   const char* mqtt_server = "YOUR_MQTT_BROKER_IP";
   const char* mqtt_username = "YOUR_MQTT_USERNAME";
   const char* mqtt_password = "YOUR_MQTT_PASSWORD";
   ```

### Unique Device Identification

Each device automatically generates a unique client ID based on its MAC address:
- Format: `basic-d1-{MAC_ADDRESS}`
- Example: `basic-d1-1234567890ab`

This ensures that multiple devices can run the same code without conflicts on the MQTT broker.

### MQTT Topics

- **Publish Topic**: `basic-d1/status`
  - The device publishes "BUTTON_PRESSED" when the button is pressed

- **Subscribe Topic**: `basic-d1/control`
  - The device subscribes to this topic and responds to the following commands:
    - `ON`: Turns on the LED and sets NeoPixel to white
    - `OFF`: Turns off the LED and sets NeoPixel to off

### MQTT Features

- Automatic reconnection if connection is lost
- Secure authentication with username and password
- Message handling for device control
- Status publishing for button events
- Unique client IDs based on MAC address

## Required Libraries

1. Adafruit NeoPixel
2. PubSubClient (for MQTT)
3. WiFi (built-in for ESP32)
4. ESP8266WiFi (for ESP8266)

## Installation

1. Install the required libraries through the Arduino Library Manager
2. Configure WiFi and MQTT settings as described above
3. Upload the code to your device
4. Open the Serial Monitor to view connection status and debug information

## Troubleshooting

### WiFi Connection Issues
- Verify your WiFi credentials are correct
- Check that your network is within range
- Ensure your network allows new devices to connect

### MQTT Connection Issues
- Verify your MQTT broker is running and accessible
- Check that the broker IP address is correct
- Ensure your username and password are correct
- Verify that the broker port (default: 1883) is open and accessible
- Check the Serial Monitor for the unique client ID being used

## Security Notes

- Never commit your actual WiFi or MQTT credentials to version control
- Consider using environment variables or a separate configuration file for sensitive data
- Use secure MQTT (MQTTS) if your broker supports it
- The MAC address is used as part of the client ID, which is visible on the network 