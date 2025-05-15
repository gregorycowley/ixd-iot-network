#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <PubSubClient.h>
#include "wifi_setup.h"
#include "pin_definitions.h"
#include "config_local.h"
#include <esp_system.h>  // Required for esp_read_efuse_mac

// Function to get unique client ID based on MAC address
String getClientId() {
  String clientId = "xiao-esp32c3-";
  clientId += WiFi.macAddress();
  clientId.replace(":", "");  // Remove colons from MAC address
  return clientId;
}

// Create WiFi and MQTT clients
WiFiClient espClient;
PubSubClient client(espClient);

// Forward declaration of the NeoPixel object
extern Adafruit_NeoPixel pixels;

// Callback function for received MQTT messages
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  // Convert payload to string
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // Handle the message based on its content
  if (message == "ON" || message == "on" || message == "1") {
    // Handle ON command
    digitalWrite(ledPin, HIGH);
    pixels.setPixelColor(0, pixels.Color(255, 255, 255)); // White
    pixels.show();
  } else if (message == "OFF" || message == "off" || message == "0") {
    // Handle OFF command
    digitalWrite(ledPin, LOW);
    pixels.setPixelColor(0, pixels.Color(0, 0, 0)); // Off
    pixels.show();
  }
}

// Function to reconnect to MQTT broker
void reconnectMQTT() {
  // Check WiFi connection first
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected. Cannot connect to MQTT broker.");
    return;
  }

  static bool usingAltServer = false;
  const char* currentServer = usingAltServer ? mqtt_server_alt : mqtt_server;

  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection to ");
    Serial.print(currentServer);
    Serial.print(":");
    Serial.println(mqtt_port);
    
    // Get unique client ID
    String clientId = getClientId();
    
    // Attempt to connect without authentication
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      Serial.print("Client ID: ");
      Serial.println(clientId);
      
      // Subscribe to the topic
      client.subscribe(topic_subscribe);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.print(" (");
      switch (client.state()) {
        case -4: Serial.println("Connection timeout"); break;
        case -3: Serial.println("Connection lost"); break;
        case -2: Serial.println("Connect failed"); break;
        case -1: Serial.println("Disconnected"); break;
        case 0: Serial.println("Connected"); break;
        case 1: Serial.println("Bad protocol"); break;
        case 2: Serial.println("Bad client ID"); break;
        case 3: Serial.println("Unavailable"); break;
        case 4: Serial.println("Bad credentials"); break;
        case 5: Serial.println("Unauthorized"); break;
        default: Serial.println("Unknown error"); break;
      }
      Serial.println(") try again in 5 seconds");
      
      // Switch servers
      usingAltServer = !usingAltServer;
      currentServer = usingAltServer ? mqtt_server_alt : mqtt_server;
      client.setServer(currentServer, mqtt_port);
      
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// Function to setup MQTT
void setupMQTT() {
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqttCallback);
}

// Function to maintain MQTT connection and handle messages
void mqttLoop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();
}

// Function to publish a message
void publishMessage(const char* message) {
  if (client.connected()) {
    client.publish(topic_publish, message);
  }
}

#endif // MQTT_HANDLER_H 