#pragma once

#include <stdint.h>

class ILedDriver {
public:
    virtual ~ILedDriver() = default;

    /// Initialize the LED hardware.
    virtual void begin() = 0;

    /// Set brightness (0–255).
    virtual void setBrightness(uint8_t brightness) = 0;

    /// Optionally set color (for RGB LEDs).
    virtual void setColor(uint8_t r, uint8_t g, uint8_t b) = 0;

    /// Commit any pending updates (e.g. FastLED.show()).
    virtual void show() = 0;
};
