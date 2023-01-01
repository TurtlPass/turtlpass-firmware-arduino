#include "LedState.h"

enum State {
  LED_OFF = 0,
  LED_ON = 1,
  LED_PULSING = 2,
  LED_BLINKING = 3
};
enum State state = LED_ON;
int brightness = 0;
const int fadeAmountDefault = 3;
int fadeAmount = fadeAmountDefault;
int blinkState = LOW;
int blinkSpeed = 60; // in ms
int pulseSpeed = 6; // in ms
const long MAX_LONG = 1410065407;
unsigned long lastLedBlinkUpdateMillis = MAX_LONG;
unsigned long lastLedPulseUpdateMillis = MAX_LONG;


/////////////////
// CONSTRUCTOR //
/////////////////

LedState::LedState(byte pin) {
  this->pin = pin;
  init();
}

//////////
// INIT //
//////////

void LedState::init() {
  pinMode(pin, OUTPUT);
  setOn();
}

//////////////////
// UPDATE STATE //
//////////////////

void LedState::setOn() {
  if (state != LED_ON) {
    state = LED_ON;
    brightness = 255;
  }
}

void LedState::setOff() {
  if (state != LED_OFF) {
    state = LED_OFF;
    brightness = 0;
  }
}

void LedState::setPulsing() {
  if (state != LED_PULSING) {
    brightness = 0;
    fadeAmount = fadeAmountDefault;
    state = LED_PULSING;
  }
}

void LedState::setBlinking() {
  if (state != LED_BLINKING) {
    blinkState = LOW;
    state = LED_BLINKING;
  }
}

//////////
// LOOP //
//////////

void LedState::loop() {
  unsigned long now = millis();
  switch (state) {
    case LED_OFF: {
        brightness = 0;
        digitalWrite(pin, LOW);
        break;
      }
    case LED_BLINKING: {
        if (now - lastLedBlinkUpdateMillis > blinkSpeed) {
          if (blinkState == LOW) {
            blinkState = HIGH;
            digitalWrite(pin, HIGH);
          } else {
            blinkState = LOW;
            digitalWrite(pin, LOW);
          }
          lastLedBlinkUpdateMillis = now;
        }
        break;
      }
    case LED_PULSING: {
        if (now - lastLedPulseUpdateMillis > pulseSpeed) {

          analogWrite(pin, brightness);

          brightness = brightness + fadeAmount;

          // reverse the direction of the fading at the ends of the fade
          if (brightness <= 0 || brightness >= 255) {
            fadeAmount = -fadeAmount;
          }
          lastLedPulseUpdateMillis = now;
        }
        break;
      }
    default: { //case LED_ON:
        brightness = 255;
        digitalWrite(pin, HIGH);
        break;
      }
  }
}
