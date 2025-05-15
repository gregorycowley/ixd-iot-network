#ifndef SLEEP_MANAGER_H
#define SLEEP_MANAGER_H

#include <esp_sleep.h>
#include <driver/gpio.h>

// Sleep settings
const unsigned long SLEEP_DELAY = 30000; // Sleep after 30 seconds of inactivity
unsigned long lastActivityTime = 0;
bool isSleeping = false;

// Function to initialize sleep functionality
void initSleep(int wakePin) {
  lastActivityTime = millis();
}

// Function to check if it's time to sleep
bool shouldSleep() {
  return (!isSleeping && (millis() - lastActivityTime > SLEEP_DELAY));
}

// Function to reset sleep timer
void resetSleepTimer() {
  lastActivityTime = millis();
  isSleeping = false;
}

// Function to put device to sleep
void goToSleep() {
  if (!isSleeping) {
    Serial.println("Going to sleep...");
    Serial.flush();
    isSleeping = true;
    
    // Enter a low-power state while keeping USB active
    esp_sleep_enable_timer_wakeup(1000000); // 1 second wake-up interval
    esp_light_sleep_start();
    
    // After waking up, print a message
    Serial.println("Woke up!");
  }
}

#endif // SLEEP_MANAGER_H 