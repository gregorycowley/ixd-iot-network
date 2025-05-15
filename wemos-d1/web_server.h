#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "battery_monitor.h"

// Web server port
const int WEB_PORT = 80;

// Global variables for device status
struct DeviceStatus {
    unsigned long lastButtonPressTime;
    String lastButtonPressed;
    float batteryVoltage;
    float batteryPercentage;
    bool isConnected;
} deviceStatus;

ESP8266WebServer server(WEB_PORT);

// Function to update device status
void updateDeviceStatus(const String& buttonName) {
    deviceStatus.lastButtonPressTime = millis();
    deviceStatus.lastButtonPressed = buttonName;
    deviceStatus.batteryVoltage = readBatteryVoltage();
    deviceStatus.batteryPercentage = getBatteryPercentage();
    deviceStatus.isConnected = true;
}

// Handle root path
void handleRoot() {
    String html = "<html><head>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
    html += "<meta http-equiv='refresh' content='5'>";  // Auto refresh every 5 seconds
    html += "<style>";
    html += "body { font-family: Arial; margin: 20px; }";
    html += ".status { background: #f0f0f0; padding: 10px; border-radius: 5px; margin: 10px 0; }";
    html += ".battery { color: #0066cc; }";
    html += ".button { color: #006600; }";
    html += "</style></head><body>";
    html += "<h1>Device Status</h1>";
    
    html += "<div class='status'>";
    html += "<h2 class='battery'>Battery Status</h2>";
    html += "<p>Voltage: " + String(deviceStatus.batteryVoltage, 2) + "V</p>";
    html += "<p>Percentage: " + String(deviceStatus.batteryPercentage, 1) + "%</p>";
    html += "</div>";
    
    html += "<div class='status'>";
    html += "<h2 class='button'>Last Button Press</h2>";
    html += "<p>Button: " + deviceStatus.lastButtonPressed + "</p>";
    html += "<p>Time: " + String(deviceStatus.lastButtonPressTime / 1000) + " seconds ago</p>";
    html += "</div>";
    
    html += "</body></html>";
    server.send(200, "text/html", html);
}

// Handle status API endpoint
void handleStatus() {
    JsonDocument doc;
    
    doc["lastButtonPressTime"] = deviceStatus.lastButtonPressTime;
    doc["lastButtonPressed"] = deviceStatus.lastButtonPressed;
    doc["batteryVoltage"] = deviceStatus.batteryVoltage;
    doc["batteryPercentage"] = deviceStatus.batteryPercentage;
    doc["isConnected"] = deviceStatus.isConnected;
    
    String jsonString;
    serializeJson(doc, jsonString);
    server.send(200, "application/json", jsonString);
}

// Initialize web server
void initWebServer() {
    server.on("/", handleRoot);
    server.on("/status", handleStatus);
    server.begin();
    Serial.println("Web server started");
}

// Update web server
void updateWebServer() {
    server.handleClient();
}

#endif // WEB_SERVER_H 