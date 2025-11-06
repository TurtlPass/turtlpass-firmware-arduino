#pragma once
#include "ILedDriver.h"

/**
 * @class NullLedDriver
 * @brief Dummy LED driver used when no hardware LED is available.
 *
 * All methods are no-ops, ensuring code compiles and runs safely
 * even on unsupported boards.
 */
class NullLedDriver : public ILedDriver {
public:
    void begin() override {}
    void setBrightness(uint8_t) override {}
    void setColor(uint8_t, uint8_t, uint8_t) override {}
    void show() override {}
};
