#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <PubSubClient.h>
#include <WiFiClient.h>
#include "Config_device.h"

class MQTTManager {
public:
    MQTTManager();
    void begin();
    void loop();
    bool connect();
    bool isConnected();
    void publishButtonPress();
    void setCallback(void (*callback)(char*, uint8_t*, unsigned int));
    void setLEDCallback(void (*callback)(bool));

private:
    static const char* MQTT_BROKER;
    static const char* MQTT_BROKER_ALT;
    static const int MQTT_PORT;
    static String MQTT_PUB_TOPIC;
    static String MQTT_SUB_TOPIC;
    
    WiFiClient _wifiClient;
    PubSubClient _mqtt;
    bool _connected;
    bool _usingAltBroker;
    void (*_ledCallback)(bool);
    void handleMessage(char* topic, byte* payload, unsigned int length);
};

#endif // MQTT_MANAGER_H 