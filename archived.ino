#include "WiFi.h"

// WiFi credentials
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Button and LED pins
const int BUTTON_PIN = D10;  // Button with pulldown resistor
const int LED_PIN = D9;      // LED output

void setup() {
  // Initialize serial communication at 9600 baud
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect
  }
  
  // Initialize button and LED pins
  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  
  // Start with LED off
  digitalWrite(LED_PIN, LOW);
  
  Serial.println("Initializing XIAO ESP32C3...");
  
  // Initialize WiFi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  Serial.println("WiFi initialized");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("");
    Serial.println("Failed to connect to WiFi");
  }
}

void loop() {
  // Check WiFi connection status
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost. Attempting to reconnect...");
    WiFi.reconnect();
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      delay(500);
      Serial.print(".");
      attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Reconnected!");
    } else {
      Serial.println("Reconnection failed");
    }
  }

  // Check button state
  if (digitalRead(BUTTON_PIN) == HIGH) {
    Serial.println("Button pressed!");
    
    // Flash the LED 3 times
    for (int i = 0; i < 3; i++) {
      digitalWrite(LED_PIN, HIGH);
      delay(200);
      digitalWrite(LED_PIN, LOW);
      delay(200);
    }
    
    // Wait for button release to prevent multiple triggers
    while (digitalRead(BUTTON_PIN) == HIGH) {
      delay(10);
    }
  }
  
  // Add a small delay to prevent overwhelming the serial monitor
  delay(10);
}

