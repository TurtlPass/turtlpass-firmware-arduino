#include "AnalogLedDriver.h"

AnalogLedDriver::AnalogLedDriver(uint8_t pin, bool inverted)
    : pin(pin), inverted(inverted), currentBrightness(0) {}

void AnalogLedDriver::begin() {
    pinMode(pin, OUTPUT);
    // Set initial state (off)
    digitalWrite(pin, inverted ? HIGH : LOW);
}

void AnalogLedDriver::setBrightness(uint8_t brightness) {
    currentBrightness = brightness;
}

void AnalogLedDriver::setColor(uint8_t, uint8_t, uint8_t) {
    // No-op — mono LED doesn't support color
}

void AnalogLedDriver::show() {
    // Write brightness to PWM pin
    uint8_t value = inverted ? (255 - currentBrightness) : currentBrightness;
    analogWrite(pin, value);
}
