#include "WiFiManager.h"
#include "WebServerManager.h"
#include "MQTTManager.h"
// #include "SleepManager.h"
#include "Config_device.h"

// Pin definitions
const int BUTTON_PIN = D10;  // Button connected to D10
const int LED_PIN = D9;      // LED connected to D9

// Create manager instances
WiFiManager wifiManager;
WebServerManager webServer;
MQTTManager mqttManager;
// SleepManager sleepManager;  // Temporarily disabled

// Button state variables
int lastButtonState = LOW;   // Previous button state
int buttonState = LOW;       // Current button state
unsigned long lastDebounceTime = 0;  // Last time the button state changed
const unsigned long debounceDelay = 50;  // Debounce time in milliseconds

// LED control callback
void handleLEDControl(bool state) {
    digitalWrite(LED_PIN, state ? HIGH : LOW);
    webServer.setLEDState(state);
    Serial.println(state ? "LED turned ON via MQTT" : "LED turned OFF via MQTT");
    // sleepManager.resetSleepTimer();  // Temporarily disabled
}

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  
  // Print MAC address
  uint8_t mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC Address: ");
  for(int i = 0; i < 6; i++) {
    Serial.printf("%02X", mac[i]);
    if(i < 5) Serial.print(":");
  }
  Serial.println();
  
  // Configure button pin as input with internal pull-down
  pinMode(BUTTON_PIN, INPUT);
  
  // Configure LED pin as output
  pinMode(LED_PIN, OUTPUT);
  
  // Initial LED state (off)
  digitalWrite(LED_PIN, LOW);
  webServer.setLEDState(false);
  
  Serial.println("XIAO ESP32C3 Button and LED Test");
  Serial.print("Device ID: ");
  Serial.println(DEVICE_ID);
  Serial.print("Series ID: ");
  Serial.println(SERIES_ID);
  
  // Set up LED control callback
  mqttManager.setLEDCallback(handleLEDControl);
  
  // Initialize sleep manager
  // sleepManager.begin();  // Temporarily disabled
  
  // Connect to WiFi
  if (wifiManager.connect()) {
    // Start web server
    webServer.begin();
    // Start MQTT
    mqttManager.begin();
  }
}

void loop() {
  // Check WiFi connection status
  wifiManager.checkConnection();
  
  // Handle web server clients
  webServer.handleClient();
  
  // Handle MQTT
  mqttManager.loop();
  
  // Handle sleep management
  // sleepManager.loop();  // Temporarily disabled
  
  // Read the current button state
  int reading = digitalRead(BUTTON_PIN);
  
  // Check if the button state has changed
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  
  // If the button state has been stable for longer than the debounce delay
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // If the button state has changed
    if (reading != buttonState) {
      buttonState = reading;
      
      // Toggle LED when button is pressed
      if (buttonState == HIGH) {
        digitalWrite(LED_PIN, HIGH);
        webServer.setLEDState(true);
        Serial.println("Button pressed - LED ON");
        // Publish button press to MQTT
        mqttManager.publishButtonPress();
        // Reset sleep timer on button press
        // sleepManager.resetSleepTimer();  // Temporarily disabled
      } else {
        digitalWrite(LED_PIN, LOW);
        webServer.setLEDState(false);
        Serial.println("Button released - LED OFF");
      }
    }
  }
  
  // Save the current button state for the next comparison
  lastButtonState = reading;
}
