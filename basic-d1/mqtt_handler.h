#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <PubSubClient.h>
#include "wifi_setup.h"
#include "pin_definitions.h"

// MQTT Broker configuration
const char* mqtt_server = "192.168.100.1";  // Update to match your MQTT broker's IP on the 192.168.100.x network
const int mqtt_port = 1883;                       // Default MQTT port

// Function to get unique client ID based on MAC address
String getClientId() {
  String clientId = "basic-d1-";
  #ifdef ESP32
    uint8_t mac[6];
    esp_read_efuse_mac(mac);
    for (int i = 0; i < 6; i++) {
      clientId += String(mac[i], HEX);
    }
  #else
    clientId += WiFi.macAddress();
  #endif
  return clientId;
}

// MQTT Topics - using dynamic client ID
String topic_publish = "/cca/button2/blue";    // Topic to publish messages
String topic_subscribe = "/cca/control2/blue"; // Topic to subscribe to

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
  if (message.startsWith("LED:")) {
    // Handle LED commands
    String state = message.substring(4);
    if (state == "ON") {
      digitalWrite(ledPin, HIGH);
    } else if (state == "OFF") {
      digitalWrite(ledPin, LOW);
    }
  } else if (message.startsWith("PIXEL:")) {
    // Handle NeoPixel commands
    String color = message.substring(6);
    if (color == "RED") {
      pixels.setPixelColor(0, pixels.Color(255, 0, 0));
    } else if (color == "GREEN") {
      pixels.setPixelColor(0, pixels.Color(0, 255, 0));
    } else if (color == "BLUE") {
      pixels.setPixelColor(0, pixels.Color(0, 0, 255));
    } else if (color == "WHITE") {
      pixels.setPixelColor(0, pixels.Color(255, 255, 255));
    } else if (color == "OFF") {
      pixels.setPixelColor(0, pixels.Color(0, 0, 0));
    }
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

  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection to ");
    Serial.print(mqtt_server);
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
      client.subscribe(topic_subscribe.c_str());
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
    client.publish(topic_publish.c_str(), message);
  }
}

#endif // MQTT_HANDLER_H 