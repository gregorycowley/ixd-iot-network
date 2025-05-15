#ifndef PIN_DEFINITIONS_H
#define PIN_DEFINITIONS_H

// Button pin definitions for 6-button controller
// Using D1, D2, D5, D6, D7, D8 pins
const int button1Pin = D1;  // Button 1
const int button2Pin = D2;  // Button 2
const int button3Pin = D5;  // Button 3
const int button4Pin = D6;  // Button 4
const int button5Pin = D7;  // Button 5
const int button6Pin = D8;  // Button 6

// Number of buttons
const int numButtons = 6;

// Debounce time in milliseconds
const int debounceTime = 50;

// Board detection and pin definitions
#ifdef ESP32
  // ESP32-WROOM-32 pin configuration
  const int ledPin = 27;     // IO27 for LED
  const int neoPixelPin = 16; // IO16 for NeoPixel
#else
  // ESP8266 pin configuration
  const int ledPin = 12;     // GPIO13 for LED
  const int neoPixelPin = 15 ; // GPIO12 for NeoPixel
#endif

const int numPixels = 1;    // Number of NeoPixels
const int brightness = 20;  // Low brightness

#endif // PIN_DEFINITIONS_H 