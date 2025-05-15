#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <ESP8266WiFi.h>

// Configuration
#define WIFI_ENABLED true  // Set to false to disable WiFi functionality

class WiFiManager {
public:
    WiFiManager(const char* ssid, const char* password);
    
    void begin();
    void update();
    bool isConnected();
    void printStatus();
    void scanNetworks();
    
private:
    const char* _ssid;
    const char* _password;
    bool _enabled;
    
    void printWiFiStatus();
    bool connect();
};

#endif // WIFI_MANAGER_H 