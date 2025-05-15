#ifndef SLEEP_MANAGER_H
#define SLEEP_MANAGER_H

#include <ESP8266WiFi.h>

class SleepManager {
public:
    SleepManager(int wakePin, int ledPin, unsigned long idleThreshold = 30000, unsigned long powerSaveDelay = 1000);
    
    void begin();
    void update();
    bool isInPowerSaveMode() const { return _isPowerSaveMode; }
    
private:
    void enterPowerSaveMode();
    void exitPowerSaveMode();
    
    const int _wakePin;
    const int _ledPin;
    const unsigned long _idleThreshold;
    const unsigned long _powerSaveDelay;
    
    unsigned long _lastActivityTime;
    unsigned long _lastDebugTime;
    bool _isPowerSaveMode;
    
    static const int UNUSED_PINS[];
    static const int NUM_UNUSED_PINS;
};

#endif // SLEEP_MANAGER_H 