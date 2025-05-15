#ifndef WEB_SERVER_MANAGER_H
#define WEB_SERVER_MANAGER_H

#include <WebServer.h>
#include "Config_device.h"

class WebServerManager {
public:
    WebServerManager();
    void begin();
    void handleClient();
    void stop();
    void setLEDState(bool state);

private:
    WebServer _server;
    void handleRoot();
    void handleNotFound();
    String getStatusJSON();
    bool _ledState;
};

#endif // WEB_SERVER_MANAGER_H 