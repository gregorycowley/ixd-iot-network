#include "config_local.h"
#include <cstdio>

// WiFi credentials array
const WiFiCredential wifiCredentials[] = {
    {"YOUR_SSID", "YOUR_PASSWORD"},  // Primary network
    {"YOUR_SSID", "YOUR_PASSWORD"},  // Fallback network
    // Add more networks here as needed
};

const int wifiCredentialsCount = sizeof(wifiCredentials) / sizeof(wifiCredentials[0]);

// MQTT Broker configuration
const char* mqtt_server = "192.168.100.1";
const char* mqtt_server_alt = "192.168.100.123";
const int mqtt_port = 1883;

// Button configuration
const int buttonSeries = 101;
const char* buttonName = "wolf";

// MQTT Topics
char topic_publish[50];
char topic_subscribe[50];

// Initialize MQTT topics
void initMQTTTopics() {
    snprintf(topic_publish, sizeof(topic_publish), "/cca/pub/%d/%s", buttonSeries, buttonName);
    snprintf(topic_subscribe, sizeof(topic_subscribe), "/cca/sub/%d/%s", buttonSeries, buttonName);
} 