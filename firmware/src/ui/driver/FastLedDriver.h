#pragma once

#if defined(TP_RGB_LED) && (TP_RGB_LED)

#include "ILedDriver.h"
#include <FastLED.h>

/**
 * @class FastLedDriver
 * @brief LED driver using the FastLED library.
 * 
 * Templated on the data pin because FastLED requires the pin number
 * to be a compile-time constant. Supports configurable color order.
 *
 * @tparam DATA_PIN GPIO pin connected to the LED data line.
 * @tparam COLOR_ORDER Color order (default GRB).
 */
template<uint8_t DATA_PIN, EOrder COLOR_ORDER = GRB>
class FastLedDriver : public ILedDriver {
public:
    /**
     * @param numLeds Number of LEDs in the strip (default 1).
     */
    explicit FastLedDriver(uint16_t numLeds = 1)
        : numLeds(numLeds),
          leds(nullptr),
          currentColor(CRGB::Green),
          currentBrightness(255) {}

    /** Initialize FastLED and clear the strip. */
    void begin() override {
        leds = new CRGB[numLeds];
        FastLED.addLeds<WS2812, DATA_PIN, COLOR_ORDER>(leds, numLeds).setCorrection(TypicalSMD5050);
        FastLED.clear();
        FastLED.setBrightness(currentBrightness);
    }

    /** Set global brightness (0–255). */
    void setBrightness(uint8_t brightness) override {
        currentBrightness = brightness;
        FastLED.setBrightness(brightness);
    }

    /** Fill the strip with a single RGB color. */
    void setColor(uint8_t r, uint8_t g, uint8_t b) override {
        currentColor = CRGB(r, g, b);
        for (uint16_t i = 0; i < numLeds; ++i) {
            leds[i] = currentColor;
        }
    }

    /** Push the buffered color data to the LEDs. */
    void show() override {
        FastLED.show();
    }

private:
    uint16_t numLeds;
    CRGB* leds;
    CRGB currentColor;
    uint8_t currentBrightness;
};

#endif // conditions for FastLedDriver
