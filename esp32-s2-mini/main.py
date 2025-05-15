from machine import Pin, deepsleep, wake_reason
import time

# Define button pins
BUTTON_PINS = [1, 2, 4, 6, 8, 10]
buttons = []

# Initialize buttons with external pull-down resistors
for pin in BUTTON_PINS:
    buttons.append(Pin(pin, Pin.IN))  # No internal pull-up since we have external pull-downs

# Button messages
messages = [
    "Button 1 pressed!",
    "Button 3 pressed!",
    "Button 4 pressed!",
    "Button 5 pressed!",
    "Button 6 pressed!"
]

# Check wake reason
wake_reason_code = wake_reason()
if wake_reason_code == 1:  # Wake from deep sleep
    print("Woke up from deep sleep!")
else:
    print("Button input program started. Press any button to see a message.")

# Main loop
while True:
    button_pressed = False
    for i, button in enumerate(buttons):
        if button.value() == 1:  # Button is pressed (active high)
            print(messages[i])
            time.sleep(0.2)  # Debounce delay
            button_pressed = True
    
    if not button_pressed:
        print("No button pressed. Entering deep sleep mode...")
        # Configure wake-up sources
        # Wake up on any button press (GPIO 1, 2, 4, 6, 8, 10)
        for pin in BUTTON_PINS:
            Pin(pin, Pin.IN).irq(trigger=Pin.IRQ_RISING, wake=deep_sleep)  # Rising edge for active-high
        
        # Enter deep sleep indefinitely until button press
        deepsleep()  # No timeout parameter means sleep until wake-up source triggers
    
    time.sleep(0.01)  # Small delay to prevent CPU hogging 