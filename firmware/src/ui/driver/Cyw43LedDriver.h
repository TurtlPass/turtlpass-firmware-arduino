#pragma once

#include "ILedDriver.h"

#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif

/**
 * @class Cyw43LedDriver
 * @brief Mono LED driver for Raspberry Pi Pico W onboard LED.
 *
 * The LED is controlled via the CYW43 Wi-Fi chip. Color is ignored; only brightness matters.
 */
class Cyw43LedDriver : public ILedDriver {
public:
    Cyw43LedDriver() = default;
    virtual ~Cyw43LedDriver() = default; // important for vtable

    void begin() override;
    void setBrightness(uint8_t brightness) override;
    void setColor(uint8_t r, uint8_t g, uint8_t b) override;
    void show() override;
};
