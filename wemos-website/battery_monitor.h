#ifndef BATTERY_MONITOR_H
#define BATTERY_MONITOR_H

#include <Arduino.h>

// Voltage divider constants
const float R1 = 10000.0;  // 10k ohm resistor
const float R2 = 10000.0;  // 10k ohm resistor
const float VOLTAGE_DIVIDER_RATIO = (R1 + R2) / R2;  // Calculate voltage divider ratio

// ADC constants
const float ADC_REF_VOLTAGE = 3.3;  // ESP8266 ADC reference voltage
const int ADC_RESOLUTION = 1023;    // 10-bit ADC resolution

// Battery monitoring pin
const int BATTERY_PIN = A0;

// Function to read raw ADC value
int readBatteryRaw() {
    return analogRead(BATTERY_PIN);
}

// Function to calculate actual battery voltage
float readBatteryVoltage() {
    int rawValue = readBatteryRaw();
    float voltage = (rawValue * ADC_REF_VOLTAGE) / ADC_RESOLUTION;
    return voltage * VOLTAGE_DIVIDER_RATIO;
}

// Function to get battery percentage (assuming 3.7V LiPo battery)
// 3.7V = 100%, 3.3V = 0%
float getBatteryPercentage() {
    float voltage = readBatteryVoltage();
    float percentage = ((voltage - 3.3) / (3.7 - 3.3)) * 100.0;
    return constrain(percentage, 0.0, 100.0);  // Constrain between 0-100%
}

#endif // BATTERY_MONITOR_H 