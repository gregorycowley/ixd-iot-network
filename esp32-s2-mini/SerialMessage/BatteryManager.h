#ifndef BATTERY_MANAGER_H
#define BATTERY_MANAGER_H

#include <Arduino.h>

class BatteryManager {
private:
    bool enabled;
    bool initialized;
    const uint8_t BATTERY_PIN = 3;  // Analog input pin for battery voltage
    const float VOLTAGE_DIVIDER_RATIO = 1.0;  // R1/(R1+R2) - adjusted for actual voltage divider
    const float MAX_BATTERY_VOLTAGE = 3.2;    // Maximum voltage for 2 fresh AA batteries (1.6V each)
    const float MIN_BATTERY_VOLTAGE = 2.0;    // Minimum voltage for 2 depleted AA batteries (1.0V each)
    const float ADC_REFERENCE_VOLTAGE = 3.3;  // ESP32-S2 ADC reference voltage
    const int ADC_RESOLUTION = 4095;          // ESP32-S2 ADC resolution (12-bit)
    
    unsigned long lastUpdate;
    const unsigned long updateInterval = 1000; // Update every second
    
    float currentVoltage;
    int currentPercentage;
    
    void updateBatteryLevel() {
        // Read the analog value
        int rawValue = analogRead(BATTERY_PIN);
        
        // Convert to voltage
        float voltage = (rawValue * ADC_REFERENCE_VOLTAGE) / ADC_RESOLUTION;
        
        // Apply voltage divider ratio
        voltage *= VOLTAGE_DIVIDER_RATIO;
        
        // Calculate percentage
        int percentage = map(voltage * 100, 
                           MIN_BATTERY_VOLTAGE * 100, 
                           MAX_BATTERY_VOLTAGE * 100, 
                           0, 100);
        
        // Constrain percentage between 0 and 100
        percentage = constrain(percentage, 0, 100);
        
        // Update values
        currentVoltage = voltage;
        currentPercentage = percentage;
        
        // Print debug information
        // Serial1.print("Battery: ");
        // Serial1.print(voltage);
        // Serial1.print("V (");
        // Serial1.print(percentage);
        // Serial1.println("%)");
    }

public:
    BatteryManager() : enabled(false), initialized(false), 
                      currentVoltage(0.0), currentPercentage(0) {}
    
    void begin() {
        if (!enabled || initialized) return;
        
        // Configure ADC
        analogReadResolution(12);  // Set ADC resolution to 12 bits
        analogSetAttenuation(ADC_11db);  // Set attenuation for 0-3.3V range
        
        initialized = true;
        Serial1.println("BatteryManager: begin: Initialized");
    }
    
    void update() {
        if (!enabled || !initialized) return;
        
        unsigned long currentMillis = millis();
        if (currentMillis - lastUpdate >= updateInterval) {
            lastUpdate = currentMillis;
            updateBatteryLevel();
        }
    }
    
    void enable() {
        if (enabled) return;
        enabled = true;
        Serial1.println("BatteryManager: enable: Enabled");
    }
    
    void disable() {
        if (!enabled) return;
        enabled = false;
        initialized = false;
        Serial1.println("BatteryManager: disable: Disabled");
    }
    
    bool isEnabled() {
        return enabled;
    }
    
    float getVoltage() {
        return currentVoltage;
    }
    
    int getPercentage() {
        return currentPercentage;
    }
    
    bool isLow() {
        return currentPercentage <= 20;  // Consider battery low at 20% or less
    }
    
    bool isCritical() {
        return currentPercentage <= 10;  // Consider battery critical at 10% or less
    }
};

#endif // BATTERY_MANAGER_H 