#include <Arduino.h>
#include "TFT_eSPI.h"

TFT_eSPI tft;
int currentPage = 0;

// Define buttons
const int BTN_1 = WIO_KEY_A;
const int BTN_2 = WIO_KEY_B;
const int BTN_3 = WIO_KEY_C;

// PC State Structure
struct PCStats {
    int cpuUsage = 0;
    int ramUsage = 0;
    float cpuTemp = 0.0;
    int diskUsage = 0;
    int gpuUsage = 0;
    float gpuTemp = 0.0;
    char ipAddress[16] = "0.0.0.0";
    unsigned long lastUpdate = 0;
} pcStats;

// Drawing Functions
void drawBox(int x, int y, int w, int h) {
    tft.drawRect(x, y, w, h, TFT_GREEN);
}

void drawTitle(const char* title) {
    tft.drawString("<<", 10, 10);
    tft.drawString(title, 40, 10);
    tft.drawString(">>", 270, 10);
    drawBox(5, 5, 310, 230);
    tft.drawLine(10, 30, 300, 30, TFT_GREEN);
}

// [Other drawing functions...]

void setup() {
    Serial.begin(115200);
    tft.begin();
    tft.setRotation(3);
    tft.setTextSize(2);
    
    pinMode(BTN_1, INPUT_PULLUP);
    pinMode(BTN_2, INPUT_PULLUP);
    pinMode(BTN_3, INPUT_PULLUP);
    
    drawPage(currentPage);
}

void loop() {
    // Receive and parse data
    if (Serial.available()) {
        String data = Serial.readStringUntil('\n');
        // Parse data...
    }
    
    // Button handling
    if (digitalRead(BTN_1) == LOW) {
        currentPage = 0;
        drawPage(currentPage);
        delay(200);
    }
    // [Other button handling...]

    // Update display
    if (currentPage == 0) {
        drawSystemStatus();
        delay(100);
    }
}