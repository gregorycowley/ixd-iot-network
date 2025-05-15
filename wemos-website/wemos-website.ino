#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "battery_monitor.h"
#include "web_server.h"

// Configuration
const bool ENABLE_MQTT = false;  // Set to false to disable MQTT for testing
const bool DEBUG_INPUTS = true;  // Set to true to print input states
const unsigned long BATTERY_CHECK_INTERVAL = 60000;  // Check battery every minute
unsigned long lastBatteryCheck = 0;

// WiFi credentials
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// MQTT Broker
const char* mqtt_server = "YOUR_MQTT_BROKER_IP";
const int mqtt_port = 1883;
const char* mqtt_client_id = "wemos_d1_client";

// MQTT Topics
const char* button_topic = "wemos/button";
const char* led_topic = "wemos/led";

// Pin definitions
const int buttonPins[] = {D0, D1, D2, D5, D6, D7};  // Array of button pins
const int numButtons = 6;  // Number of buttons
// const int ledPin = D2;     // LED pin

// Button names for messagesx
const char* buttonNames[] = {
  "Button D0",
  "Button D1",
  "Button D2",
  "Button D5",
  "Button D6",
  "Button D7"
};

// Debounce settings
const unsigned long DEBOUNCE_DELAY = 50;  // milliseconds
unsigned long lastDebounceTime[numButtons] = {0};
int lastButtonState[numButtons] = {HIGH};  // Start with HIGH since buttons are HIGH when released
int buttonState[numButtons] = {HIGH};      // Start with HIGH since buttons are HIGH when released

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // If the message is for the LED topic, control the LED
  if (strcmp(topic, led_topic) == 0) {
    if (message == "ON") {
      // digitalWrite(ledPin, HIGH);
    } else if (message == "OFF") {
      // digitalWrite(ledPin, LOW);
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(mqtt_client_id)) {
      Serial.println("connected");
      client.subscribe(led_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  // Initialize pins
  for (int i = 0; i < numButtons; i++) {
    pinMode(buttonPins[i], INPUT);
    lastButtonState[i] = digitalRead(buttonPins[i]);
    
    if (DEBUG_INPUTS) {
      Serial.print("Initial state for ");
      Serial.print(buttonNames[i]);
      Serial.print(" (pin ");
      Serial.print(buttonPins[i]);
      Serial.print("): ");
      Serial.println(lastButtonState[i]);
    }
  }
  // pinMode(ledPin, OUTPUT);
  // digitalWrite(ledPin, LOW);
  
  // Initialize battery monitoring
  pinMode(BATTERY_PIN, INPUT);
  
  setup_wifi();
  
  // Initialize web server
  initWebServer();
  
  if (ENABLE_MQTT) {
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
  }
}

void loop() {
  if (ENABLE_MQTT) {
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
  }

  // Update web server
  updateWebServer();

  // Check battery periodically
  if (millis() - lastBatteryCheck >= BATTERY_CHECK_INTERVAL) {
    float voltage = readBatteryVoltage();
    float percentage = getBatteryPercentage();
    Serial.print("Battery Voltage: ");
    Serial.print(voltage);
    Serial.print("V (");
    Serial.print(percentage);
    Serial.println("%)");
    lastBatteryCheck = millis();
    
    // Update device status
    updateDeviceStatus(deviceStatus.lastButtonPressed);
  }

  // Check each button state with debouncing
  for (int i = 0; i < numButtons; i++) {
    int reading = digitalRead(buttonPins[i]);
    
    // // Debug print the raw reading
    // Serial.print(buttonNames[i]);
    // Serial.print(" reading: ");
    // Serial.println(reading);
    
    // If the button state changed, reset the debounce timer
    if (reading != lastButtonState[i]) {
      lastDebounceTime[i] = millis();
    }
    
    // If the reading has been stable for longer than the debounce delay
    if ((millis() - lastDebounceTime[i]) > DEBOUNCE_DELAY) {
      // If the button state has changed
      if (reading != buttonState[i]) {
        buttonState[i] = reading;
        
        // Trigger on button press (HIGH to LOW transition)
        if (buttonState[i] == LOW) {
          String message = String(buttonNames[i]) + " pressed";
          if (ENABLE_MQTT) {
            client.publish(button_topic, message.c_str());
          }
          Serial.println(message);
          
          // Update device status
          updateDeviceStatus(buttonNames[i]);
        }
      }
    }
    
    // Save the reading for next time
    lastButtonState[i] = reading;
  }
}
