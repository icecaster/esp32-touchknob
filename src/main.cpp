#include <Arduino.h>
#include "display.h"
#include "CST816D.h"
#include "encoder.h"
#include <Adafruit_NeoPixel.h>

// --- Hardware ---
LGFX         tft;
CST816D      touch;
Encoder      encoder;

#define NEOPIXEL_PIN 48
#define NEOPIXEL_NUM 5
Adafruit_NeoPixel pixels(NEOPIXEL_NUM, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

#define LED_PWR 40   // power indicator LED (active HIGH)

// --- State ---
int value = 0;   // current knob value

void drawKnobUI() {
    int cx = 120, cy = 120, r = 100;
    tft.fillScreen(TFT_BLACK);

    // Outer ring
    tft.drawCircle(cx, cy, r, TFT_WHITE);

    // Value arc (simple tick marks, 0–9)
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(cx - 12, cy - 12);
    tft.printf("%3d", value);
}

void setup() {
    Serial.begin(115200);

    // Power LED
    pinMode(LED_PWR, OUTPUT);
    digitalWrite(LED_PWR, HIGH);

    // Display
    tft.init();
    tft.setRotation(0);
    tft.setBrightness(255);
    tft.fillScreen(TFT_BLACK);

    // Touch
    touch.begin();

    // Encoder
    encoder.begin();

    // NeoPixels
    pixels.begin();
    pixels.setBrightness(30);
    for (int i = 0; i < NEOPIXEL_NUM; i++) pixels.setPixelColor(i, pixels.Color(0, 0, 64));
    pixels.show();

    drawKnobUI();
    Serial.println("CrowPanel 1.28\" ready");
}

void loop() {
    // Rotary encoder
    int d = encoder.readDelta();
    if (d != 0) {
        value = constrain(value + d, 0, 100);
        drawKnobUI();
        Serial.printf("Value: %d\n", value);
    }

    if (encoder.readPress()) {
        Serial.println("Knob pressed");
        value = 0;
        drawKnobUI();
    }

    // Touch
    TouchPoint tp;
    if (touch.read(tp)) {
        Serial.printf("Touch: (%d, %d) gesture=%d\n", tp.x, tp.y, tp.gesture);
    }

    delay(10);
}
