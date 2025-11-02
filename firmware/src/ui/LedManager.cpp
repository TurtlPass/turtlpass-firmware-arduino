#include "ui/LedManager.h"

LedManager::LedManager() {
    led.colorIndex = 0;
    led.brightness = DEFAULT_BRIGHTNESS;
    led.fadeAmount = DEFAULT_FADE_AMOUNT;  // step
    led.pulseSpeed = 6;                    // in ms
    led.blinkSpeed = 60;                   // in ms
    led.blinkState = LOW;
    led.lastUpdate = MAX_LONG;
    led.state = LED_ON;
}

void LedManager::begin() {
  if (LED_IS_RGB) {
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
    FastLED.setBrightness(DEFAULT_BRIGHTNESS);
  } else {
    pinMode(LED_PIN, OUTPUT);
  }
}

void LedManager::loop() {
  if (LED_IS_RGB) {
    leds[0] = colors[led.colorIndex];
    FastLED.setBrightness(getNewBrightness());
    FastLED.show();
  } else {
    analogWrite(LED_PIN, getNewBrightness());
  }
  delay(1000 / LED_UPDATES_PER_SECOND);
}

void LedManager::setColorIndex(uint8_t colorIndex) {
  if (!LED_IS_RGB) return;
  led.colorIndex = colorIndex;
  leds[0] = colors[colorIndex];
}

uint8_t LedManager::getColorIndex() {
  return led.colorIndex;
}

void LedManager::showNextColor() {
  if (!LED_IS_RGB) return;
  if (led.colorIndex >= NUM_COLORS - 1) {
    setColorIndex(0);
  } else {
    setColorIndex(led.colorIndex + 1);
  }
}

void LedManager::setOn() {
  if (led.state != LED_ON) {
    led.brightness = 255;
    led.state = LED_ON;
  }
}

void LedManager::setOff() {
  if (led.state != LED_OFF) {
    led.brightness = 0;
    led.state = LED_OFF;
  }
}

void LedManager::setPulsing() {
  if (led.state != LED_PULSING) {
    led.brightness = 0;
    led.fadeAmount = DEFAULT_FADE_AMOUNT;
    led.state = LED_PULSING;
  }
}

void LedManager::setBlinking() {
  if (led.state != LED_BLINKING) {
    led.brightness = DEFAULT_BRIGHTNESS;
    led.state = LED_BLINKING;
  }
}

void LedManager::setFadeOutOnce(uint32_t intervalInSeconds) {
  if (led.state != LED_FADE_OUT_ONCE) {
    led.brightness = DEFAULT_BRIGHTNESS;
    led.fadeOutStartTime = millis();
    led.fadeOutDuration = intervalInSeconds * 1000;
    led.fadeAmount = 1;
    led.state = LED_FADE_OUT_ONCE;
  }
}

void LedManager::setFadeOutLoop(uint32_t intervalInSeconds, uint32_t currentTime) {
  if (led.state != LED_FADE_OUT_LOOP) {
    uint32_t elapsedSeconds = currentTime % intervalInSeconds;
    led.fadeOutStartTime = millis() - (elapsedSeconds * 1000);
    led.fadeOutDuration = intervalInSeconds * 1000;
    led.fadeAmount = DEFAULT_FADE_AMOUNT;
    led.state = LED_FADE_OUT_LOOP;
  }
}

/**
   Maps a value from one range to another range, using an exponential curve.

   A higher exponent will result in a faster initial decrease in brightness,
   and a slower decrease as the brightness approaches zero.
   Conversely, a lower exponent will result in a slower initial decrease in brightness,
   and a faster decrease as the brightness approaches zero.
*/
int LedManager::mapExponentially(float value, float inMin, float inMax, float outMin, float outMax) {
  return (int)(outMax - outMin) * pow((value - inMin) / (inMax - inMin), 0.2) + outMin;
}

uint8_t LedManager::getCurrentBrightness() {
  return led.brightness;
}

uint8_t LedManager::getNewBrightness() {
  unsigned long now = millis();
  uint8_t brightness = 0;

  switch (led.state) {
    case LED_OFF:
      {
        brightness = 0;
        break;
      }
    case LED_ON:
      {
        brightness = DEFAULT_BRIGHTNESS;
        break;
      }
    case LED_BLINKING:
      {
        if (now - led.lastUpdate > led.blinkSpeed) {
          brightness = (led.brightness == 0) ? 255 : 0;
          led.lastUpdate = now;
        } else {
          brightness = led.brightness;
        }
        break;
      }
    case LED_PULSING:
      {
        if (now - led.lastUpdate > led.pulseSpeed) {
          led.brightness += led.fadeAmount;
          // reverse the direction of the fading at the ends of the fade
          if (led.brightness <= 0 || led.brightness >= 255) {
            led.fadeAmount = -led.fadeAmount;
          }
          led.lastUpdate = now;
        }
        brightness = led.brightness;
        break;
      }
    case LED_FADE_OUT_ONCE:
      {
        if (now - led.lastUpdate > led.pulseSpeed) {
          if (led.brightness >= led.fadeAmount) {
            // calculate the elapsed time since the fade out started
            unsigned long elapsedTime = now - led.fadeOutStartTime;
            // exponential fading (255 -> 0, fast at beginning and slow at end)
            led.brightness = mapExponentially(elapsedTime, 0, led.fadeOutDuration, 255, 0);
          }
          led.lastUpdate = now;
        }
        brightness = led.brightness;
        break;
      }
    case LED_FADE_OUT_LOOP:  // clock synced
      {
        // calculate the elapsed time since the fade out started
        unsigned long elapsedTime = now - led.fadeOutStartTime;
        if (elapsedTime > led.fadeOutDuration) {
          led.fadeOutStartTime = now;
          elapsedTime = 0;
        }
        // map the elapsed time to a brightness value using a exponential mapping
        int newBrightness = mapExponentially(elapsedTime, 0, led.fadeOutDuration, 255, 0);
        if (newBrightness != led.brightness) {
          if (now - led.lastUpdate > led.blinkSpeed) {
            led.brightness = newBrightness;
            led.lastUpdate = now;
          }
        }
        brightness = led.brightness;
        break;
      }
    default:
      {
      }
  }
  led.brightness = brightness;
  return brightness;
}
