// Wemos D1 Button Check
// This sketch monitors all GPIO pins for button presses
// Buttons should be connected with pulldown resistors

// Array of all available GPIO pins on Wemos D1
const int buttonPins[] = {D0, D1, D2, D3, D4, D5, D6, D7, D8};
const int numButtons = sizeof(buttonPins) / sizeof(buttonPins[0]);

// Array to store previous button states
bool previousButtonStates[9] = {false};

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  Serial.println("Wemos D1 Button Check");
  Serial.println("Monitoring all GPIO pins for button presses...");
  
  // Initialize all button pins as inputs
  for (int i = 0; i < numButtons; i++) {
    pinMode(buttonPins[i], INPUT);
  }
}

void loop() {
  // Check each button
  for (int i = 0; i < numButtons; i++) {
    bool currentState = digitalRead(buttonPins[i]);
    
    // If button state changed from LOW to HIGH (button pressed)
    if (currentState == HIGH && previousButtonStates[i] == false) {
      Serial.print("Button on pin D");
      Serial.print(i);
      Serial.println(" pressed!");
    }
    
    // Update previous state
    previousButtonStates[i] = currentState;
  }
  
  // Small delay to prevent bouncing
  delay(50);
}
