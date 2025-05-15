# XIAO ESP32C3 Button and Battery Monitor

This project implements a simple button and battery monitoring system using the XIAO ESP32C3 microcontroller. It reads a button input and monitors battery voltage using a voltage divider circuit.

## Hardware Requirements

- XIAO ESP32C3 microcontroller
- Momentary push button
- 2x 10kΩ resistors (for voltage divider)
- Battery (connected to battery pins on bottom of XIAO)
- Jumper wires

## Battery Charging

The XIAO ESP32C3 includes a TP4056 lithium battery charging IC that can charge single-cell Li-ion/LiPo batteries:
- Connect a 3.7V Li-ion/LiPo battery to the battery pins on the bottom of the XIAO
- The board will automatically charge the battery when connected to USB
- Charging current is limited to 500mA by default
- The board includes overcharge and over-discharge protection
- LED indicators on the board show charging status:
  - Red LED: Charging in progress
  - Blue LED: Charging complete

## Battery Voltage Monitoring

The program monitors the battery voltage using a voltage divider circuit. For a 3.7V Li-ion/LiPo battery:
- Fully charged voltage: ~4.2V
- Nominal voltage: 3.7V
- Discharged voltage: ~3.0V (minimum safe voltage)

The voltage reading of 4.56V indicates that:
1. The battery is fully charged
2. The voltage divider ratio might need adjustment if the reading is consistently high

To calibrate the voltage reading:
1. Measure the actual battery voltage with a multimeter
2. Adjust the `VOLTAGE_DIVIDER_RATIO` in the code if needed
3. The current ratio is set to 2.0 for equal 10kΩ resistors

## Wiring

### Button Connection
- Connect one terminal of the button to pin D10
- Connect the other terminal to 3.3V
- The pin will read LOW when the button is not pressed and HIGH when pressed

### Battery Voltage Divider
- Connect the positive terminal of the battery to the battery pins on the bottom of the XIAO
- Create a voltage divider using two 10kΩ resistors:
  - Connect one 10kΩ resistor between battery positive and A0
  - Connect the second 10kΩ resistor between A0 and ground

## Software Setup

1. Install the Arduino IDE
2. Install the ESP32 board support package:
   - Open Arduino IDE
   - Go to Tools > Board > Boards Manager
   - Search for "ESP32"
   - Install "ESP32 by Espressif Systems"

## Compilation and Upload

1. Select the correct board:
   ```
   Tools > Board > ESP32 Arduino > XIAO ESP32C3
   ```

2. Select the correct port:
   ```
   Tools > Port > [Your XIAO's port]
   ```

3. Compile and upload:
   ```
   Sketch > Upload
   ```
   or press Ctrl+U (Cmd+U on Mac)

## Serial Monitor

To view the output:

1. Open the Serial Monitor:
   ```
   Tools > Serial Monitor
   ```
   or press Ctrl+Shift+M (Cmd+Shift+M on Mac)

2. Set the baud rate to 115200

## Output

The program will output:
- Battery voltage readings every second
- "Button pressed!" message when the button is pressed

## Code Structure

- `BUTTON_PIN`: Digital pin D10 for button input
- `BATTERY_PIN`: Analog pin A0 for battery voltage reading
- `VOLTAGE_DIVIDER_RATIO`: Set to 2.0 for equal 10kΩ resistors
- `REFERENCE_VOLTAGE`: ESP32C3 reference voltage (3.3V)

## Troubleshooting

1. If the button readings are erratic:
   - Check the button connections
   - Ensure the button is properly grounded
   - Try adjusting the debounce delay in the code

2. If battery readings are incorrect:
   - Verify the voltage divider resistor values
   - Check the battery voltage with a multimeter
   - Ensure proper connections to A0 and ground
