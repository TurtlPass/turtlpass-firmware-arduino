// TurtlPass Firmware Arduino
//
// Input accepted:
//  - prefix hash with '/'
//  - hash lenght between 1-128 chars
//  - hash only with hexadecimal digits
//
// Input example:
//   /0
//   /d0d80a28b90a2fb1bef0
//   /4dff4ea340f0a823f15d3f4f01ab62eae0e5da579ccb851f8db9dfe84c58b2b37b89903a740e1ee172da793a6e79d560e5f7f9bd058a12a280433ed6fa46510a
//
// Output:
//   100 characters password
//   characters type: [a-z][A-Z][0-9]
//
// Output message:
//   'OK' or else is an 'ERROR'
//

#include <Arduino.h>
#include <Keyboard.h>
#include "Seed.h"
#include "Kdf.h"

#define BOARD_WITH_RGB_LED true // <--- COMMENT THIS LINE IF YOUR BOARD DOES NOT HAVE A RGB LED

#ifdef BOARD_WITH_RGB_LED
#include "RgbLedState.h"
RgbLedState ledState;
#else
#include "LedState.h"
LedState ledState(LED_BUILTIN);
#endif

///////////////
// Variables //
///////////////

const uint8_t INPUT_BUFFER_SIZE = 255;
const uint8_t MIN_INPUT_SIZE = 1;
const uint8_t MAX_INPUT_SIZE = 128;
const uint8_t OUTPUT_SIZE = 100; // 100 characters
char input[INPUT_BUFFER_SIZE];
uint8_t output[OUTPUT_SIZE + 1];
bool isExecuting = false;
int colorIndex = 0; // green

/////////////
// Methods //
/////////////

void processSerial(void);
void processInput();
bool validateInput();
void typeOutputKey();


//////////////////////////
// Thread 0: First core //
//////////////////////////

void setup() {
  Serial.begin(115200);
  Keyboard.begin();
  ledState.init();
  delay(1000);
}

void loop() {
  if (BOOTSEL) {
    if (output[0] != 0) {
      typeOutputKey();
    } else {
#ifdef BOARD_WITH_RGB_LED
      colorIndex = (colorIndex + 1) % NUM_COLORS;
      ledState.setColor(colorIndex);
#endif
    }
    while (BOOTSEL);
  } else {
    processSerial();
    if (isExecuting) processInput();
  }
}

///////////////////////////
// Thread 1: Second core //
///////////////////////////

void loop1() {
  ledState.loop();
}

///////////
// Input //
///////////

void processSerial(void) {
  static byte index = 0;
  while (Serial.available() > 0 && isExecuting == false) {
    char readChar = Serial.read();
    if (readChar != '\n') {
      if (index < INPUT_BUFFER_SIZE) input[index++] = readChar;
    } else {
      input[index] = '\0'; // terminate the char array
      index = 0;
      isExecuting = true;
    }
  }
}

void processInput() {
  if (validateInput()) {
    Serial.println("OK");
    ledState.setPulsing();
  } else {
    Serial.println("ERROR");
  }
  // Clear input and reset execution state
  memset(input, '\0', INPUT_BUFFER_SIZE);
  isExecuting = false;
}

bool validateInput() {
  size_t inputLength = strlen(input);
  if (inputLength <= MIN_INPUT_SIZE || inputLength > MAX_INPUT_SIZE + 1 || input[0] != '/') {
    Serial.println("invalid input");
    return false;
  }

  // Calculate the length of the input string, minus one character
  // (to skip the first character of the input string)
  size_t length = inputLength - 1;
  char *substring = (char *)malloc(length + 1);
  strcpy(substring, input + 1);

  // Check if valid hexadecimal input
  for (size_t i = 0; i < length; i++) {
    if (!isHexadecimalDigit(substring[i])) {
      Serial.println("is not hexadecimal digit");
      return false;
    }
  }
  return processKeyDerivation(output, sizeof(output), substring, seedArray[colorIndex]);
}

////////////
// Output //
////////////

void typeOutputKey() {
  if (memchr(output, 0, sizeof(output)) != NULL) {
    ledState.setBlinking();

    Keyboard.write(output, sizeof(output));

    delay(150); // delay blinking state
    ledState.setOn();

    memset(output, 0, sizeof(output));
  }
}
