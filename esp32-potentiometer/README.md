# ESP32 Potentiometer MQTT Controller

This project uses an ESP32 WROOM 32 to read a potentiometer value and publish it to an MQTT broker.

## Hardware Requirements

- ESP32 WROOM 32
- Potentiometer
- USB cable for programming and power
- WiFi network access
- MQTT broker

## Pin Connections

- Potentiometer:
  - Middle pin (wiper) to GPIO 15 (D15)
  - One outer pin to 3.3V
  - Other outer pin to GND
- LED:
  - GPIO 5 (for status indication)

## Software Setup

1. Install required libraries in Arduino IDE:
   - PubSubClient (for MQTT)
   - WiFi (included with ESP32 board support)

2. Configure the following files:

### wifi_setup.h
```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
```

### mqtt_handler.h
```cpp
const char* mqtt_server = "YOUR_MQTT_BROKER_IP";
const int mqtt_port = 1883;
const char* mqtt_topic = "/cca/potentiometer/blue";
```

## Debug Mode

The code includes a debug mode that can be enabled/disabled by changing the `DEBUG_MODE` define in `potentiometer.ino`:

```cpp
#define DEBUG_MODE true  // Set to false to disable debug output
```

When debug mode is enabled, the following information will be printed to the Serial Monitor:

### Setup Debug Information
- Startup progress
- Pin initialization status
- WiFi connection status
- MQTT setup status

### Runtime Debug Information (every second)
- Current potentiometer reading
- WiFi connection status
- MQTT connection status
- WiFi signal strength (RSSI)

### Event-based Debug Information
- Potentiometer value changes
- MQTT publish events

Debug messages are prefixed with `[DEBUG]` for easy identification in the Serial Monitor.

## Functionality

- Reads potentiometer value (0-4095 range)
- Publishes value to MQTT topic `/cca/potentiometer/blue` when it changes by more than 10 units
- Prints value to Serial Monitor for debugging
- LED indicates startup status

## MQTT Topics

- Publish: `/cca/potentiometer/blue`
  - Message format: integer value (0-4095)

## Serial Output

The device will output the following to the Serial Monitor (9600 baud):
- WiFi connection status
- MQTT connection status
- Potentiometer values when they change significantly
- Debug information (when debug mode is enabled)

## Troubleshooting

1. If WiFi connection fails:
   - Check WiFi credentials
   - Ensure network is in range
   - Verify network allows ESP32 connections
   - Enable debug mode to see detailed connection status

2. If MQTT connection fails:
   - Verify MQTT broker IP and port
   - Check if broker is running
   - Ensure network allows MQTT traffic
   - Enable debug mode to see connection attempts

3. If potentiometer readings are unstable:
   - Check connections
   - Verify potentiometer is properly powered
   - Adjust the change threshold in the code if needed
   - Enable debug mode to monitor raw readings

4. Debug Mode Tips:
   - Use debug mode during initial setup and troubleshooting
   - Disable debug mode in production to reduce serial output
   - Monitor RSSI values to ensure good WiFi signal strength
   - Check MQTT connection status if messages aren't being received