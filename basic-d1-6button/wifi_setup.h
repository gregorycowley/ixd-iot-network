#ifndef WIFI_SETUP_H
#define WIFI_SETUP_H

// Include the correct WiFi library based on board type
#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESP8266WiFiMulti.h>
#endif

// WiFi credentials - replace these with your network details
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// WiFi connection parameters
const int MAX_RECONNECT_ATTEMPTS = 3;
const int WIFI_RECONNECT_DELAY = 5000; // 5 seconds
unsigned long lastReconnectAttempt = 0;
int reconnectAttempts = 0;

// Create WiFiMulti object for ESP8266
#ifndef ESP32
ESP8266WiFiMulti wifiMulti;
#endif

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
      Serial.printf("%d dBm\t", WiFi.RSSI(i));
      
      // Print encryption type
      switch (WiFi.encryptionType(i)) {
        #ifdef ESP32
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
        #else
          case ENC_TYPE_NONE:
            Serial.println("Open");
            break;
          case ENC_TYPE_WEP:
            Serial.println("WEP");
            break;
          case ENC_TYPE_TKIP:
            Serial.println("WPA");
            break;
          case ENC_TYPE_CCMP:
            Serial.println("WPA2");
            break;
          case ENC_TYPE_AUTO:
            Serial.println("Auto");
            break;
        #endif
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
  
  // Reinitialize WiFi with specific settings for ESP8266
  #ifndef ESP32
    // Set ESP8266 WiFi settings
    WiFi.setSleepMode(WIFI_NONE_SLEEP);
    WiFi.setAutoReconnect(true);
    WiFi.setPhyMode(WIFI_PHY_MODE_11N);
    WiFi.setOutputPower(20.5); // Maximum power
  #endif
  
  // Set to station mode
  WiFi.mode(WIFI_STA);
  delay(100);
  
  Serial.println("WiFi reset complete");
}

// Function to connect to WiFi
void setupWiFi() {
  Serial.println("\n=== WiFi Setup ===");
  
  // Initial WiFi reset
  resetWiFi();
  
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  
  #ifdef ESP32
    // ESP32 connection
    WiFi.begin(ssid, password);
  #else
    // ESP8266 connection with WiFiMulti
    wifiMulti.addAP(ssid, password);
    WiFi.begin();
  #endif
  
  // Wait for connection with timeout
  int attempts = 0;
  const int maxAttempts = 20; // 10 seconds total (20 * 500ms)
  
  while (WiFi.status() != WL_CONNECTED && attempts < maxAttempts) {
    delay(500);
    attempts++;
    
    #ifndef ESP32
      // For ESP8266, try to connect using WiFiMulti
      if (wifiMulti.run() == WL_CONNECTED) {
        break;
      }
    #endif
    
    // Print status every 2 seconds
    if (attempts % 4 == 0) {
      Serial.print("\nWiFi Status: ");
      Serial.print(getWiFiStatus(WiFi.status()));
      Serial.print(" (Attempt ");
      Serial.print(attempts);
      Serial.print("/");
      Serial.print(maxAttempts);
      Serial.println(")");
      
      #ifndef ESP32
        // Print ESP8266 specific info
        Serial.print("WiFi Channel: ");
        Serial.println(WiFi.channel());
        Serial.print("WiFi Mode: ");
        switch (WiFi.getMode()) {
          case WIFI_OFF:
            Serial.println("OFF");
            break;
          case WIFI_STA:
            Serial.println("STA");
            break;
          case WIFI_AP:
            Serial.println("AP");
            break;
          case WIFI_AP_STA:
            Serial.println("AP+STA");
            break;
        }
      #endif
    } else {
      Serial.print(".");
    }
  }
  
  // Check if connection was successful
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected successfully!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Signal Strength (RSSI): ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
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
      
      // Try to reconnect
      WiFi.disconnect();
      WiFi.begin(ssid, password);
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