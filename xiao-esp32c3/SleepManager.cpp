#include "SleepManager.h"

SleepManager::SleepManager() : 
    _sleepTimeout(DEFAULT_SLEEP_TIMEOUT),
    _lastActivityTime(0),
    _isSleeping(false) {
}

void SleepManager::begin() {
    configureWakeupSources();
    resetSleepTimer();
    Serial.println("Sleep Manager initialized");
}

void SleepManager::loop() {
    if (!_isSleeping && (millis() - _lastActivityTime > _sleepTimeout)) {
        Serial.println("Sleep timeout reached, preparing to sleep...");
        enterSleep();
    }
}

void SleepManager::enterSleep() {
    if (!_isSleeping) {
        Serial.println("Entering sleep mode...");
        _isSleeping = true;
        
        // Turn off LED before sleep
        digitalWrite(LED_PIN, LOW);
        Serial.println("LED turned off");
        
        // Configure wake-up sources
        esp_sleep_enable_gpio_wakeup();
        
        // Configure GPIO for wake-up
        gpio_config_t io_conf = {};
        io_conf.pin_bit_mask = (1ULL << static_cast<uint32_t>(WAKE_PIN));
        io_conf.mode = GPIO_MODE_INPUT;
        io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
        io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
        io_conf.intr_type = GPIO_INTR_HIGH_LEVEL;
        gpio_config(&io_conf);
        
        // Enable wake-up on GPIO
        gpio_wakeup_enable(WAKE_PIN, GPIO_INTR_HIGH_LEVEL);
        
        Serial.println("Wake-up sources configured");
        Serial.println("Entering deep sleep...");
        delay(100);  // Give time for serial messages to be sent
        
        // Enter deep sleep
        esp_deep_sleep_start();
    }
}

void SleepManager::wakeUp() {
    if (_isSleeping) {
        Serial.println("Waking up...");
        _isSleeping = false;
        resetSleepTimer();
        
        // Reconfigure GPIO after wake-up
        configureWakeupSources();
        Serial.println("Wake-up complete");
    }
}

bool SleepManager::isSleeping() {
    return _isSleeping;
}

void SleepManager::setSleepTimeout(unsigned long timeout) {
    _sleepTimeout = timeout;
    Serial.print("Sleep timeout set to: ");
    Serial.println(timeout);
}

void SleepManager::resetSleepTimer() {
    _lastActivityTime = millis();
    Serial.println("Sleep timer reset");
}

void SleepManager::configureWakeupSources() {
    // Configure GPIO for normal operation
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = (1ULL << static_cast<uint32_t>(WAKE_PIN));
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_conf);
    Serial.println("GPIO configured for normal operation");
} 