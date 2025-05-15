#include <Adafruit_NeoPixel.h>
#include "pin_definitions.h"
#include "wifi_setup.h"
#include "mqtt_handler.h"

// Create NeoPixel object
Adafruit_NeoPixel pixels(numPixels, neoPixelPin, NEO_GRB + NEO_KHZ800);

// Variable to store the button state
int buttonState = 0;
// Variable to track last button press time
unsigned long lastButtonPress = 0;

void setup() {
  // Initialize serial communication at 115200 baud rate
  Serial.begin(115200);
  Serial.println("\n\n=== Basic D1 Device Setup ===");
  
  // Print board information
  #ifdef ESP32
    Serial.println("Board: ESP32");
  #else
    Serial.println("Board: ESP8266");
  #endif
  
  // Print pin configuration
  Serial.println("\nPin Configuration:");
  Serial.printf("Button Pin: %d\n", buttonPin);
  Serial.printf("LED Pin: %d\n", ledPin);
  Serial.printf("NeoPixel Pin: %d\n", neoPixelPin);
  Serial.printf("Number of Pixels: %d\n", numPixels);
  Serial.printf("Brightness: %d\n", brightness);
  
  // Set the button pin as input (using external pull-down resistor)
  pinMode(buttonPin, INPUT);
  Serial.println("Button pin set as INPUT");
  
  // Set the LED pin as output
  pinMode(ledPin, OUTPUT);
  Serial.println("LED pin set as OUTPUT");
  
  // Initialize NeoPixel
  pixels.begin();
  pixels.setBrightness(brightness);  // Set the global brightness
  pixels.show(); // Initialize all pixels to 'off'
  Serial.println("NeoPixel initialized");
  
  // Print MAC address
  Serial.print("\nMAC Address: ");
  #ifdef ESP32
    uint8_t mac[6];
    esp_read_efuse_mac(mac);
    for (int i = 0; i < 6; i++) {
      Serial.printf("%02X", mac[i]);
      if (i < 5) Serial.print(":");
    }
  #else
    Serial.println(WiFi.macAddress());
  #endif
  Serial.println();
  
  // Scan for available WiFi networks
  scanWiFiNetworks();
  
  // Setup WiFi connection
  Serial.println("\nSetting up WiFi...");
  setupWiFi();
  
  // Setup MQTT
  Serial.println("\nSetting up MQTT...");
  setupMQTT();
  
  Serial.println("\n=== Setup Complete ===");
  Serial.println("Device is ready!");
}

void loop() {
  // Check WiFi connection and reconnect if necessary
  reconnectWiFi();
  
  // Handle MQTT connection and messages
  mqttLoop();

  // Read the state of the button
  buttonState = digitalRead(buttonPin);
  
  // With pull-down resistor, the button will read HIGH when pressed
  if (buttonState == HIGH && (millis() - lastButtonPress > 200)) {  // Debounce check
    lastButtonPress = millis();
    Serial.println("Button pressed!");
    // Publish button press event
    publishMessage("PRESSED");
    // Add a small delay to prevent multiple readings
    delay(100);
  }
}