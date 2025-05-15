#include <ESP8266WiFi.h>
#include "SleepManager.h"

// Pin definitions
const int WAKE_PIN = D0;  // Button connected to D0
const int LED_PIN = D4;   // Onboard LED pin

// Create sleep manager instance
SleepManager sleepManager(WAKE_PIN, LED_PIN);

void setup() {
  // Disable WiFi to save power
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
  delay(1);  // Give WiFi time to enter sleep mode
  
  Serial.begin(9600);
  Serial.println("Device starting...");
  
  sleepManager.begin();
}

void loop() {
  sleepManager.update();
}
