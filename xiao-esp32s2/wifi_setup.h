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
    case WL_IDLE_STATUS:
      return "Idle";
    case WL_NO_SSID_AVAIL:
      return "No SSID Available";
    case WL_SCAN_COMPLETED:
      return "Scan Completed";
    case WL_CONNECTED:
      return "Connected";
    case WL_CONNECT_FAILED:
      return "Connection Failed";
    case WL_CONNECTION_LOST:
      return "Connection Lost";
    case WL_DISCONNECTED:
      return "Disconnected";
    default:
      return "Unknown Status";
  }
}

// Function to scan for available WiFi networks
void scanWiFiNetworks() {
  Serial.println("\nScanning for WiFi networks...");
  
  // Start WiFi scan
  int n = WiFi.scanNetworks();
  
  if (n == 0) {
    Serial.println("No networks found");
  } else {
    Serial.printf("%d networks found\n", n);
    Serial.println("----------------------------------------");
    Serial.println("SSID\t\t\tSignal\tEncryption");
    Serial.println("----------------------------------------");
    
    for (int i = 0; i < n; ++i) {
      // Print SSID
      Serial.printf("%-32s", WiFi.SSID(i).c_str());
      
      // Print signal strength
      Serial.printf("%ld dBm\t", WiFi.RSSI(i));
      
      // Print encryption type
      switch (WiFi.encryptionType(i)) {
        case WIFI_AUTH_OPEN:
          Serial.println("Open");
          break;
        case WIFI_AUTH_WEP:
          Serial.println("WEP");
          break;
        case WIFI_AUTH_WPA_PSK:
          Serial.println("WPA");
          break;
        case WIFI_AUTH_WPA2_PSK:
          Serial.println("WPA2");
          break;
        case WIFI_AUTH_WPA_WPA2_PSK:
          Serial.println("WPA/WPA2");
          break;
        case WIFI_AUTH_WPA2_ENTERPRISE:
          Serial.println("WPA2 Enterprise");
          break;
        default:
          Serial.println("Unknown");
          break;
      }
    }
  }
  Serial.println("----------------------------------------");
}

// Function to perform a complete WiFi reset
void resetWiFi() {
  Serial.println("Performing complete WiFi reset...");
  
  // Disconnect and turn off WiFi
  WiFi.disconnect(true);
  delay(1000);
  
  // Reset WiFi mode
  WiFi.mode(WIFI_OFF);
  delay(1000);
  
  // Reset reconnect attempts counter
  reconnectAttempts = 0;
  
  // Set to station mode
  WiFi.mode(WIFI_STA);
  delay(100);
  
  Serial.println("WiFi reset complete");
}

// Function to check if WiFi connection is stable
bool isWiFiStable() {
  static unsigned long lastCheck = 0;
  static int stableCount = 0;
  const int requiredStableChecks = 3;  // Number of consecutive stable checks required
  const int checkInterval = 1000;      // Check every second
  
  unsigned long now = millis();
  if (now - lastCheck >= checkInterval) {
    lastCheck = now;
    
    if (WiFi.status() == WL_CONNECTED) {
      stableCount++;
      Serial.printf("Stability check %d/%d (RSSI: %d)\n", 
                   stableCount, requiredStableChecks, WiFi.RSSI());
      if (stableCount >= requiredStableChecks) {
        return true;
      }
    } else {
      Serial.printf("Connection lost during stability check (Status: %s)\n", 
                   getWiFiStatus(WiFi.status()).c_str());
      stableCount = 0;
    }
  }
  return false;
}

// Function to test network connectivity
bool testNetworkConnectivity() {
  Serial.println("\nTesting network connectivity...");
  
  // Test gateway connectivity
  Serial.print("Testing gateway connectivity (");
  Serial.print(WiFi.gatewayIP());
  Serial.println(")...");
  
  if (WiFi.ping(WiFi.gatewayIP())) {
    Serial.println("Gateway is reachable");
    
    // Test MQTT server connectivity
    Serial.print("Testing MQTT server connectivity (");
    Serial.print(mqtt_server);
    Serial.println(")...");
    
    if (WiFi.ping(mqtt_server)) {
      Serial.println("MQTT server is reachable");
      return true;
    } else {
      Serial.println("MQTT server is not reachable");
      return false;
    }
  } else {
    Serial.println("Gateway is not reachable");
    return false;
  }
}

// Function to connect to WiFi
void setupWiFi() {
  Serial.println("\n=== WiFi Setup ===");
  
  // Initial WiFi reset
  resetWiFi();
  
  // Scan for available networks
  Serial.println("Scanning for available networks...");
  int n = WiFi.scanNetworks();
  
  if (n == 0) {
    Serial.println("No networks found!");
    return;
  }
  
  // Check which networks from our credentials list are available
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
  
  // Try to connect to the first available network
  bool connected = false;
  for (int i = 0; i < wifiCredentialsCount && !connected; i++) {
    if (foundNetworks[i]) {
      Serial.printf("Found network %d/%d, connecting to: %s\n", 
                   i + 1, wifiCredentialsCount, wifiCredentials[i].ssid);
      
      WiFi.begin(wifiCredentials[i].ssid, wifiCredentials[i].password);
      
      // Wait for connection with timeout
      int attempts = 0;
      const int maxAttempts = 20; // 10 seconds total (20 * 500ms)
      
      while (WiFi.status() != WL_CONNECTED && attempts < maxAttempts) {
        delay(500);
        attempts++;
        
        // Print status every 2 seconds
        if (attempts % 4 == 0) {
          Serial.print("\nWiFi Status: ");
          Serial.print(getWiFiStatus(WiFi.status()));
          Serial.print(" (Attempt ");
          Serial.print(attempts);
          Serial.print("/");
          Serial.print(maxAttempts);
          Serial.println(")");
        } else {
          Serial.print(".");
        }
      }
      
      if (WiFi.status() == WL_CONNECTED) {
        connected = true;
        break;
      }
    }
  }
  
  // Check if connection was successful
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected successfully!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Gateway IP: ");
    Serial.println(WiFi.gatewayIP());
    Serial.print("Subnet Mask: ");
    Serial.println(WiFi.subnetMask());
    Serial.print("DNS Server: ");
    Serial.println(WiFi.dnsIP());
    Serial.print("Signal Strength (RSSI): ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
    
    // Wait for connection to stabilize
    Serial.println("\nWaiting for connection to stabilize...");
    int stabilityAttempts = 0;
    const int maxStabilityAttempts = 30; // Wait up to 30 seconds for stability
    
    while (!isWiFiStable() && stabilityAttempts < maxStabilityAttempts) {
      delay(1000);
      stabilityAttempts++;
      Serial.print(".");
    }
    
    if (isWiFiStable()) {
      Serial.println("\nConnection is stable!");
      
      // Test network connectivity
      if (testNetworkConnectivity()) {
        Serial.println("Network connectivity test passed!");
      } else {
        Serial.println("Network connectivity test failed!");
        Serial.println("There may be network configuration issues.");
      }
    } else {
      Serial.println("\nConnection failed to stabilize!");
    }
    
    reconnectAttempts = 0; // Reset reconnect counter on successful connection
  } else {
    Serial.println("\nWiFi connection failed!");
    Serial.print("Final Status: ");
    Serial.println(getWiFiStatus(WiFi.status()));
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
    
    // Check if it's time to attempt reconnection
    if (now - lastReconnectAttempt > WIFI_RECONNECT_DELAY) {
      lastReconnectAttempt = now;
      
      Serial.println("WiFi connection lost. Attempting to reconnect...");
      
      // If we've tried too many times, do a complete reset
      if (reconnectAttempts >= MAX_RECONNECT_ATTEMPTS) {
        Serial.println("Too many reconnect attempts, performing full reset...");
        resetWiFi();
        setupWiFi();
        return;
      }
      
      // Try to reconnect using the first network in our credentials list
      WiFi.disconnect();
      WiFi.begin(wifiCredentials[0].ssid, wifiCredentials[0].password);
      reconnectAttempts++;
      
      int attempts = 0;
      while (WiFi.status() != WL_CONNECTED && attempts < 10) {
        delay(500);
        Serial.print(".");
        attempts++;
      }
      
      if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi reconnected successfully!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        reconnectAttempts = 0; // Reset counter on successful reconnection
      } else {
        Serial.println("\nFailed to reconnect to WiFi");
        Serial.print("Reconnect attempt ");
        Serial.print(reconnectAttempts);
        Serial.print(" of ");
        Serial.println(MAX_RECONNECT_ATTEMPTS);
      }
    }
  }
}

#endif // WIFI_SETUP_H 