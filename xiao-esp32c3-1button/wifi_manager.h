#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <WebServer.h>

// WiFi credentials
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Web server
WebServer server(80);

// Store the latest reading
static float lastVoltage = 0;
static String lastStatus = "";

// Function to initialize WiFi
void initWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nConnected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// Function to handle root request
void handleRoot() {
  String html = "<html><body>";
  html += "<h1>XIAO ESP32C3 Battery Monitor</h1>";
  html += "<p>Battery Voltage: ";
  html += String(lastVoltage, 2);
  html += "V</p>";
  html += "<p>Battery Status: ";
  html += lastStatus;
  html += "</p>";
  html += "<p><a href='/status'>Get JSON Status</a></p>";
  html += "<p><a href='/'>Refresh Page</a></p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

// Function to handle status request
void handleStatus() {
  String status = "{\"voltage\":";
  status += String(lastVoltage, 2);
  status += ",\"status\":\"";
  status += lastStatus;
  status += "\"}";
  server.send(200, "application/json", status);
}

// Function to setup web server
void setupWebServer() {
  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.begin();
  Serial.println("HTTP server started");
}

// Function to update battery status
void updateBatteryStatus(float voltage, String status) {
  lastVoltage = voltage;
  lastStatus = status;
}

// Function to handle web server
void handleWebServer() {
  server.handleClient();
}

#endif // WIFI_MANAGER_H 