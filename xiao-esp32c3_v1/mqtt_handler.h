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

// Track MQTT connection state
bool mqttConnected = false;
unsigned long lastMqttReconnectAttempt = 0;
unsigned long lastMqttKeepalive = 0;
const unsigned long MQTT_RECONNECT_DELAY = 5000;    // 5 seconds between reconnect attempts
const unsigned long MQTT_KEEPALIVE_INTERVAL = 15000; // 15 seconds between keepalive messages

// Callback function for received MQTT messages
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  if (message == "ON" || message == "on" || message == "1") {
    digitalWrite(ledPin, HIGH);
  } else if (message == "OFF" || message == "off" || message == "0") {
    digitalWrite(ledPin, LOW);
  }
}

// Function to reconnect to MQTT broker
void reconnectMQTT() {
  if (WiFi.status() != WL_CONNECTED) {
    mqttConnected = false;
    return;
  }

  static bool usingAltServer = false;
  const char* currentServer = usingAltServer ? mqtt_server_alt : mqtt_server;

  while (!client.connected()) {
    Serial.print("Connecting to MQTT: ");
    Serial.println(currentServer);
    
    String clientId = getClientId();
    
    if (client.connect(clientId.c_str())) {
      client.subscribe(topic_subscribe);
      mqttConnected = true;
      lastMqttKeepalive = millis();
    } else {
      usingAltServer = !usingAltServer;
      currentServer = usingAltServer ? mqtt_server_alt : mqtt_server;
      client.setServer(currentServer, mqtt_port);
      delay(5000);
    }
  }
}

// Function to setup MQTT
void setupMQTT() {
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqttCallback);
  client.setKeepAlive(60);
  client.setSocketTimeout(10);
  mqttConnected = false;
  reconnectMQTT();
}

// Function to maintain MQTT connection and handle messages
void mqttLoop() {
  if (!client.connected()) {
    mqttConnected = false;
    unsigned long now = millis();
    if (now - lastMqttReconnectAttempt > MQTT_RECONNECT_DELAY) {
      lastMqttReconnectAttempt = now;
      reconnectMQTT();
    }
  } else {
    mqttConnected = true;
    
    unsigned long now = millis();
    if (now - lastMqttKeepalive > MQTT_KEEPALIVE_INTERVAL) {
      lastMqttKeepalive = now;
      String keepaliveMsg = "{\"type\":\"keepalive\",\"rssi\":" + String(WiFi.RSSI()) + "}";
      client.publish(topic_publish, keepaliveMsg.c_str());
    }
  }
  
  if (mqttConnected) {
    client.loop();
  }
}

// Function to publish a message
void publishMessage(const char* message) {
  if (mqttConnected && client.connected()) {
    if (!client.publish(topic_publish, message)) {
      mqttConnected = false;
    }
  }
}

#endif // MQTT_HANDLER_H 