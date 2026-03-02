#pragma once

// CST816D capacitive touch driver (I2C)
// SDA=6, SCL=7, INT=5, RST=13, addr=0x15

#include <Arduino.h>
#include <Wire.h>

#define CST816D_ADDR    0x15
#define CST816D_SDA     6
#define CST816D_SCL     7
#define CST816D_INT     5
#define CST816D_RST     13

struct TouchPoint {
    uint16_t x;
    uint16_t y;
    uint8_t  gesture;  // 0=none,1=slideDown,2=slideUp,3=slideLeft,4=slideRight,5=tap,11=doubleTap,12=longPress
    bool     touched;
};

class CST816D {
public:
    CST816D() {}

    void begin() {
        pinMode(CST816D_RST, OUTPUT);
        digitalWrite(CST816D_RST, LOW);
        delay(20);
        digitalWrite(CST816D_RST, HIGH);
        delay(50);

        Wire.begin(CST816D_SDA, CST816D_SCL);
    }

    // Returns true if a touch event is available
    bool read(TouchPoint &tp) {
        Wire.beginTransmission(CST816D_ADDR);
        Wire.write(0x01);  // gesture register
        if (Wire.endTransmission(false) != 0) {
            tp.touched = false;
            return false;
        }

        Wire.requestFrom(CST816D_ADDR, 6);
        if (Wire.available() < 6) {
            tp.touched = false;
            return false;
        }

        tp.gesture = Wire.read();
        uint8_t fingers = Wire.read();
        uint8_t xh = Wire.read();
        uint8_t xl = Wire.read();
        uint8_t yh = Wire.read();
        uint8_t yl = Wire.read();

        tp.x       = ((xh & 0x0F) << 8) | xl;
        tp.y       = ((yh & 0x0F) << 8) | yl;
        tp.touched = (fingers > 0);
        return tp.touched;
    }
};
