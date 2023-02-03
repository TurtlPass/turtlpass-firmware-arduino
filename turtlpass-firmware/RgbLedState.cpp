#include "RgbLedState.h"

// Create the neopixel with the built in definitions NUM_NEOPIXEL and PIN_NEOPIXEL
Adafruit_NeoPixel neoPixel = Adafruit_NeoPixel(NUM_NEOPIXEL, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
Led RgbLedState::led;

/////////////////
// CONSTRUCTOR //
/////////////////

RgbLedState::RgbLedState() {}

//////////
// INIT //
//////////

void RgbLedState::init() {
  led.red = 0;
  led.green = 255;
  led.blue = 0;
  led.brightness = 255; // max
  led.fadeAmount = 3; // step
  led.blinkSpeed = 60; // in ms
  led.pulseSpeed = 6; // in ms
  led.lastUpdate = 1410065407; // max long
  led.state = LED_ON;

  neoPixel.begin();
  neoPixel.setPixelColor(0, neoPixel.Color(led.red, led.green, led.blue));
  neoPixel.setBrightness(led.brightness);
  neoPixel.show();
}

//////////////////
// UPDATE STATE //
//////////////////

void RgbLedState::setOn() {
  if (led.state != LED_ON) {
    led.state = LED_ON;
    led.brightness = 255;
  }
}

void RgbLedState::setOff() {
  if (led.state != LED_OFF) {
    led.state = LED_OFF;
    led.brightness = 0;
  }
}

void RgbLedState::setPulsing() {
  if (led.state != LED_PULSING) {
    led.brightness = 0;
    led.fadeAmount = 3;
    led.state = LED_PULSING;
  }
}

void RgbLedState::setBlinking() {
  if (led.state != LED_BLINKING) {
    led.state = LED_BLINKING;
  }
}

void RgbLedState::setColor(int colorIndex) {
  if (colorIndex < 0 || colorIndex >= NUM_COLORS) {
    return;
  }
  led.red = colors[colorIndex][0];
  led.green = colors[colorIndex][1];
  led.blue = colors[colorIndex][2];
}


//////////
// LOOP //
//////////

void RgbLedState::loop() {
  switch (led.state) {
    case LED_OFF: {
        led.brightness = 0;
        break;
      }
    case LED_BLINKING: {
        if (millis() - led.lastUpdate > led.blinkSpeed) {
          led.brightness = (led.brightness == 0) ? 255 : 0;
          led.lastUpdate = millis();
        }
        break;
      }
    case LED_PULSING: {
        if (millis() - led.lastUpdate > led.pulseSpeed) {
          led.brightness += led.fadeAmount;
          // reverse the direction of the fading at the ends of the fade
          if (led.brightness <= 0 || led.brightness >= 255) {
            led.fadeAmount = -led.fadeAmount;
          }
          led.lastUpdate = millis();
        }
        break;
      }
    default: { //case LED_ON:
        led.brightness = 255;
        break;
      }
  }
  neoPixel.setPixelColor(0, neoPixel.Color(led.red * led.brightness / 255, led.green * led.brightness / 255, led.blue * led.brightness / 255));
  neoPixel.show();
}
