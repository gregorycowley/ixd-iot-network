#ifndef CONFIG_H
#define CONFIG_H

// WiFi credentials structure
struct WiFiCredential {
    const char* ssid;
    const char* password;
};

// WiFi credentials array
extern const WiFiCredential wifiCredentials[];
extern const int wifiCredentialsCount;

// MQTT Broker configuration
extern const char* mqtt_server;
extern const char* mqtt_server_alt;
extern const int mqtt_port;

// Button configuration
extern const int buttonSeries;
extern const char* buttonName;

// MQTT Topics
extern char topic_publish[50];
extern char topic_subscribe[50];

// Function declarations
void initMQTTTopics();

#endif // CONFIG_H 