#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <PubSubClient.h>
#include "wifi_setup.h"
#include "pin_definitions.h"

#ifdef ESP32
  #include <esp_system.h>  // For esp_read_efuse_mac
#endif

// MQTT Broker configuration
const char* mqtt_server = "192.168.100.1";  // Update to match your MQTT broker's IP on the 192.168.100.x network
const int mqtt_port = 1883;                       // Default MQTT port

// Forward declarations
void publishMessage(const char* message);

// Function to get unique client ID based on MAC address
String getClientId() {
  String clientId = "basic-d1-";
  clientId += WiFi.macAddress();
  Serial.print("Generated Client ID: ");
  Serial.println(clientId);
  return clientId;
}

// MQTT Topics - using dynamic client ID
String topic_publish = "/cca/led/rings/pub";    // Topic to publish messages
String topic_subscribe = "/cca/led/rings"; // Topic to subscribe to

// Create WiFi and MQTT clients
WiFiClient espClient;
PubSubClient client(espClient);

// Forward declaration of the NeoPixel object
extern Adafruit_NeoPixel strip;
extern const int RING_COUNT;
extern const int ring_counts[];

// Function to set ring LEDs
void setRingBrightness(int ring, int percentage) {
  Serial.println("\n=== Setting Ring LEDs ===");
  Serial.print("Ring: ");
  Serial.println(ring);
  Serial.print("Percentage: ");
  Serial.println(percentage);
  
  // Calculate the starting index for this ring
  int start_index = 0;
  for(int i = 0; i < ring; i++) {
    start_index += ring_counts[i];
  }
  Serial.print("Starting LED index: ");
  Serial.println(start_index);
  
  // Calculate how many LEDs to light up based on percentage
  int leds_to_light = (ring_counts[ring] * percentage) / 100;
  Serial.print("LEDs to light: ");
  Serial.print(leds_to_light);
  Serial.print(" out of ");
  Serial.println(ring_counts[ring]);
  
  // Turn off all LEDs in the ring first
  Serial.println("Turning off all LEDs in ring...");
  for(int i = 0; i < ring_counts[ring]; i++) {
    strip.setPixelColor(start_index + i, strip.Color(0, 0, 0));
  }
  
  // Light up the calculated number of LEDs
  Serial.println("Lighting up LEDs...");
  for(int i = 0; i < leds_to_light; i++) {
    strip.setPixelColor(start_index + i, strip.Color(255, 255, 255));
  }
  
  strip.show();
  Serial.println("LED update complete");
  Serial.println("=====================\n");
}

// Callback function for received MQTT messages
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.println("\n=== MQTT Message Received ===");
  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Payload length: ");
  Serial.println(length);
  
  // Convert payload to string
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Message: ");
  Serial.println(message);

  // Handle ring-specific messages
  if (message.startsWith("RING")) {
    Serial.println("Processing RING command...");
    // Format: RING<number>:<percentage>
    int colonIndex = message.indexOf(':');
    if (colonIndex != -1) {
      String ringStr = message.substring(4, colonIndex);
      String percentageStr = message.substring(colonIndex + 1);
      
      int ring = ringStr.toInt();
      int percentage = percentageStr.toInt();
      
      Serial.print("Parsed ring: ");
      Serial.println(ring);
      Serial.print("Parsed percentage: ");
      Serial.println(percentage);
      
      // Validate ring number and percentage
      if (ring >= 0 && ring < RING_COUNT && percentage >= 0 && percentage <= 100) {
        Serial.println("Parameters valid, setting ring...");
        setRingBrightness(ring, percentage);
        
        // Publish confirmation
        String response = "RING" + String(ring) + " set to " + String(percentage) + "% (" + 
                         String((ring_counts[ring] * percentage) / 100) + " LEDs)";
        Serial.print("Sending response: ");
        Serial.println(response);
        publishMessage(response.c_str());
      } else {
        String error = "Invalid parameters: RING" + String(ring) + ":" + String(percentage);
        Serial.print("Error: ");
        Serial.println(error);
        publishMessage(error.c_str());
      }
    } else {
      Serial.println("Invalid message format - missing colon");
    }
  } else {
    Serial.println("Unknown message format");
  }
  Serial.println("==========================\n");
}

// Function to reconnect to MQTT broker
void reconnectMQTT() {
  Serial.println("\n=== MQTT Reconnection Attempt ===");
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
      Serial.println("MQTT connected successfully");
      Serial.print("Client ID: ");
      Serial.println(clientId);
      
      // Subscribe to the topic
      Serial.print("Subscribing to topic: ");
      Serial.println(topic_subscribe);
      client.subscribe(topic_subscribe.c_str());
      Serial.println("Subscription complete");
    } else {
      Serial.print("MQTT connection failed, rc=");
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
  Serial.println("==============================\n");
}

// Function to setup MQTT
void setupMQTT() {
  Serial.println("\n=== MQTT Setup ===");
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqttCallback);
  Serial.println("MQTT setup complete");
  Serial.println("=================\n");
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
  Serial.println("\n=== Publishing MQTT Message ===");
  Serial.print("Topic: ");
  Serial.println(topic_publish);
  Serial.print("Message: ");
  Serial.println(message);
  
  if (client.connected()) {
    client.publish(topic_publish.c_str(), message);
    Serial.println("Message published successfully");
  } else {
    Serial.println("Failed to publish - MQTT not connected");
  }
  Serial.println("=============================\n");
}

#endif // MQTT_HANDLER_H 