#include "Cyw43LedDriver.h"

void Cyw43LedDriver::begin() {
#if defined(CYW43_WL_GPIO_LED_PIN)
    // Initialize the Wi-Fi subsystem (required for onboard LED)
    cyw43_arch_init();
    setBrightness(0);  // start with LED off
#endif
}

void Cyw43LedDriver::setBrightness(uint8_t brightness) {
#if defined(CYW43_WL_GPIO_LED_PIN)
    // Treat any non-zero brightness as LED on
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, brightness > 0);
#endif
}

void Cyw43LedDriver::setColor(uint8_t, uint8_t, uint8_t) {
    // No-op — mono LED ignores color
}

void Cyw43LedDriver::show() {
    // No-op — LED state is applied immediately in setBrightness
}
