#pragma once

#include "ILedDriver.h"
#include <Arduino.h>

/**
 * @class AnalogLedDriver
 * @brief ILedDriver implementation for single-color LEDs using analog/PWM output.
 *
 * This driver controls a single LED via analogWrite() (or digitalWrite()).
 * It ignores color, only brightness matters.
 */
class AnalogLedDriver : public ILedDriver {
public:
    /**
     * @param pin GPIO pin connected to the LED.
     * @param inverted True if logic is inverted (HIGH = off).
     */
    explicit AnalogLedDriver(uint8_t pin, bool inverted = false);

    void begin() override;
    void setBrightness(uint8_t brightness) override;
    void setColor(uint8_t r, uint8_t g, uint8_t b) override; // No-op
    void show() override;

private:
    uint8_t pin;
    bool inverted;
    uint8_t currentBrightness;
};
