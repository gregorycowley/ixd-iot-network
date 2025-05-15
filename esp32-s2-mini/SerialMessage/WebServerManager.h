#ifndef WEB_SERVER_MANAGER_H
#define WEB_SERVER_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <WebServer.h>
#include <HTTP_Method.h>
#include "BatteryManager.h"

// HTML template for the status page - stored in PROGMEM to save RAM
static const char PROGMEM HTML_TEMPLATE[] = R"(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32-S2 Status</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: Arial; margin: 20px; }
        .card { background: #f0f0f0; padding: 20px; border-radius: 10px; margin: 10px 0; }
        .status { font-weight: bold; }
        .online { color: green; }
        .offline { color: red; }
        .low { color: orange; }
        .critical { color: red; }
    </style>
    <script>
        function updateStatus() {
            fetch('/status')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('uptime').textContent = data.uptime;
                    document.getElementById('wifi-status').textContent = data.wifiStatus;
                    document.getElementById('wifi-status').className = 
                        data.wifiStatus === 'Connected' ? 'status online' : 'status offline';
                    document.getElementById('ip-address').textContent = data.ipAddress;
                    document.getElementById('rssi').textContent = data.rssi + ' dBm';
                    document.getElementById('free-heap').textContent = data.freeHeap + ' bytes';
                    document.getElementById('battery-voltage').textContent = data.batteryVoltage + 'V';
                    document.getElementById('battery-percentage').textContent = data.batteryPercentage + '%';
                    document.getElementById('battery-percentage').className = 
                        data.batteryPercentage <= 10 ? 'status critical' :
                        data.batteryPercentage <= 20 ? 'status low' : 'status';
                });
        }
        setInterval(updateStatus, 1000);
    </script>
</head>
<body>
    <h1>ESP32-S2 Status</h1>
    <div class="card">
        <h2>System Information</h2>
        <p>Uptime: <span id="uptime">--:--:--</span></p>
        <p>Free Heap: <span id="free-heap">--</span></p>
    </div>
    <div class="card">
        <h2>Battery Status</h2>
        <p>Voltage: <span id="battery-voltage">--</span></p>
        <p>Level: <span id="battery-percentage" class="status">--</span></p>
    </div>
    <div class="card">
        <h2>WiFi Status</h2>
        <p>Status: <span id="wifi-status" class="status">Disconnected</span></p>
        <p>IP Address: <span id="ip-address">--</span></p>
        <p>Signal Strength: <span id="rssi">--</span></p>
    </div>
</body>
</html>
)";

class WebServerManager {
private:
    bool enabled;
    bool initialized;
    WebServer* server;
    unsigned long lastUpdate;
    const unsigned long updateInterval = 1000; // Update every second
    BatteryManager* batteryManager;  // Reference to battery manager
    
    void handleRoot() {
        char html[4096];  // Buffer for HTML content
        memcpy_P(html, HTML_TEMPLATE, sizeof(HTML_TEMPLATE));
        server->send(200, "text/html", html);
    }
    
    void handleStatus() {
        String json = "{";
        json += "\"uptime\":\"" + String(millis() / 1000) + "\",";
        json += "\"wifiStatus\":\"" + String(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected") + "\",";
        json += "\"ipAddress\":\"" + WiFi.localIP().toString() + "\",";
        json += "\"rssi\":" + String(WiFi.RSSI()) + ",";
        json += "\"freeHeap\":" + String(ESP.getFreeHeap());
        
        // Add battery information if available
        if (batteryManager != nullptr && batteryManager->isEnabled()) {
            json += ",\"batteryVoltage\":" + String(batteryManager->getVoltage());
            json += ",\"batteryPercentage\":" + String(batteryManager->getPercentage());
        }
        
        json += "}";
        server->send(200, "application/json", json);
    }
    
    void handleNotFound() {
        server->send(404, "text/plain", "Not found");
    }

public:
    WebServerManager() : enabled(false), initialized(false), server(nullptr), batteryManager(nullptr) {}
    
    ~WebServerManager() {
        if (server != nullptr) {
            server->stop();
            delete server;
            server = nullptr;
        }
    }
    
    void setBatteryManager(BatteryManager* manager) {
        batteryManager = manager;
    }
    
    void begin() {
        if (!enabled || initialized) return;
        
        Serial1.print("WebServer: begin: Free heap before server creation: ");
        Serial1.println(ESP.getFreeHeap());
        
        // Create server instance
        server = new WebServer(80);
        if (server == nullptr) {
            Serial1.println("WebServer: begin: Failed to create server instance");
            return;
        }
        
        // Set up routes
        server->on("/", HTTP_GET, [this]() { handleRoot(); });
        server->on("/status", HTTP_GET, [this]() { handleStatus(); });
        server->onNotFound([this]() { handleNotFound(); });
        
        // Start server
        server->begin();
        initialized = true;
        
        Serial1.print("WebServer: begin: Free heap after server creation: ");
        Serial1.println(ESP.getFreeHeap());
        Serial1.println("WebServer: begin: HTTP server started");
    }
    
    void update() {
        if (!enabled || !initialized) return;
        server->handleClient();
    }
    
    void enable() {
        if (enabled) return;
        enabled = true;
        Serial1.println("WebServer: enable: Enabled");
    }
    
    void disable() {
        if (!enabled) return;
        enabled = false;
        if (server != nullptr) {
            server->stop();
            delete server;
            server = nullptr;
        }
        initialized = false;
        Serial1.println("WebServer: disable: Disabled");
    }
    
    bool isEnabled() {
        return enabled;
    }
};

#endif // WEB_SERVER_MANAGER_H 