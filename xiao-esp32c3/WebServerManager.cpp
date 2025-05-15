#include "WebServerManager.h"
#include <ArduinoJson.h>
#include <WiFi.h>

WebServerManager::WebServerManager() : _server(WEB_SERVER_PORT), _ledState(false) {
}

void WebServerManager::begin() {
    _server.on("/", HTTP_GET, [this]() { handleRoot(); });
    _server.onNotFound([this]() { handleNotFound(); });
    
    _server.begin();
    Serial.println("HTTP server started");
}

void WebServerManager::handleClient() {
    _server.handleClient();
}

void WebServerManager::stop() {
    _server.stop();
}

void WebServerManager::setLEDState(bool state) {
    _ledState = state;
}

void WebServerManager::handleRoot() {
    String response = getStatusJSON();
    _server.send(200, "application/json", response);
}

void WebServerManager::handleNotFound() {
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += _server.uri();
    message += "\nMethod: ";
    message += (_server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += _server.args();
    message += "\n";
    
    for (uint8_t i = 0; i < _server.args(); i++) {
        message += " " + _server.argName(i) + ": " + _server.arg(i) + "\n";
    }
    
    _server.send(404, "text/plain", message);
}

String WebServerManager::getStatusJSON() {
    JsonDocument doc;
    
    doc["device_id"] = DEVICE_ID;
    doc["series_id"] = SERIES_ID;
    doc["status"] = "online";
    doc["ip"] = WiFi.localIP().toString();
    doc["led_state"] = _ledState;
    
    String response;
    serializeJson(doc, response);
    return response;
} 