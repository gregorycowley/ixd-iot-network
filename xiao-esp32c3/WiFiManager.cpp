#include "WiFiManager.h"

WiFiManager::WiFiManager() : _connected(false) {
}

bool WiFiManager::connect() {
    Serial.println("Attempting to connect to WiFi...");
    
    WiFi.begin(PRIMARY_SSID, PRIMARY_PASSWORD);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < CONNECTION_TIMEOUT) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConnected to WiFi!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        _connected = true;
        return true;
    } else {
        Serial.println("\nFailed to connect to network");
        _connected = false;
        return false;
    }
}

bool WiFiManager::isConnected() {
    return _connected;
}

void WiFiManager::checkConnection() {
    if (WiFi.status() != WL_CONNECTED && _connected) {
        Serial.println("WiFi connection lost. Attempting to reconnect...");
        _connected = false;
        connect();
    }
}

String WiFiManager::getIPAddress() {
    return WiFi.localIP().toString();
} 