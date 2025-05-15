#ifndef NEO_PIXEL_MANAGER_H
#define NEO_PIXEL_MANAGER_H

#include <Arduino.h>
#include <FastLED.h>

class NeoPixelManager {
private:
    static const uint8_t LED_PIN = 40;      // NeoPixel data pin
    static const uint8_t NUM_LEDS = 8;      // Number of NeoPixels
    static const uint8_t BRIGHTNESS = 20;   // Brightness (0-255)
    static const uint8_t MAX_POWER = 100;   // Maximum power in mA per LED
    
    bool enabled;
    bool initialized;
    bool powerSavingMode;
    unsigned long lastUpdate;
    const unsigned long UPDATE_INTERVAL = 100; // Update every 100ms
    
    CRGB leds[NUM_LEDS];  // Array to store LED states
    
    // Rainbow colors with reduced brightness
    const CRGB RAINBOW_COLORS[8] = {
        CRGB(64, 0, 0),      // Dim Red
        CRGB(64, 32, 0),     // Dim Orange
        CRGB(64, 64, 0),     // Dim Yellow
        CRGB(0, 64, 0),      // Dim Green
        CRGB(0, 0, 64),      // Dim Blue
        CRGB(32, 0, 64),     // Dim Indigo
        CRGB(64, 0, 64),     // Dim Violet
        CRGB(32, 0, 32)      // Dim Purple
    };

    // Power management functions
    void setPowerLimit() {
        // Set power limit to prevent brownouts
        FastLED.setMaxPowerInVoltsAndMilliamps(3, MAX_POWER * NUM_LEDS);
    }
    
    void updatePowerSaving() {
        if (!powerSavingMode) return;
        
        // Reduce brightness during power saving mode
        FastLED.setBrightness(BRIGHTNESS / 2);
    }

public:
    NeoPixelManager() : enabled(false), initialized(false),
                       powerSavingMode(true), lastUpdate(0) {}
    
    void begin() {
        if (!enabled || initialized) return;
        
        // Initialize FastLED with power management
        FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
        setPowerLimit();
        FastLED.setBrightness(BRIGHTNESS);
        
        // Set initial rainbow pattern
        for (int i = 0; i < NUM_LEDS; i++) {
            leds[i] = RAINBOW_COLORS[i];
        }
        FastLED.show();
        
        initialized = true;
        Serial1.println("NeoPixelManager: begin: Initialized");
    }
    
    void update() {
        if (!enabled || !initialized) return;
        
        // Only update LEDs periodically to save power
        unsigned long currentMillis = millis();
        if (currentMillis - lastUpdate >= UPDATE_INTERVAL) {
            lastUpdate = currentMillis;
            updatePowerSaving();
            FastLED.show();
        }
    }
    
    void enable() {
        if (enabled) return;
        enabled = true;
        Serial1.println("NeoPixelManager: enable: Enabled");
    }
    
    void disable() {
        if (!enabled) return;
        enabled = false;
        initialized = false;
        
        // Turn off all LEDs
        FastLED.clear();
        FastLED.show();
        
        Serial1.println("NeoPixelManager: disable: Disabled");
    }
    
    bool isEnabled() {
        return enabled;
    }
    
    // Set a specific LED to a specific color
    void setLED(uint8_t index, CRGB color) {
        if (index < NUM_LEDS) {
            leds[index] = color;
            FastLED.show();
        }
    }
    
    // Set all LEDs to a specific color
    void setAll(CRGB color) {
        for (int i = 0; i < NUM_LEDS; i++) {
            leds[i] = color;
        }
        FastLED.show();
    }
    
    // Reset to rainbow pattern
    void setRainbow() {
        for (int i = 0; i < NUM_LEDS; i++) {
            leds[i] = RAINBOW_COLORS[i];
        }
        FastLED.show();
    }
    
    // Enable/disable power saving mode
    void setPowerSaving(bool enable) {
        powerSavingMode = enable;
        if (powerSavingMode) {
            FastLED.setBrightness(BRIGHTNESS / 2);
        } else {
            FastLED.setBrightness(BRIGHTNESS);
        }
        FastLED.show();
    }
    
    // Get current power saving mode
    bool isPowerSaving() {
        return powerSavingMode;
    }
};

#endif // NEO_PIXEL_MANAGER_H 