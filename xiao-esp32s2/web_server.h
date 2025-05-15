#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <WebServer.h>
#include <ESPmDNS.h>
#include "config_local.h"

// Create web server instance
WebServer server(80);

// Variables to store button press information
struct ButtonPress {
  unsigned long timestamp;
  bool isPressed;
};

ButtonPress lastButtonPress = {0, false};

// Function to format timestamp
String getFormattedTime(unsigned long timestamp) {
  unsigned long seconds = timestamp / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;
  
  seconds = seconds % 60;
  minutes = minutes % 60;
  
  char timeStr[20];
  snprintf(timeStr, sizeof(timeStr), "%02lu:%02lu:%02lu", hours, minutes, seconds);
  return String(timeStr);
}

// Function to handle root path
void handleRoot() {
  String html = "<!DOCTYPE html>";
  html += "<html>";
  html += "<head>";
  html += "<title>Button Status</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; margin: 20px; }";
  html += ".status { padding: 20px; border-radius: 5px; margin: 10px 0; }";
  html += ".pressed { background-color: #ffcdd2; }";
  html += ".released { background-color: #c8e6c9; }";
  html += "</style>";
  html += "</head>";
  html += "<body>";
  html += "<h1>Button Status</h1>";
  
  if (lastButtonPress.timestamp > 0) {
    html += "<div class='status " + String(lastButtonPress.isPressed ? "pressed" : "released") + "'>";
    html += "<h2>Last Button " + String(lastButtonPress.isPressed ? "Press" : "Release") + "</h2>";
    html += "<p>Time: " + getFormattedTime(lastButtonPress.timestamp) + "</p>";
    html += "</div>";
  } else {
    html += "<p>No button activity recorded yet.</p>";
  }
  
  html += "<p><small>Device ID: " + String(buttonSeries) + " - " + String(buttonName) + "</small></p>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

// Function to setup web server
void setupWebServer() {
  // Set up mDNS
  if (MDNS.begin("xiao-esp32c3")) {
    Serial.println("mDNS responder started");
    Serial.println("You can now access the device at http://xiao-esp32c3.local");
  }
  
  // Set up web server routes
  server.on("/", handleRoot);
  
  // Start web server
  server.begin();
  Serial.println("HTTP server started");
}

// Function to handle web server
void handleWebServer() {
  server.handleClient();
}

// Function to update button press information
void updateButtonPress(bool isPressed) {
  lastButtonPress.timestamp = millis();
  lastButtonPress.isPressed = isPressed;
}

#endif // WEB_SERVER_H 