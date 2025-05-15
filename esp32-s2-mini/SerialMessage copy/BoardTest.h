#ifndef BOARD_TEST_H
#define BOARD_TEST_H

#include <Arduino.h>

// Pin definitions
const int ledPin = 35;  // Onboard LED pin
const int txPin = 10;   // Serial TX pin

class BoardTest {
private:
    bool enabled;
    
public:
    BoardTest() : enabled(false) {}
    
    void begin() {
        if (!enabled) return;
        
        // Initialize serial communication
        Serial1.begin(115200, SERIAL_8N1, -1, txPin);  // RX pin -1 means not used
        
        // Initialize LED pin as output
        pinMode(ledPin, OUTPUT);
        
        // Wait for serial connection to be established
        delay(1000);
    }
    
    void update() {
        if (!enabled) return;
        
        // Turn LED on
        digitalWrite(ledPin, HIGH);
        
        // Send a message through the serial connection
        Serial1.println("Hello from ESP32-S2!");
        
        // Wait for 500ms
        delay(500);
        
        // Turn LED off
        digitalWrite(ledPin, LOW);
        
        // Wait for 500ms
        delay(500);
    }
    
    void enable() {
        enabled = true;
    }
    
    void disable() {
        enabled = false;
        digitalWrite(ledPin, LOW);  // Turn off LED when disabled
    }
    
    bool isEnabled() {
        return enabled;
    }
};

#endif // BOARD_TEST_H 