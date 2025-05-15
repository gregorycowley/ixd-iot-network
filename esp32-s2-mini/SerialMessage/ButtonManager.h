#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

#include <Arduino.h>

class ButtonManager {
private:
    bool enabled;
    bool initialized;
    const uint8_t BUTTON_PIN = 39;  // Button input pin
    const unsigned long DEBOUNCE_DELAY = 50;  // Debounce time in milliseconds
    
    bool lastButtonState;    // Previous button state
    bool buttonState;        // Current button state
    bool lastDebouncedState; // Previous debounced state
    bool debouncedState;     // Current debounced state
    unsigned long lastDebounceTime;  // Last time the button state changed
    
    // Callback function type for button events
    typedef void (*ButtonCallback)(bool state);
    ButtonCallback onButtonChange;
    
    void updateButtonState() {
        // Read the current button state (LOW when idle, HIGH when pressed)
        bool reading = digitalRead(BUTTON_PIN);
        
        // Check if the button state has changed
        if (reading != lastButtonState) {
            lastDebounceTime = millis();
        }
        
        // If the button state has been stable for longer than the debounce delay
        if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
            // If the debounced state has changed
            if (reading != debouncedState) {
                debouncedState = reading;
                
                // If the button state has changed and we have a callback
                if (debouncedState != lastDebouncedState && onButtonChange != nullptr) {
                    onButtonChange(debouncedState);
                }
            }
        }
        
        // Save the current state for next time
        lastButtonState = reading;
        lastDebouncedState = debouncedState;
    }

public:
    ButtonManager() : enabled(false), initialized(false),
                     lastButtonState(false), buttonState(false),
                     lastDebouncedState(false), debouncedState(false),
                     lastDebounceTime(0), onButtonChange(nullptr) {}
    
    void begin() {
        if (!enabled || initialized) return;
        
        // Set pin as output and set it LOW
        pinMode(BUTTON_PIN, OUTPUT);
        digitalWrite(BUTTON_PIN, LOW);
        delay(10);  // Give time for the pin to settle
        
        // Now switch to input mode with pull-down
        pinMode(BUTTON_PIN, INPUT_PULLDOWN);
        
        // Read initial state
        lastButtonState = digitalRead(BUTTON_PIN);
        debouncedState = lastButtonState;
        lastDebouncedState = debouncedState;
        
        initialized = true;
        Serial1.println("ButtonManager: begin: Initialized");
    }
    
    void setPinLow() {
        pinMode(BUTTON_PIN, OUTPUT);
        digitalWrite(BUTTON_PIN, LOW);
        Serial1.println("ButtonManager: Pin set to LOW");
    }
    
    void update() {
        if (!enabled || !initialized) {
            Serial1.println("ButtonManager: update: Not enabled or initialized");
            return;
        }
        updateButtonState();
    }
    
    void enable() {
        if (enabled) return;
        enabled = true;
        Serial1.println("ButtonManager: enable: Enabled");
    }
    
    void disable() {
        if (!enabled) return;
        enabled = false;
        initialized = false;
        Serial1.println("ButtonManager: disable: Disabled");
    }
    
    bool isEnabled() {
        return enabled;
    }
    
    bool isPressed() {
        return debouncedState;
    }
    
    void setCallback(ButtonCallback callback) {
        onButtonChange = callback;
        Serial1.println("ButtonManager: Callback set");
    }
    
    void clearCallback() {
        onButtonChange = nullptr;
        Serial1.println("ButtonManager: Callback cleared");
    }
};

#endif // BUTTON_MANAGER_H 