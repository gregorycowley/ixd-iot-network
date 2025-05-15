#include <esp_system.h>  // Required for esp_read_efuse_mac
#include <esp_sleep.h>   // Required for sleep mode
#include "pin_definitions.h"
#include "wifi_setup.h"
#include "mqtt_handler.h"

// Variable to store the button state
int buttonState = 0;
int lastButtonState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;  // Debounce time in milliseconds

// Variable to track last activity time
unsigned long lastActivityTime = 0;
const unsigned long SLEEP_TIMEOUT = 300000;  // 5 minutes in milliseconds

// Function to create button event message
String createButtonEventMessage(bool pressed) {
  String message = "{";
  message += "\"event\": \"";
  message += pressed ? "PRESSED" : "RELEASED";
  message += "\",";
  message += "\"timestamp\": ";
  message += millis();
  message += ",";
  message += "\"rssi\": ";
  message += WiFi.RSSI();
  message += "}";
  return message;
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== Xiao ESP32 Device Setup ===");
  
  // Configure pins
  pinMode(buttonPin, INPUT_PULLDOWN);
  pinMode(ledPin, OUTPUT);
  
  // Setup WiFi and MQTT
  setupWiFi();
  setupMQTT();
  
  Serial.println("Device is ready!");
  lastActivityTime = millis();
}

void enterSleepMode() {
  Serial.println("Entering sleep mode...");
  digitalWrite(ledPin, LOW);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  gpio_wakeup_enable((gpio_num_t)buttonPin, GPIO_INTR_HIGH_LEVEL);
  esp_sleep_enable_gpio_wakeup();
  esp_deep_sleep_start();
}

void loop() {
  if (millis() - lastActivityTime > SLEEP_TIMEOUT) {
    enterSleepMode();
  }

  reconnectWiFi();
  mqttLoop();

  // Read the state of the button
  int reading = digitalRead(buttonPin);

  // Check if the button state has changed
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  // If the button state has been stable for longer than the debounce delay
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // If the button state has changed
    if (reading != buttonState) {
      buttonState = reading;

      // If the button is pressed (HIGH with pull-down)
      if (buttonState == HIGH) {
        String message = createButtonEventMessage(true);
        publishMessage(message.c_str());
        lastActivityTime = millis();
        digitalWrite(ledPin, HIGH);
      } else {
        String message = createButtonEventMessage(false);
        publishMessage(message.c_str());
        lastActivityTime = millis();
        digitalWrite(ledPin, LOW);
      }
    }
  }

  // Save the reading for next time
  lastButtonState = reading;

  // Blink LED to show activity
  static unsigned long lastBlinkTime = 0;
  static bool ledState = false;
  
  if (millis() - lastBlinkTime >= 1000) {  // Blink every second
    lastBlinkTime = millis();
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
  }
}