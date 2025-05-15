#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>

// WiFi credentials
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

class WiFiManager {
private:
    bool enabled;
    bool connected;
    bool initialized;
    bool connecting;
    unsigned long lastConnectionAttempt;
    unsigned long lastStatusCheck;
    const unsigned long connectionTimeout = 20000;    // 20 seconds timeout
    const unsigned long reconnectInterval = 5000;     // Try to reconnect every 5 seconds
    const unsigned long statusCheckInterval = 1000;   // Check status every second
    
public:
    WiFiManager() : enabled(false), connected(false), initialized(false), connecting(false), 
                   lastConnectionAttempt(0), lastStatusCheck(0) {}
    
    void begin() {
        if (!enabled) return;
        if (initialized) return;  // Prevent multiple initializations
        
        // Initialize Serial1 for hardware serial communication
        Serial1.begin(115200, SERIAL_8N1, -1, 10);  // RX pin -1 (not used), TX pin 10
        delay(100); // Give serial time to initialize
        
        Serial1.println("WiFiManager: begin: Starting...");
        
        // Set WiFi to station mode
        WiFi.mode(WIFI_STA);
        
        initialized = true;
        
        // Initial connection attempt
        Serial1.println("WiFiManager: begin: Initial connection attempt");
        startConnection();
    }
    
    void update() {
        if (!enabled || !initialized) return;
        
        // Only check status periodically
        if (millis() - lastStatusCheck < statusCheckInterval) {
            return;
        }
        lastStatusCheck = millis();
        
        // Check current connection status
        if (WiFi.status() == WL_CONNECTED) {
            if (!connected) {
                connected = true;
                connecting = false;
                Serial1.println("\nWiFiManager: update: Connected to WiFi!");
                Serial1.print("WiFiManager: update: IP address: ");
                Serial1.println(WiFi.localIP());
            }
        } else {
            if (connected) {
                connected = false;
                Serial1.println("WiFiManager: update: WiFi connection lost!");
            }
            
            // Check for connection timeout
            if (connecting && (millis() - lastConnectionAttempt > connectionTimeout)) {
                Serial1.println("WiFiManager: update: Connection attempt timed out");
                connecting = false;
                WiFi.disconnect();
            }
            
            // Only attempt reconnection if we're not connected and not already connecting
            if (!connected && !connecting && (millis() - lastConnectionAttempt > reconnectInterval)) {
                Serial1.println("WiFiManager: update: Attempting reconnection");
                startConnection();
            }
        }
    }
    
    void startConnection() {
        if (WiFi.status() == WL_CONNECTED) {
            connected = true;
            connecting = false;
            Serial1.println("WiFiManager: connect: Already connected");
            return;
        }
        
        lastConnectionAttempt = millis();
        connecting = true;
        Serial1.println("WiFiManager: connect: Starting connection...");
        
        // Start connection attempt
        WiFi.disconnect();
        delay(100);
        WiFi.begin(ssid, password);
    }
    
    void enable() {
        if (enabled) return;  // Prevent multiple enables
        enabled = true;
        Serial1.println("WiFiManager: enable: Enabled");
    }
    
    void disable() {
        enabled = false;
        if (connected || connecting) {
            WiFi.disconnect();
            connected = false;
            connecting = false;
            Serial1.println("WiFiManager: disable: Disabled and disconnected");
        }
    }
    
    bool isEnabled() {
        return enabled;
    }
    
    bool isConnected() {
        return connected;
    }
    
    IPAddress getIP() {
        return WiFi.localIP();
    }
    
    int getRSSI() {
        return WiFi.RSSI();
    }
};

#endif // WIFI_MANAGER_H 