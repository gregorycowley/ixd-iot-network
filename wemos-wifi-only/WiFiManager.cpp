#include "WiFiManager.h"

WiFiManager::WiFiManager(const char* ssid, const char* password) 
    : _ssid(ssid), _password(password), _enabled(WIFI_ENABLED) {
}

void WiFiManager::begin() {
    if (!_enabled) {
        Serial.println("WiFi functionality is disabled");
        return;
    }

    // Set WiFi mode and configuration
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    // Configure WiFi settings
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
    
    // Try to connect
    connect();
}

void WiFiManager::update() {
    if (!_enabled) return;
    
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("\nWiFi connection lost. Attempting to reconnect...");
        printWiFiStatus();
        connect();
    }
}

bool WiFiManager::isConnected() {
    return _enabled && WiFi.status() == WL_CONNECTED;
}

void WiFiManager::printStatus() {
    if (!_enabled) {
        Serial.println("WiFi functionality is disabled");
        return;
    }
    printWiFiStatus();
}

void WiFiManager::scanNetworks() {
    if (!_enabled) {
        Serial.println("WiFi functionality is disabled");
        return;
    }

    Serial.println("Scanning for networks...");
    int n = WiFi.scanNetworks();
    if (n == 0) {
        Serial.println("No networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i) {
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.print(" Channel: ");
            Serial.print(WiFi.channel(i));
            Serial.print(" Encryption: ");
            Serial.println(WiFi.encryptionType(i));
            delay(10);
        }
    }
}

void WiFiManager::printWiFiStatus() {
    switch(WiFi.status()) {
        case WL_IDLE_STATUS:
            Serial.println("WiFi idle");
            break;
        case WL_NO_SSID_AVAIL:
            Serial.println("No SSID available");
            break;
        case WL_SCAN_COMPLETED:
            Serial.println("Scan completed");
            break;
        case WL_CONNECTED:
            Serial.println("Connected");
            break;
        case WL_CONNECT_FAILED:
            Serial.println("Connection failed - Authentication failed");
            break;
        case WL_CONNECTION_LOST:
            Serial.println("Connection lost");
            break;
        case WL_DISCONNECTED:
            Serial.println("Disconnected");
            break;
        case WL_WRONG_PASSWORD:
            Serial.println("Wrong password");
            break;
        default:
            Serial.print("Unknown status: ");
            Serial.println(WiFi.status());
    }
}

bool WiFiManager::connect() {
    if (!_enabled) return false;

    Serial.print("Connecting to ");
    Serial.println(_ssid);

    // Try multiple connection attempts
    int maxAttempts = 3;
    for (int attempt = 0; attempt < maxAttempts; attempt++) {
        Serial.print("\nConnection attempt ");
        Serial.println(attempt + 1);
        
        WiFi.begin(_ssid, _password);
        
        // Wait for connection with timeout
        int connectionAttempts = 0;
        while (WiFi.status() != WL_CONNECTED && connectionAttempts < 30) {
            delay(500);
            Serial.print(".");
            connectionAttempts++;
        }
        Serial.println();

        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("WiFi connected");
            Serial.print("IP address: ");
            Serial.println(WiFi.localIP());
            Serial.print("Signal strength (RSSI): ");
            Serial.println(WiFi.RSSI());
            Serial.print("Channel: ");
            Serial.println(WiFi.channel());
            return true;
        } else {
            Serial.print("Connection failed! Status: ");
            printWiFiStatus();
            
            if (attempt < maxAttempts - 1) {
                Serial.println("Retrying...");
                WiFi.disconnect();
                delay(1000);
            }
        }
    }

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("\nAll connection attempts failed");
        scanNetworks();
    }
    
    return false;
} 