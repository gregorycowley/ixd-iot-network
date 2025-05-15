#include <Adafruit_NeoPixel.h>

// Define the pin connected to the NeoPixels
#define LED_PIN    12
#define RING_COUNT 9
int ring_counts[RING_COUNT] = {60, 48, 40, 32, 24, 16, 12, 8, 1};

// Create NeoPixel object
Adafruit_NeoPixel strip(241, LED_PIN, NEO_GRB + NEO_KHZ800);  // Total of 241 LEDs

void setup() {
  // Initialize the NeoPixel strip
  strip.begin();
  // Set all pixels to off
  strip.show();
  // Set brightness (0-255)
  strip.setBrightness(100);
}

void loop() {
  // Light up each ring from outer to inner
  for(int ring = 0; ring < RING_COUNT; ring++) {
    // Turn off all pixels
    strip.clear();
    
    // Calculate the starting index for this ring
    int start_index = 0;
    for(int i = 0; i < ring; i++) {
      start_index += ring_counts[i];
    }
    
    // Light up all LEDs in the current ring
    for(int i = 0; i < ring_counts[ring]; i++) {
      strip.setPixelColor(start_index + i, strip.Color(255, 255, 255));
    }
    
    // Update the strip
    strip.show();
    // Wait for 500 milliseconds
    delay(500);
  }
  
  // Light up each ring from inner to outer
  for(int ring = RING_COUNT - 2; ring >= 0; ring--) {
    // Turn off all pixels
    strip.clear();
    
    // Calculate the starting index for this ring
    int start_index = 0;
    for(int i = 0; i < ring; i++) {
      start_index += ring_counts[i];
    }
    
    // Light up all LEDs in the current ring
    for(int i = 0; i < ring_counts[ring]; i++) {
      strip.setPixelColor(start_index + i, strip.Color(255, 255, 255));
    }
    
    // Update the strip
    strip.show();
    // Wait for 500 milliseconds
    delay(500);
  }
} 