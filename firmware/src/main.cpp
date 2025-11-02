#include <Arduino.h>
#include "InternalState.h"
#include "storage/SeedManager.h"
#include "ui/LedManager.h"
#include "crypto/Kdf.h"
#include "crypto/EncryptionManager.h"
#include "keyboard/HidKeyboard.h"
#include "core/CommandProcessor.h"
#include "core/TouchHandler.h"
#include "core/SerialProcessor.h"

#if defined(__TURTLPASS_PIN_TTP223__)
#include "input/TTP223.h"
#else
#include "input/BootselButton.h"
#endif

InternalState internalState = IDLE;
LedManager ledManager;
Kdf kdf;
SeedManager seedManager;
EncryptionManager encryption;
uint8_t output[MAX_PASS_SIZE + 1];
CommandProcessor commandProcessor(seedManager, kdf, ledManager, internalState, output, sizeof(output));
TouchHandler touchHandler(internalState, ledManager, commandProcessor);
SerialProcessor serialProcessor(commandProcessor);

#if defined(__TURTLPASS_PIN_TTP223__)
TTP223 ttp223(__TURTLPASS_PIN_TTP223__,
              [](){ touchHandler.onSingleTouch(); },
              [](){ touchHandler.onLongTouchStart(); },
              [](){ touchHandler.onLongTouchEnd(); },
              [](){ touchHandler.onLongTouchCancelled(); });
#else
BootselButton bootselButton(
    [](){ touchHandler.onSingleTouch(); },
    [](){ touchHandler.onLongTouchStart(); },
    [](){ touchHandler.onLongTouchEnd(); },
    [](){ touchHandler.onLongTouchCancelled(); });
#endif

//////////////////////////
// Thread 0: First core //
//////////////////////////

void setup() {
  Serial.begin(115200);
  seedManager.begin();
  hidKeyboardInit();

#if defined(__TURTLPASS_PIN_TTP223__)
  ttp223.begin();
#endif
}

void loop() {
#if defined(__TURTLPASS_PIN_TTP223__)
  ttp223.loop(ledManager.getCurrentBrightness());
#else
  bootselButton.loop(ledManager.getCurrentBrightness());
#endif
  serialProcessor.loop();
}

///////////////////////////
// Thread 1: Second core //
///////////////////////////

void setup1() {
  delay(1000);  // power-up safety delay
  ledManager.begin();
}

void loop1() {
  ledManager.loop();
}
