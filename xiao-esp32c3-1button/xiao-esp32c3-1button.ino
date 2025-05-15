// XIAO ESP32C3 Button and Battery Monitor
// Button connected to D10 and 3.3V
// Battery voltage divider connected to A0 (2x 10k resistors)

#include "sleep_manager.h"
#include "wifi_manager.h"

const int BUTTON_PIN = 10;  // Button connected to D10
const int BATTERY_PIN = A0; // Battery voltage divider connected to A0
const float VOLTAGE_DIVIDER_RATIO = 1.75; // Calibrated ratio based on actual ADC voltage
const float REFERENCE_VOLTAGE = 3.3; // ESP32C3 reference voltage

// Battery voltage thresholds
const float BATTERY_FULL = 4.2;    // Fully charged
const float BATTERY_GOOD = 3.7;    // Good charge
const float BATTERY_LOW = 3.3;     // Low battery
const float BATTERY_CRITICAL = 3.0; // Critical low

// Button debouncing variables
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;    // Debounce time in milliseconds
int lastButtonState = LOW;           // Previous button state
int buttonState = LOW;               // Current button state
int reading = LOW;                   // Raw reading from button

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLDOWN); // Enable internal pull-down resistor
  pinMode(BATTERY_PIN, INPUT);
  
  // Initialize sleep functionality
  initSleep(BUTTON_PIN);
  
  // Initialize WiFi
  initWiFi();
  setupWebServer();
  
  // Print initial state
  Serial.println("System initialized");
  Serial.println("Press button to read battery voltage");
  Serial.println("Device will sleep after 30 seconds of inactivity");
}

// Function to get battery status
String getBatteryStatus(float voltage) {
  if (voltage >= BATTERY_FULL) return "FULL";
  if (voltage >= BATTERY_GOOD) return "GOOD";
  if (voltage >= BATTERY_LOW) return "LOW";
  if (voltage >= BATTERY_CRITICAL) return "CRITICAL";
  return "DANGER";
}

void loop() {
  // Handle web server requests
  handleWebServer();
  
  // Check if it's time to sleep
  if (shouldSleep()) {
    goToSleep();
  }
  
  // Read the current button state
  reading = digitalRead(BUTTON_PIN);
  
  // Check if the button state has changed
  if (reading != lastButtonState) {
    lastDebounceTime = millis();  // Reset the debounce timer
  }
  
  // If the button state has been stable for longer than the debounce delay
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // If the button state has changed
    if (reading != buttonState) {
      buttonState = reading;
      
      // If the button is pressed (HIGH)
      if (buttonState == HIGH) {
        // Reset sleep timer
        resetSleepTimer();
        
        // Read battery voltage multiple times and average
        float totalVoltage = 0;
        const int numReadings = 5;
        
        for(int i = 0; i < numReadings; i++) {
          int rawValue = analogRead(BATTERY_PIN);
          float voltage = (rawValue * REFERENCE_VOLTAGE * VOLTAGE_DIVIDER_RATIO) / 4095.0;
          totalVoltage += voltage;
          
          // Print raw ADC value for debugging
          Serial.print("Raw ADC value: ");
          Serial.println(rawValue);
          
          delay(10); // Small delay between readings
        }
        
        float averageVoltage = totalVoltage / numReadings;
        String status = getBatteryStatus(averageVoltage);
        
        // Print battery voltage and status
        Serial.print("Battery Voltage: ");
        Serial.print(averageVoltage);
        Serial.print("V (");
        Serial.print(status);
        Serial.println(")");
        
        // Update web server with new reading
        updateBatteryStatus(averageVoltage, status);
      }
    }
  }
  
  // Save the current button state for the next comparison
  lastButtonState = reading;
  
  delay(10); // Small delay to prevent CPU hogging
} 