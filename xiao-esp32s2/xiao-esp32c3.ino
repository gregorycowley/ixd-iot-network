#include <Adafruit_NeoPixel.h>
#include <esp_system.h>  // Required for esp_read_efuse_mac
#include <esp_sleep.h>   // Required for sleep mode
#include "pin_definitions.h"
#include "wifi_setup.h"
#include "mqtt_handler.h"
#include "web_server.h"
#include "battery_monitor.h"

// Create NeoPixel object
Adafruit_NeoPixel pixels(numPixels, neoPixelPin, NEO_GRB + NEO_KHZ800);

// Variable to store the button state
int buttonState = 0;
int lastButtonState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;  // Debounce time in milliseconds

// Variable to track color state
int colorState = 0;
// Variable to track last color change time
unsigned long lastColorChange = 0;

// Variable to track last activity time
unsigned long lastActivityTime = 0;
const unsigned long SLEEP_TIMEOUT = 300000;  // 5 minutes in milliseconds

// Variable to track last battery check time
unsigned long lastBatteryCheck = 0;
const unsigned long BATTERY_CHECK_INTERVAL = 60000;  // Check battery every minute

void setup() {
  // Initialize serial communication at 115200 baud rate
  Serial.begin(115200);
  Serial.println("\n\n=== Xiao ESP32 Device Setup ===");
  
  // Initialize MQTT topics
  initMQTTTopics();
  
  // Print board information
  Serial.print("Board: ");
  Serial.println(BOARD_TYPE);
  
  // Print pin configuration
  Serial.println("\nPin Configuration:");
  Serial.printf("Button Pin: %d\n", buttonPin);
  if (ledPin != -1) {
    Serial.printf("LED Pin: %d\n", ledPin);
  }
  Serial.printf("NeoPixel Pin: %d\n", neoPixelPin);
  if (batteryPin != -1) {
    Serial.printf("Battery Pin: %d\n", batteryPin);
  }
  Serial.printf("Number of Pixels: %d\n", numPixels);
  Serial.printf("Brightness: %d\n", brightness);
  
  // Set the button pin as input (using internal pull-down resistor)
  pinMode(buttonPin, INPUT_PULLDOWN);
  Serial.println("Button pin set as INPUT_PULLDOWN");
  
  // Set the LED pin as output if available
  if (ledPin != -1) {
    pinMode(ledPin, OUTPUT);
    Serial.println("LED pin set as OUTPUT");
  }
  
  // Initialize NeoPixel
  pixels.begin();
  pixels.setBrightness(brightness);  // Set the global brightness
  pixels.show(); // Initialize all pixels to 'off'
  Serial.println("NeoPixel initialized");
  
  // Setup battery monitoring if available
  if (batteryPin != -1) {
    setupBatteryMonitor();
    Serial.println("Battery monitoring initialized");
  }
  
  // Print MAC address
  Serial.print("\nMAC Address: ");
  Serial.println(WiFi.macAddress());
  
  // Scan for available WiFi networks
  scanWiFiNetworks();
  
  // Setup WiFi connection
  Serial.println("\nSetting up WiFi...");
  setupWiFi();
  
  // Setup MQTT
  Serial.println("\nSetting up MQTT...");
  setupMQTT();
  
  // Setup Web Server
  Serial.println("\nSetting up Web Server...");
  setupWebServer();
  
  Serial.println("\n=== Setup Complete ===");
  Serial.println("Device is ready!");

  // Initialize last activity time
  lastActivityTime = millis();
  lastBatteryCheck = millis();
}

void enterSleepMode() {
  Serial.println("Entering sleep mode...");
  
  // Turn off NeoPixel
  pixels.clear();
  pixels.show();
  
  // Turn off LED
  if (ledPin != -1) {
    digitalWrite(ledPin, LOW);
  }
  
  // Disconnect WiFi
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  
  // Configure wake-up source (button)
  gpio_wakeup_enable((gpio_num_t)buttonPin, GPIO_INTR_HIGH_LEVEL);
  esp_sleep_enable_gpio_wakeup();
  
  // Enter deep sleep
  esp_deep_sleep_start();
}

void loop() {
  // Check if it's time to enter sleep mode
  if (millis() - lastActivityTime > SLEEP_TIMEOUT) {
    enterSleepMode();
  }

  // Check battery status if available
  if (batteryPin != -1 && millis() - lastBatteryCheck >= BATTERY_CHECK_INTERVAL) {
    updateBatteryStatus();
    lastBatteryCheck = millis();
    
    // If battery is low, blink NeoPixel red
    if (batteryStatus.isLow) {
      pixels.setPixelColor(0, pixels.Color(255, 0, 0));
      pixels.show();
      delay(100);
      pixels.clear();
      pixels.show();
    }
  }

  // Check WiFi connection and reconnect if necessary
  reconnectWiFi();
  
  // Handle MQTT connection and messages
  mqttLoop();

  // Handle web server
  handleWebServer();

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
        Serial.println("Button pressed!");
        // Publish button press event
        publishMessage("PRESSED");
        // Update button press information for web server
        updateButtonPress(true);
        // Update last activity time
        lastActivityTime = millis();
      } else {
        Serial.println("Button released!");
        // Publish button release event
        publishMessage("RELEASED");
        // Update button press information for web server
        updateButtonPress(false);
        // Update last activity time
        lastActivityTime = millis();
      }
    }
  }

  // Save the reading for next time
  lastButtonState = reading;

  // Check if it's time to change the NeoPixel color
  if (millis() - lastColorChange >= 2000) {  // 2000ms = 2 seconds
    lastColorChange = millis();
    
    // Only change colors if battery is not low
    if (batteryPin == -1 || !batteryStatus.isLow) {
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
    // Update last activity time
    lastActivityTime = millis();
  }
}