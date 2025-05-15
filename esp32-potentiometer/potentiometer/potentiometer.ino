#include <Arduino.h>
#include "pin_definitions.h"
#include "wifi_setup.h"
#include "mqtt_handler.h"

// Rotary Encoder Pins
#define ENCODER_PIN_A 2  // D2
#define ENCODER_PIN_B 16 // D16

// Potentiometer Pin
#define POT_PIN 15 // D15

// LED Pin for ESP32 WROOM 32 (using GPIO 5 instead of 2 to avoid conflict with encoder)
#define LED_PIN 5

// Variables for rotary encoder
volatile int encoderCount = 0;
volatile int lastEncoded = 0;
volatile long encoderValue = 0;

// Variables for potentiometer
int potValue = 0;
int lastPotValue = 0;

// Variables for debouncing
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 5; // milliseconds

// Debug mode - set to true to enable detailed serial output
#define DEBUG_MODE true

// Variables for potentiometer
unsigned long lastDebugTime = 0;
const unsigned long DEBUG_INTERVAL = 1000; // Debug output every 1 second

void debugPrint(const char* message) {
  if (DEBUG_MODE) {
    Serial.print("[DEBUG] ");
    Serial.println(message);
  }
}

void debugPrintValue(const char* label, int value) {
  if (DEBUG_MODE) {
    Serial.print("[DEBUG] ");
    Serial.print(label);
    Serial.print(": ");
    Serial.println(value);
  }
}

void updateEncoder() {
  // Read the current state of the encoder pins
  int MSB = digitalRead(ENCODER_PIN_A);
  int LSB = digitalRead(ENCODER_PIN_B);

  // Combine the two bits to form a binary number
  int encoded = (MSB << 1) | LSB;
  
  // Calculate the difference between the current and last state
  int sum = (lastEncoded << 2) | encoded;

  // Handle the state change
  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
    encoderValue++;
  }
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
    encoderValue--;
  }

  // Update the last encoded value
  lastEncoded = encoded;
}

void setup() {
  // Initialize Serial communication
  Serial.begin(9600);
  delay(1000);
  
  debugPrint("Starting setup...");
  
  // Initialize LED pin
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); // Turn on LED to indicate startup
  debugPrint("LED pin initialized");
  
  // Set up potentiometer pin
  pinMode(POT_PIN, INPUT);
  debugPrint("Potentiometer pin initialized");
  
  // Setup WiFi
  debugPrint("Setting up WiFi...");
  setupWiFi();
  debugPrint("WiFi setup complete");
  
  // Setup MQTT
  debugPrint("Setting up MQTT...");
  setupMQTT();
  debugPrint("MQTT setup complete");
  
  digitalWrite(LED_PIN, LOW); // Turn off LED after setup
  debugPrint("Setup complete");
}

void loop() {
  // Read potentiometer value
  potValue = analogRead(POT_PIN);
  
  // Print and publish potentiometer value when it changes significantly
  if (abs(potValue - lastPotValue) > 10) { // Only process if change is more than 10
    lastPotValue = potValue;
    
    // Print to Serial for debugging
    debugPrintValue("Potentiometer Value", potValue);
    
    // Publish to MQTT
    debugPrint("Publishing to MQTT...");
    publishPotentiometerValue(potValue);
    debugPrint("MQTT publish complete");
  }
  
  // Periodic debug output
  if (DEBUG_MODE && millis() - lastDebugTime >= DEBUG_INTERVAL) {
    lastDebugTime = millis();
    debugPrintValue("Current Potentiometer Reading", potValue);
    debugPrintValue("WiFi Status", WiFi.status());
    debugPrintValue("MQTT Connection Status", client.connected() ? 1 : 0);
    debugPrintValue("RSSI", WiFi.RSSI());
  }
  
  // Small delay to prevent overwhelming the output
  delay(10);
} 