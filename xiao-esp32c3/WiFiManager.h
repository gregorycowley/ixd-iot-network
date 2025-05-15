#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include "Config_device.h"

class WiFiManager {
public:
    WiFiManager();
    bool connect();
    bool isConnected();
    void checkConnection();
    String getIPAddress();

private:
    bool _connected;
    static const int CONNECTION_TIMEOUT = 20; // 10 seconds (20 * 500ms)
};

#endif // WIFI_MANAGER_H 