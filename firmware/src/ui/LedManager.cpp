#include "ui/LedManager.h"
#include <cmath>
#include <Arduino.h>

// Predefined color table (0–255 PWM values)
const uint8_t LedManager::colors[NUM_COLORS][3] = {
    {0, 128, 0},     // 0: 🟢 green
    {255, 255, 0},   // 1: 🟡 yellow
    {255, 0, 0},     // 2: 🔴 red
    {0, 0, 255},     // 3: 🔵 blue
    {255, 255, 255}, // 4: ⚪ white
    {138, 43, 226},  // 5: 🟣 violet
    {255, 69, 0},    // 6: 🟠 orange
    {0, 255, 255},   // 7: 🩵 aqua
    {255, 20, 147},  // 8: 🩷 pink
};

LedManager::LedManager(ILedDriver* driver) : driver(driver) {}

void LedManager::begin() {
    if (driver) driver->begin();
}

void LedManager::loop() {
    uint8_t brightness = getNewBrightness();
    led.brightness = brightness;

    const uint8_t* c = colors[led.colorIndex];
    driver->setColor(c[0], c[1], c[2]);
    driver->setBrightness(brightness);
    driver->show();

    delay(1000 / LED_UPDATES_PER_SECOND); // frame rate
}

void LedManager::setColorIndex(uint8_t colorIndex) {
    if (colorIndex < NUM_COLORS) {
        led.colorIndex = colorIndex;
    }
}

uint8_t LedManager::getColorIndex() const {
    return led.colorIndex;
}

void LedManager::showNextColor() {
    led.colorIndex = (led.colorIndex + 1) % NUM_COLORS;
}

void LedManager::setOn() {
    led.brightness = MAX_BRIGHTNESS;
    led.state = LED_ON;
}

void LedManager::setOff() {
    led.brightness = 0;
    led.state = LED_OFF;
}

void LedManager::setPulsing() {
    led.brightness = 0;
    led.fadeAmount = FADE_AMOUNT;
    led.state = LED_PULSING;
}

void LedManager::setBlinking() {
    led.brightness = MAX_BRIGHTNESS;
    led.state = LED_BLINKING;
}

void LedManager::setFadeOutOnce(uint32_t intervalInSeconds) {
    led.brightness = MAX_BRIGHTNESS;
    led.fadeOutStartTime = millis();
    led.fadeOutDuration = intervalInSeconds * 1000;
    led.fadeAmount = 1;
    led.state = LED_FADE_OUT_ONCE;
    led.lastUpdate = 0;
}

void LedManager::setFadeOutLoop(uint32_t intervalInSeconds, uint32_t currentTime) {
    uint32_t elapsedSeconds = currentTime % intervalInSeconds;
    led.fadeOutStartTime = millis() - (elapsedSeconds * 1000);
    led.fadeOutDuration = intervalInSeconds * 1000;
    led.fadeAmount = FADE_AMOUNT;
    led.state = LED_FADE_OUT_LOOP;
}

uint8_t LedManager::getCurrentBrightness() const {
    return led.brightness;
}

int LedManager::mapExponentially(float value, float inMin, float inMax, float outMin, float outMax) {
    float normalized = (value - inMin) / (inMax - inMin);
    return (int)((outMax - outMin) * pow(normalized, 0.2f) + outMin);
}

uint8_t LedManager::getNewBrightness() {
    unsigned long now = millis();
    uint8_t brightness = led.brightness;

    switch (led.state) {
        case LED_OFF:
            brightness = 0;
            break;

        case LED_ON:
            brightness = MAX_BRIGHTNESS;
            break;

        case LED_BLINKING:
            if (now - led.lastUpdate > (unsigned long)led.blinkSpeed) {
                brightness = (brightness == 0) ? MAX_BRIGHTNESS : 0;
                led.lastUpdate = now;
            }
            break;

        case LED_PULSING:
            if (now - led.lastUpdate > (unsigned long)led.pulseSpeed) {
                brightness += led.fadeAmount;
                // reverse the direction of the fading at the ends of the fade
                if (brightness <= 0 || brightness >= MAX_BRIGHTNESS) {
                    led.fadeAmount = -led.fadeAmount;
                }
                led.lastUpdate = now;
            }
            break;

        case LED_FADE_OUT_ONCE: {
            if (led.brightness > 0) {
                unsigned long elapsedTime = now - led.fadeOutStartTime;
                if (elapsedTime >= led.fadeOutDuration) {
                    brightness = 0;  // fade complete
                } else {
                    brightness = mapExponentially(elapsedTime, 0, led.fadeOutDuration, MAX_BRIGHTNESS, 0);
                }
                led.lastUpdate = now;
            }
            break;
        }

        case LED_FADE_OUT_LOOP: {
            unsigned long elapsedTime = now - led.fadeOutStartTime;
            if (elapsedTime > led.fadeOutDuration) {
                led.fadeOutStartTime = now;
                elapsedTime = 0;
            }
            brightness = mapExponentially(elapsedTime, 0, led.fadeOutDuration, MAX_BRIGHTNESS, 0);
            led.lastUpdate = now;
            break;
        }

        default:
            break;
    }
    
    led.brightness = brightness;
    return brightness;
}
