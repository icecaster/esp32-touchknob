#pragma once

// Rotary encoder driver
// CLK=45, DT=42, SW=41

#include <Arduino.h>

#define ENC_CLK 45
#define ENC_DT  42
#define ENC_SW  41

class Encoder {
public:
    volatile int  delta     = 0;   // accumulated steps (+/-)
    volatile bool pressed   = 0;   // button pressed flag (cleared after read)

    void begin() {
        pinMode(ENC_CLK, INPUT_PULLUP);
        pinMode(ENC_DT,  INPUT_PULLUP);
        pinMode(ENC_SW,  INPUT_PULLUP);

        attachInterrupt(digitalPinToInterrupt(ENC_CLK), []() {
            instance->onClk();
        }, RISING);

        attachInterrupt(digitalPinToInterrupt(ENC_SW), []() {
            instance->onSw();
        }, FALLING);

        instance = this;
    }

    // Returns steps since last call (positive = CW, negative = CCW)
    int readDelta() {
        int d = delta;
        delta = 0;
        return d;
    }

    // Returns true once per press, then resets
    bool readPress() {
        if (pressed) { pressed = false; return true; }
        return false;
    }

private:
    static Encoder *instance;

    void IRAM_ATTR onClk() {
        static uint32_t lastTime = 0;
        uint32_t now = millis();
        if (now - lastTime < 5) return;  // debounce
        lastTime = now;
        delta += (digitalRead(ENC_DT) == LOW) ? 1 : -1;
    }

    void IRAM_ATTR onSw() {
        static uint32_t lastTime = 0;
        uint32_t now = millis();
        if (now - lastTime < 20) return;  // debounce
        lastTime = now;
        pressed = true;
    }
};

Encoder *Encoder::instance = nullptr;
