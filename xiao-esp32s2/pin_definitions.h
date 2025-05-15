#ifndef PIN_DEFINITIONS_H
#define PIN_DEFINITIONS_H

#include <Arduino.h>

// Debug board detection
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
// #pragma message "Board definition: " TOSTRING(ARDUINO_BOARD)

// Board detection
// #if defined(ARDUINO_BOARD) && (ARDUINO_BOARD == "XIAO_ESP32C3" || ARDUINO_BOARD == "esp32c3")
//   #define BOARD_TYPE "XIAO_ESP32C3"
//   #define buttonPin 2
//   #define ledPin 7
//   #define neoPixelPin 3
//   #define batteryPin -1  // No ADC on ESP32C3
// #elif defined(ARDUINO_BOARD) && (ARDUINO_BOARD == "ESP32S2_DEV" || ARDUINO_BOARD == "esp32s3")
  #define BOARD_TYPE "ESP32S2_DEV"
  #define buttonPin 39
  #define ledPin -1     // No LED on ESP32S3
  #define neoPixelPin 40
  #define batteryPin 2  // ADC1_2 on ESP32S3
// #else
//   #error "Unsupported board type. Current board: " TOSTRING(ARDUINO_BOARD) ". Please select XIAO ESP32C3 or ESP32S2_DEV in the Arduino IDE."
// #endif

// NeoPixel configuration
const int numPixels = 1;
const int brightness = 50;

// Battery monitoring configuration
const float VOLTAGE_DIVIDER_RATIO = 2.0;  // 2x voltage divider (2x 10k resistors)
const float MAX_BATTERY_VOLTAGE = 4.2;    // Maximum battery voltage
const float MIN_BATTERY_VOLTAGE = 3.3;    // Minimum battery voltage

#endif // PIN_DEFINITIONS_H 