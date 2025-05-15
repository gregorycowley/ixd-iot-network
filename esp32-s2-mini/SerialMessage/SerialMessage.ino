// ESP32-S2 Mini Serial Message Example
// This sketch demonstrates how to use the BoardTest and WiFiManager functionality

#include "BoardTest.h"
#include "WiFiManager.h"
#include "WebServerManager.h"
#include "BatteryManager.h"
#include "ButtonManager.h"
#include "NeoPixelManager.h"
#include "esp_system.h"  // For esp_reset_reason()

// Create instances of our managers
BoardTest boardTest;
WiFiManager wifiManager;
WebServerManager webServerManager;
BatteryManager batteryManager;
ButtonManager buttonManager;
NeoPixelManager neoPixelManager;

// Enable/disable features
#define ENABLE_BOARD_TEST true
#define ENABLE_WIFI true
#define ENABLE_WEB_SERVER true
#define ENABLE_BATTERY_MONITOR true
#define ENABLE_BUTTON true
#define ENABLE_NEOPIXEL true

// Debug counter to track resets
static int resetCount = 0;

// Function to convert reset reason to string
const char* getResetReasonString(esp_reset_reason_t reason) {
    switch (reason) {
        case ESP_RST_POWERON: return "Power On";
        case ESP_RST_EXT: return "External Reset";
        case ESP_RST_SW: return "Software Reset";
        case ESP_RST_PANIC: return "Panic";
        case ESP_RST_INT_WDT: return "Interrupt Watchdog";
        case ESP_RST_TASK_WDT: return "Task Watchdog";
        case ESP_RST_WDT: return "Watchdog";
        case ESP_RST_DEEPSLEEP: return "Deep Sleep";
        case ESP_RST_BROWNOUT: return "Brownout";
        case ESP_RST_SDIO: return "SDIO";
        default: return "Unknown";
    }
}

// Button callback function
void onButtonStateChange(bool state) {
    Serial1.print("Button state changed to: ");
    Serial1.println(state ? "PRESSED" : "RELEASED");
}

void setup() {
  // Initialize serial communication first
  Serial1.begin(115200, SERIAL_8N1, -1, 10);  // RX pin -1 means not used
  delay(100); // Give serial time to initialize
  
  // Print reset information
  Serial1.println("\n\n=== ESP32-S2 Starting ===");
  Serial1.print("Reset count: ");
  Serial1.println(++resetCount);
  Serial1.print("Reset reason: ");
  Serial1.println(getResetReasonString(esp_reset_reason()));
  Serial1.print("Free heap: ");
  Serial1.println(ESP.getFreeHeap());
  
  // Initialize features one at a time with delays
  if (ENABLE_BOARD_TEST) {
    Serial1.println("SerialMessage: setup: Enabling Board Test");
    boardTest.enable();
    delay(100);  // Give time for initialization
    boardTest.begin();
    delay(100);  // Give time for initialization
    Serial1.println("SerialMessage: setup: Board Test initialized");
  }
  
  if (ENABLE_BATTERY_MONITOR) {
    Serial1.println("SerialMessage: setup: Enabling Battery Monitor");
    batteryManager.enable();
    delay(100);  // Give time for initialization
    batteryManager.begin();
    delay(100);  // Give time for initialization
    Serial1.println("SerialMessage: setup: Battery Monitor initialized");
  }
  
  if (ENABLE_BUTTON) {
    Serial1.println("SerialMessage: setup: Enabling Button");
    buttonManager.enable();
    delay(100);  // Give time for initialization
    buttonManager.begin();
    delay(100);  // Give time for initialization
    buttonManager.setCallback(onButtonStateChange);
    Serial1.println("SerialMessage: setup: Button initialized");
  }
  
  if (ENABLE_WIFI) {
    Serial1.println("SerialMessage: setup: Enabling WiFi");
    wifiManager.enable();
    delay(100);  // Give time for initialization
    wifiManager.begin();
    delay(100);  // Give time for initialization
    Serial1.println("SerialMessage: setup: WiFi initialized");
    
    // Only enable web server if WiFi is enabled
    if (ENABLE_WEB_SERVER) {
      Serial1.println("SerialMessage: setup: Enabling Web Server");
      Serial1.print("Free heap before web server: ");
      Serial1.println(ESP.getFreeHeap());
      
      webServerManager.enable();
      delay(100);  // Give time for initialization
      
      // Connect battery manager to web server if enabled
      if (ENABLE_BATTERY_MONITOR) {
        webServerManager.setBatteryManager(&batteryManager);
      }
      
      webServerManager.begin();
      delay(100);  // Give time for initialization
      
      Serial1.print("Free heap after web server: ");
      Serial1.println(ESP.getFreeHeap());
      Serial1.println("SerialMessage: setup: Web Server initialized");
    }
  }
  
  if (ENABLE_NEOPIXEL) {
    Serial1.println("SerialMessage: setup: Enabling NeoPixels");
    neoPixelManager.enable();
    delay(100);  // Give time for initialization
    neoPixelManager.begin();
    Serial1.println("SerialMessage: setup: NeoPixels initialized");
  }
  
  Serial1.println("SerialMessage: setup: Complete");
}

void loop() {
  // Feed the watchdog timer
  yield();
  
  if (ENABLE_BOARD_TEST) {
    boardTest.update();
  }
  
  if (ENABLE_BATTERY_MONITOR) {
    batteryManager.update();
  }
  
  if (ENABLE_BUTTON) {
    buttonManager.update();
  }
  
  if (ENABLE_WIFI) {
    wifiManager.update();
    
    if (ENABLE_WEB_SERVER) {
      webServerManager.update();
    }
  }
  
  if (ENABLE_NEOPIXEL) {
    neoPixelManager.update();
  }
  
  // Small delay to prevent watchdog timer issues
  delay(10);
} 