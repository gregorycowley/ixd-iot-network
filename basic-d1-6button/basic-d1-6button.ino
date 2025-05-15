#include "pin_definitions.h"

// Variables to store button states
int buttonStates[numButtons] = {0};
int lastButtonStates[numButtons] = {0};
unsigned long lastDebounceTimes[numButtons] = {0};
unsigned long lastDebugTime = 0;
bool hardwareTestMode = false;  // Temporarily disabled for debugging

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  Serial.println("\n\n=== 6-Button Controller Setup ===");
  
  // Set all button pins as inputs (using external pull-down resistors)
  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT);
  pinMode(button3Pin, INPUT);
  pinMode(button4Pin, INPUT);
  pinMode(button5Pin, INPUT);
  pinMode(button6Pin, INPUT);
  
  Serial.println("All buttons initialized");
  Serial.println("Starting main loop...");
}

void loop() {
  // Simple continuous reading of all buttons
  Serial.print("Button States: ");
  Serial.print(digitalRead(button1Pin));
  Serial.print(" ");
  Serial.print(digitalRead(button2Pin));
  Serial.print(" ");
  Serial.print(digitalRead(button3Pin));
  Serial.print(" ");
  Serial.print(digitalRead(button4Pin));
  Serial.print(" ");
  Serial.print(digitalRead(button5Pin));
  Serial.print(" ");
  Serial.println(digitalRead(button6Pin));
  
  delay(100);  // Small delay to prevent overwhelming the serial output
}