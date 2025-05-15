#include <Adafruit_NeoPixel.h>

// Pin definitions for ESP32-WROOM-32
const int buttonPin = 32;  // IO32 for button
const int ledPin = 27;     // IO27 for LED
const int neoPixelPin = 16; // IO16 for NeoPixel
const int numPixels = 1;    // Number of NeoPixels

// Brightness control (0-255)
const int brightness = 20;  // Low brightness

// Create NeoPixel object
Adafruit_NeoPixel pixels(numPixels, neoPixelPin, NEO_GRB + NEO_KHZ800);

// Variable to store the button state
int buttonState = 0;
// Variable to track color state
int colorState = 0;
// Variable to track last color change time
unsigned long lastColorChange = 0;

void setup() {
  // Initialize serial communication at 115200 baud rate (ESP32 default)
  Serial.begin(115200);
  
  // Set the button pin as input (using external pull-down resistor)
  pinMode(buttonPin, INPUT);
  // Set the LED pin as output
  pinMode(ledPin, OUTPUT);
  
  // Initialize NeoPixel
  pixels.begin();
  pixels.setBrightness(brightness);  // Set the global brightness
  pixels.show(); // Initialize all pixels to 'off'
}

void loop() {
  // Read the state of the button
  buttonState = digitalRead(buttonPin);
  
  // With pull-down resistor, the button will read HIGH when pressed
  if (buttonState == HIGH) {
    Serial.println("Button pressed!");
    // Turn on the LED
    digitalWrite(ledPin, HIGH);
    // Add a small delay to prevent multiple readings
    delay(100);
  } else {
    // Turn off the LED when button is not pressed
    digitalWrite(ledPin, LOW);
  }

  // Check if it's time to change the NeoPixel color
  if (millis() - lastColorChange >= 2000) {  // 2000ms = 2 seconds
    lastColorChange = millis();
    
    // Cycle through colors
    switch(colorState) {
      case 0:  // Red
        pixels.setPixelColor(0, pixels.Color(255, 0, 0));
        colorState = 1;
        break;
      case 1:  // Blue
        pixels.setPixelColor(0, pixels.Color(0, 0, 255));
        colorState = 2;
        break;
      case 2:  // Green
        pixels.setPixelColor(0, pixels.Color(0, 255, 0));
        colorState = 0;
        break;
    }
    pixels.show();  // Update the NeoPixel
  }
}
