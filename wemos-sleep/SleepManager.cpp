#include "SleepManager.h"

const int SleepManager::UNUSED_PINS[] = {D1, D2, D3, D5, D6, D7, D8};
const int SleepManager::NUM_UNUSED_PINS = sizeof(UNUSED_PINS) / sizeof(UNUSED_PINS[0]);

SleepManager::SleepManager(int wakePin, int ledPin, unsigned long idleThreshold, unsigned long powerSaveDelay)
    : _wakePin(wakePin)
    , _ledPin(ledPin)
    , _idleThreshold(idleThreshold)
    , _powerSaveDelay(powerSaveDelay)
    , _lastActivityTime(0)
    , _lastDebugTime(0)
    , _isPowerSaveMode(false)
{
}

void SleepManager::begin() {
    // Disable WiFi to save power
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();
    delay(1);  // Give WiFi time to enter sleep mode
    
    // Configure wake-up pin
    pinMode(_wakePin, INPUT_PULLUP);
    pinMode(_ledPin, OUTPUT);
    
    // Configure unused pins for power saving
    for (int i = 0; i < NUM_UNUSED_PINS; i++) {
        pinMode(UNUSED_PINS[i], INPUT);  // Set as input without pull-up
    }
    
    _lastActivityTime = millis();  // Initialize activity timer
    _lastDebugTime = millis();     // Initialize debug timer
}

void SleepManager::enterPowerSaveMode() {
    if (!_isPowerSaveMode) {
        Serial.println("Entering power save mode...");
        _isPowerSaveMode = true;
    }
}

void SleepManager::exitPowerSaveMode() {
    if (_isPowerSaveMode) {
        Serial.println("Exiting power save mode...");
        _isPowerSaveMode = false;
    }
}

void SleepManager::update() {
    // Feed the watchdog timer
    ESP.wdtFeed();
    
    // Check if button is pressed (LOW because of INPUT_PULLUP)
    if (digitalRead(_wakePin) == LOW) {
        Serial.println("Button pressed!");
        digitalWrite(_ledPin, LOW);  // Turn LED on (LOW turns it on for Wemos D1)
        delay(1000);  // Keep LED on for 1 second
        digitalWrite(_ledPin, HIGH); // Turn LED off
        _lastActivityTime = millis();  // Reset the activity timer
        exitPowerSaveMode();
    }
    
    // Check if we've been idle for the threshold time
    if (millis() - _lastActivityTime >= _idleThreshold) {
        enterPowerSaveMode();
    } else {
        // Normal operation mode
        if (millis() - _lastDebugTime >= 5000) {
            Serial.print("Uptime: ");
            Serial.print(millis() / 1000);
            Serial.println(" seconds");
            _lastDebugTime = millis();
        }
    }
    
    // In power save mode, use longer delays between checks
    if (_isPowerSaveMode) {
        delay(_powerSaveDelay);  // Delay in power save mode
    } else {
        delay(10);  // Normal operation delay
    }
} 