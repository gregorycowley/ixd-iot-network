#ifndef SLEEP_MANAGER_H
#define SLEEP_MANAGER_H

#include <Arduino.h>
#include <esp_sleep.h>
#include <driver/rtc_io.h>
#include <driver/gpio.h>

class SleepManager {
public:
    SleepManager();
    void begin();
    void loop();
    void enterSleep();
    void wakeUp();
    bool isSleeping();
    void setSleepTimeout(unsigned long timeout);
    void resetSleepTimer();

private:
    static const unsigned long DEFAULT_SLEEP_TIMEOUT = 1 * 60 * 1000; // 1 minute in milliseconds
    static const int LED_PIN = D9;      // LED pin definition
    static const gpio_num_t WAKE_PIN = GPIO_NUM_9;  // Wake-up pin (GPIO9)
    unsigned long _sleepTimeout;
    unsigned long _lastActivityTime;
    bool _isSleeping;
    
    void configureWakeupSources();
};

#endif // SLEEP_MANAGER_H 