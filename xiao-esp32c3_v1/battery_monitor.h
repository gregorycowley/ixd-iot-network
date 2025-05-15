#ifndef BATTERY_MONITOR_H
#define BATTERY_MONITOR_H

#include <Arduino.h>
#include "pin_definitions.h"

// Battery status structure
struct BatteryStatus {
  float voltage;
  int percentage;
  bool isLow;
};

BatteryStatus batteryStatus = {0.0, 0, false};

// Function to read battery voltage
float readBatteryVoltage() {
  #if defined(ARDUINO_XIAO_ESP32S3)
    // Read ADC value
    int adcValue = analogRead(batteryPin);
    // Convert to voltage (ESP32S3 ADC is 12-bit, reference voltage is 3.3V)
    float voltage = (adcValue * 3.3) / 4095.0;
    // Apply voltage divider ratio
    return voltage * VOLTAGE_DIVIDER_RATIO;
  #else
    return 0.0;  // No battery monitoring on ESP32C3
  #endif
}

// Function to calculate battery percentage
int calculateBatteryPercentage(float voltage) {
  if (voltage <= MIN_BATTERY_VOLTAGE) return 0;
  if (voltage >= MAX_BATTERY_VOLTAGE) return 100;
  
  return (int)((voltage - MIN_BATTERY_VOLTAGE) * 100.0 / 
               (MAX_BATTERY_VOLTAGE - MIN_BATTERY_VOLTAGE));
}

// Function to update battery status
void updateBatteryStatus() {
  #if defined(ARDUINO_XIAO_ESP32S3)
    batteryStatus.voltage = readBatteryVoltage();
    batteryStatus.percentage = calculateBatteryPercentage(batteryStatus.voltage);
    batteryStatus.isLow = batteryStatus.percentage < 20;  // Consider battery low below 20%
  #endif
}

// Function to setup battery monitoring
void setupBatteryMonitor() {
  #if defined(ARDUINO_XIAO_ESP32S3)
    analogReadResolution(12);  // Set ADC resolution to 12 bits
    analogSetAttenuation(ADC_11db);  // Set ADC attenuation for 0-3.3V range
  #endif
}

#endif // BATTERY_MONITOR_H 