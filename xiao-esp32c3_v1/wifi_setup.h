#ifndef WIFI_SETUP_H
#define WIFI_SETUP_H

#include <WiFi.h>
#include "config_local.h"

// WiFi connection parameters
const int MAX_RECONNECT_ATTEMPTS = 3;
const int WIFI_RECONNECT_DELAY = 5000; // 5 seconds
unsigned long lastReconnectAttempt = 0;
int reconnectAttempts = 0;

// Function to get WiFi status as string
String getWiFiStatus(int status) {
  switch (status) {
    case WL_IDLE_STATUS: return "Idle";
    case WL_NO_SSID_AVAIL: return "No SSID Available";
    case WL_SCAN_COMPLETED: return "Scan Completed";
    case WL_CONNECTED: return "Connected";
    case WL_CONNECT_FAILED: return "Connection Failed";
    case WL_CONNECTION_LOST: return "Connection Lost";
    case WL_DISCONNECTED: return "Disconnected";
    default: return "Unknown Status";
  }
}

// Function to scan for available WiFi networks
void scanWiFiNetworks() {
  int n = WiFi.scanNetworks();
  if (n > 0) {
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < wifiCredentialsCount; j++) {
        if (WiFi.SSID(i) == wifiCredentials[j].ssid) {
          Serial.print("Found network: ");
          Serial.println(wifiCredentials[j].ssid);
          break;
        }
      }
    }
  }
}

// Function to perform a complete WiFi reset
void resetWiFi() {
  WiFi.disconnect(true);
  delay(1000);
  WiFi.mode(WIFI_OFF);
  delay(1000);
  reconnectAttempts = 0;
  WiFi.mode(WIFI_STA);
  delay(100);
}

// Function to check if WiFi connection is stable
bool isWiFiStable() {
  static unsigned long lastCheck = 0;
  static int stableCount = 0;
  const int requiredStableChecks = 3;
  const int checkInterval = 1000;
  
  unsigned long now = millis();
  if (now - lastCheck >= checkInterval) {
    lastCheck = now;
    if (WiFi.status() == WL_CONNECTED) {
      stableCount++;
      if (stableCount >= requiredStableChecks) {
        return true;
      }
    } else {
      stableCount = 0;
    }
  }
  return false;
}

// Function to test network connectivity
bool testNetworkConnectivity() {
  WiFiClient testClient;
  
  if (testClient.connect(WiFi.gatewayIP(), 80)) {
    testClient.stop();
  } else {
    return false;
  }
  
  if (testClient.connect(mqtt_server, mqtt_port)) {
    testClient.stop();
    return true;
  }
  return false;
}

// Function to connect to WiFi
void setupWiFi() {
  resetWiFi();
  int n = WiFi.scanNetworks();
  
  if (n == 0) return;
  
  bool foundNetworks[wifiCredentialsCount] = {false};
  
  for (int i = 0; i < n; ++i) {
    String scannedSSID = WiFi.SSID(i);
    for (int j = 0; j < wifiCredentialsCount; j++) {
      if (scannedSSID == wifiCredentials[j].ssid) {
        foundNetworks[j] = true;
        break;
      }
    }
  }
  
  bool connected = false;
  for (int i = 0; i < wifiCredentialsCount && !connected; i++) {
    if (foundNetworks[i]) {
      WiFi.begin(wifiCredentials[i].ssid, wifiCredentials[i].password);
      
      int attempts = 0;
      const int maxAttempts = 20;
      
      while (WiFi.status() != WL_CONNECTED && attempts < maxAttempts) {
        delay(500);
        attempts++;
      }
      
      if (WiFi.status() == WL_CONNECTED) {
        connected = true;
        break;
      }
    }
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Connected to ");
    Serial.print(WiFi.SSID());
    Serial.print(" (");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm)");
    
    int stabilityAttempts = 0;
    const int maxStabilityAttempts = 30;
    
    while (!isWiFiStable() && stabilityAttempts < maxStabilityAttempts) {
      delay(1000);
      stabilityAttempts++;
    }
    
    if (isWiFiStable()) {
      testNetworkConnectivity();
    }
    
    reconnectAttempts = 0;
  }
}

// Function to check WiFi connection status
bool isWiFiConnected() {
  return WiFi.status() == WL_CONNECTED;
}

// Function to reconnect to WiFi if connection is lost
void reconnectWiFi() {
  if (!isWiFiConnected()) {
    unsigned long now = millis();
    
    if (now - lastReconnectAttempt > WIFI_RECONNECT_DELAY) {
      lastReconnectAttempt = now;
      
      if (reconnectAttempts >= MAX_RECONNECT_ATTEMPTS) {
        resetWiFi();
        setupWiFi();
        return;
      }
      
      WiFi.disconnect();
      WiFi.begin(wifiCredentials[0].ssid, wifiCredentials[0].password);
      reconnectAttempts++;
      
      int attempts = 0;
      while (WiFi.status() != WL_CONNECTED && attempts < 10) {
        delay(500);
        attempts++;
      }
      
      if (WiFi.status() == WL_CONNECTED) {
        reconnectAttempts = 0;
      }
    }
  }
}

#endif // WIFI_SETUP_H 