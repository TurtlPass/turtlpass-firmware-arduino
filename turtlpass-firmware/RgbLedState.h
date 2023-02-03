#ifndef RGB_LED_STATE_H
#define RGB_LED_STATE_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

enum State {
  LED_OFF = 0,
  LED_ON = 1,
  LED_PULSING = 2,
  LED_BLINKING = 3
};

#define NUM_COLORS 6
const uint8_t colors[NUM_COLORS][3] = {
  {0, 255, 0},    // green
  {255, 255, 0},  // yellow
  {255, 0, 0},    // red
  {0, 0, 255},    // blue
  {255, 255, 255},// white
  {255, 0, 255},  // magenta
};

struct Led {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  int brightness;
  int fadeAmount;
  int blinkState;
  int blinkSpeed; // in ms
  int pulseSpeed; // in ms
  unsigned long lastUpdate;
  State state;
};

class RgbLedState {

  public:
    RgbLedState();
    void init();
    void setOn();
    void setOff();
    void setPulsing();
    void setBlinking();
    void setColor(int colorIndex);
    void loop();

  private:
    static Led led;
};

#endif // RGB_LED_STATE_H
