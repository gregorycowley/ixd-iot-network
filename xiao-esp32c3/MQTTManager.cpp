#include "MQTTManager.h"
#include <WiFi.h>
#include <ArduinoJson.h>

// MQTT Configuration
const char* MQTTManager::MQTT_BROKER = "192.168.100.1";
const char* MQTTManager::MQTT_BROKER_ALT = "192.168.100.123";
const int MQTTManager::MQTT_PORT = 1883;
String MQTTManager::MQTT_PUB_TOPIC = String("/cca/") + String(SERIES_ID) + "/" + String(DEVICE_ID) + "/pub";
String MQTTManager::MQTT_SUB_TOPIC = String("/cca/") + String(SERIES_ID) + "/" + String(DEVICE_ID) + "/sub";

MQTTManager::MQTTManager() : _mqtt(_wifiClient), _connected(false), _usingAltBroker(false), _ledCallback(nullptr) {
}

void MQTTManager::begin() {
    _mqtt.setServer(MQTT_BROKER, MQTT_PORT);
    _mqtt.setCallback([this](char* topic, byte* payload, unsigned int length) {
        handleMessage(topic, payload, length);
    });
    connect();
}

void MQTTManager::loop() {
    if (!_mqtt.connected()) {
        _connected = false;
        connect();
    }
    _mqtt.loop();
}

bool MQTTManager::connect() {
    if (_mqtt.connected()) {
        return true;
    }

    // Try to connect to the current broker
    const char* currentBroker = _usingAltBroker ? MQTT_BROKER_ALT : MQTT_BROKER;
    Serial.print("Connecting to MQTT broker: ");
    Serial.println(currentBroker);
    
    // Create a unique client ID using device ID and series ID
    String clientId = String(DEVICE_ID) + "-" + String(SERIES_ID);
    
    if (_mqtt.connect(clientId.c_str())) {
        Serial.println("Connected to MQTT broker");
        // Subscribe to LED control topic
        if (_mqtt.subscribe(MQTT_SUB_TOPIC.c_str())) {
            Serial.println("Subscribed to LED control topic");
        } else {
            Serial.println("Failed to subscribe to LED control topic");
        }
        _connected = true;
        return true;
    } else {
        Serial.println("Failed to connect to MQTT broker");
        
        // If we're not already using the alternate broker, try it
        if (!_usingAltBroker) {
            Serial.println("Trying alternate MQTT broker...");
            _usingAltBroker = true;
            _mqtt.setServer(MQTT_BROKER_ALT, MQTT_PORT);
            return connect();  // Recursively try to connect to alternate broker
        }
        
        _connected = false;
        return false;
    }
}

bool MQTTManager::isConnected() {
    return _connected;
}

void MQTTManager::publishButtonPress() {
    if (!_mqtt.connected()) {
        connect();
    }
    
    if (_mqtt.connected()) {
        String message = "{\"device_id\":\"" + String(DEVICE_ID) + 
                        "\",\"series_id\":\"" + String(SERIES_ID) + 
                        "\",\"event\":\"button_press\"}";
        
        if (_mqtt.publish(MQTT_PUB_TOPIC.c_str(), message.c_str())) {
            Serial.println("Published button press to MQTT");
        } else {
            Serial.println("Failed to publish to MQTT");
        }
    }
}

void MQTTManager::setCallback(void (*callback)(char*, uint8_t*, unsigned int)) {
    _mqtt.setCallback(callback);
}

void MQTTManager::setLEDCallback(void (*callback)(bool)) {
    _ledCallback = callback;
}

void MQTTManager::handleMessage(char* topic, byte* payload, unsigned int length) {
    // Convert payload to string
    char message[length + 1];
    memcpy(message, payload, length);
    message[length] = '\0';
    
    Serial.print("Received message on topic: ");
    Serial.println(topic);
    Serial.print("Message: ");
    Serial.println(message);
    
    // Parse JSON message
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, message);
    
    if (error) {
        Serial.print("JSON parsing failed: ");
        Serial.println(error.c_str());
        return;
    }
    
    // Check if message contains LED state
    if (doc["led_state"].is<bool>()) {
        bool ledState = doc["led_state"].as<bool>();
        Serial.print("Setting LED to: ");
        Serial.println(ledState ? "ON" : "OFF");
        
        // Call LED callback if set
        if (_ledCallback != nullptr) {
            _ledCallback(ledState);
        }
    }
} 