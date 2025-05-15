#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels. Some are 32.
                         // For Grove OLED on Seeed Expansion board, it's often 64.

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The I2C address for these displays is usually 0x3C.
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Define the built-in LED pin. 
// On the XIAO nRF52840, LED_BUILTIN is typically the red LED.
const int ledPin = LED_BUILTIN; 

// Define the analog pin for the pressure sensor
const int pressureSensorPin = A0;

// Define a threshold for pressure detection (0-1023 for 10-bit ADC)
// This value will likely need calibration based on your sensor and setup.
const int pressureThreshold = 500; 

void setup() {
  Serial.begin(9600);
  // while (!Serial); // Wait for serial port to connect. Needed for native USB port only

  // Initialize the I2C interface for the OLED
  Wire.begin();

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  // display.drawPixel(10, 10, SSD1306_WHITE);

  // Display "Hello, OLED!"
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0,0);     // Start at top-left corner
  display.println(F("Hello, OLED!"));
  
  display.display(); // Actually draw buffer to screen
  delay(100);

  // Initialize the digital pin for the LED as an output.
  pinMode(ledPin, OUTPUT);
  // Ensure LED is off initially
  digitalWrite(ledPin, HIGH); 
}

void loop() {
  // Read the value from the pressure sensor
  int sensorValue = analogRead(pressureSensorPin);

  // Print the sensor value to the Serial Monitor for calibration
  Serial.print("Sensor Value: ");
  Serial.println(sensorValue);

  // Check if force is applied (sensor value is above the threshold)
  if (sensorValue > pressureThreshold) {
    digitalWrite(ledPin, LOW);   // Turn the LED ON (remember, LOW is ON for this board's LED)
    Serial.println("Pressure detected - LED ON");
  } else {
    digitalWrite(ledPin, HIGH);  // Turn the LED OFF
    // Serial.println("No pressure - LED OFF"); // Optional: print when no pressure
  }

  // Small delay to make Serial Monitor readable and debounce sensor a bit
  delay(100);

  // The OLED message is static, so nothing else to do in the loop for this example.
  // You could add dynamic content here, like displaying the sensorValue.
}
