#include "Rgb3PinLedDriver.h"

Rgb3PinLedDriver::Rgb3PinLedDriver(uint8_t pinR, uint8_t pinG, uint8_t pinB)
    : pinR(pinR), pinG(pinG), pinB(pinB),
      brightness(255),
      currentR(0), currentG(0), currentB(0) {}

void Rgb3PinLedDriver::begin() {
    pinMode(pinR, OUTPUT);
    pinMode(pinG, OUTPUT);
    pinMode(pinB, OUTPUT);
    show();
}

void Rgb3PinLedDriver::setBrightness(uint8_t b) {
    brightness = b;
    show();
}

void Rgb3PinLedDriver::setColor(uint8_t r, uint8_t g, uint8_t b) {
    currentR = r;
    currentG = g;
    currentB = b;
}

void Rgb3PinLedDriver::show() {
    // Apply global brightness scaling
    uint16_t r = (currentR * brightness) / 255;
    uint16_t g = (currentG * brightness) / 255;
    uint16_t b = (currentB * brightness) / 255;

    analogWrite(pinR, r);
    analogWrite(pinG, g);
    analogWrite(pinB, b);
}
