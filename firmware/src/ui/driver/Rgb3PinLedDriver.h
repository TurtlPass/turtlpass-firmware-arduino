#pragma once

#include "ILedDriver.h"
#include <Arduino.h>

/**
 * @class Rgb3PinLedDriver
 * @brief RGB LED driver for 3 discrete PWM-controlled pins (R, G, B).
 */
class Rgb3PinLedDriver : public ILedDriver {
public:
    Rgb3PinLedDriver(uint8_t pinR, uint8_t pinG, uint8_t pinB);

    void begin() override;
    void setBrightness(uint8_t brightness) override;
    void setColor(uint8_t r, uint8_t g, uint8_t b) override;
    void show() override;

private:
    uint8_t pinR, pinG, pinB;
    uint8_t brightness;
    uint8_t currentR, currentG, currentB;
};
