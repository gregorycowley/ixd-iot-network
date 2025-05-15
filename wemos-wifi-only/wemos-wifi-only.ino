#include "WiFiManager.h"

// WiFi credentials
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Create WiFi manager instance
WiFiManager wifiManager(ssid, password);

void setup() {
    // Initialize Serial communication with a delay to ensure proper initialization
    Serial.begin(115200);
    delay(1000);  // Give time for serial to stabilize
    
    // Clear any garbage in the serial buffer
    while(Serial.available()) {
        Serial.read();
    }
    
    Serial.println("\n\nWemos D1 WiFi Connection Test");
    Serial.println("-----------------------------");
    
    // Print MAC address
    Serial.print("MAC Address: ");
    Serial.println(WiFi.macAddress());
    Serial.println();

    // Initialize WiFi
    wifiManager.begin();
}

void loop() {
    // Update WiFi connection status
    wifiManager.update();
    
    delay(1000);
}
