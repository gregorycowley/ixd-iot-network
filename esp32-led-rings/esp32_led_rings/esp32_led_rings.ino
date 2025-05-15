#include <Adafruit_NeoPixel.h>
#include "wifi_setup.h"
#include "mqtt_handler.h"
#include "pin_definitions.h"

// Define the pin connected to the NeoPixels
const int RING_COUNT = 9;
const int ring_counts[RING_COUNT] = {60, 48, 40, 32, 24, 16, 12, 8, 1};

// Create NeoPixel object
Adafruit_NeoPixel strip(241, neoPixelPin, NEO_GRB + NEO_KHZ800);  // Total of 241 LEDs

void setup() {
  // Initialize Serial for debugging
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n=== LED Ring Setup ===");
  Serial.print("NeoPixel pin: ");
  Serial.println(neoPixelPin);
  
  // Setup WiFi
  setupWiFi();
  
  // Setup MQTT
  setupMQTT();
  
  // Initialize the NeoPixel strip
  Serial.println("Initializing NeoPixel strip...");
  strip.begin();
  // Set all pixels to off
  strip.clear();
  strip.show();
  // Set brightness (0-255)
  strip.setBrightness(255);  // Set to maximum brightness for testing
  Serial.println("NeoPixel strip initialized");
  Serial.println("=====================\n");
}

void loop() {
  // Check and maintain WiFi connection
  if (!isWiFiConnected()) {
    reconnectWiFi();
  }
  
  // Handle MQTT connection and messages
  mqttLoop();
}