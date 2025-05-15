#ifndef PIN_DEFINITIONS_H
#define PIN_DEFINITIONS_H

// Potentiometer Pin
#define POT_PIN 15 // D15

// LED Pin for ESP32 WROOM 32
#define LED_PIN 5

// Board detection and pin definitions
#ifdef ESP32
  // ESP32-WROOM-32 pin configuration
  const int buttonPin = 32;  // IO32 for button
  const int neoPixelPin = 16; // IO16 for NeoPixel
#else
  // ESP8266 pin configuration
  const int buttonPin = 13;  // GPIO15 for button
  const int ledPin = 12;     // GPIO13 for LED
  const int neoPixelPin = 15 ; // GPIO12 for NeoPixel
#endif

const int numPixels = 1;    // Number of NeoPixels
const int brightness = 20;  // Low brightness

#endif // PIN_DEFINITIONS_H 