// TurtlPass Firmware Arduino
//
// Input (115200 baud | New Line):
//  - prefix hash with '/'
//  - hash lenght between 1-128 chars
//  - hash only with hexadecimal digits
//
// Examples:
//  - Get Board Information
//    i
//
//  - Generate Password (hex input)
//    /4dff4ea340f0a823f15d3f4f01ab62eae0e5da579ccb851f8db9dfe84c58b2b37b89903a740e1ee172da793a6e79d560e5f7f9bd058a12a280433ed6fa46510a
//
//    Keyboard Output:
//      100 characters password
//      characters type: [a-z][A-Z][0-9]
//
//  - Encrypt bytes ('>', '<' to Decrypt)
//    >4dff4ea340f0a823f15d3f4f01ab62eae0e5da579ccb851f8db9dfe84c58b2b37b89903a740e1ee172da793a6e79d560e5f7f9bd058a12a280433ed6fa46510a
//    <byte> <byte> ...
//
//    Serial Output:
//      <encrypted-byte> <encrypted-byte> ...
//
//  - Add new OTP shared secret to the virtual EEPROM, given a user hash and the secret
//    +4dff4ea340f0a823f15d3f4f01ab62eae0e5da579ccb851f8db9dfe84c58b2b37b89903a740e1ee172da793a6e79d560e5f7f9bd058a12a280433ed6fa46510a:ABCDEF0123456789ABCDEF0123456789
//
//  - Get Otp Code With Secret From EEPROM, given a user hash and the current timestamp
//    @4dff4ea340f0a823f15d3f4f01ab62eae0e5da579ccb851f8db9dfe84c58b2b37b89903a740e1ee172da793a6e79d560e5f7f9bd058a12a280433ed6fa46510a:1676821524
//
//  - Print all encrypted OTP secrets from EEPROM
//    ?
//
#include <Arduino.h>
#include <Keyboard.h>
#include "Seed.h"
#include "Kdf.h"
#include "OtpManager.h"
#include "LedManager.h"
#include "EncryptionManager.h"
#include "Info.h"

#if defined(__TURTLPASS_PIN_TTP223__)
#include "TTP223.h"
#else
#include "BootselButton.h"
#endif

enum InternalState {
  IDLE = 0,
  TOUCHING = 1,
  EXECUTING_CMD = 2,
  TYPING = 3,
  PASSWORD_READY = 4,
  OTP_READY = 5,
  ENCRYPTING = 6,
  DECRYPTING = 7,
};
InternalState internalState = IDLE;
LedManager ledManager;
Kdf kdf;
OtpManager otpManager;
EncryptionManager encryption;

void onSingleTouch();         // TTP223 callback
void onLongTouchStart();      // TTP223 callback
void onLongTouchEnd();        // TTP223 callback
void onLongTouchCancelled();  // TTP223 callback

#if defined(__TURTLPASS_PIN_TTP223__)
TTP223 ttp223(__TURTLPASS_PIN_TTP223__, onSingleTouch, onLongTouchStart, onLongTouchEnd, onLongTouchCancelled);  // initialize TTP223 instance with the callback functions
#else
// if no touch sensor pin defined, fallback to bootsel button
BootselButton bootselButton(onSingleTouch, onLongTouchStart, onLongTouchEnd, onLongTouchCancelled);  // initialize BootselButton instance with the callback functions
#endif

const uint8_t INPUT_BUFFER_SIZE = 255;
const uint8_t MIN_INPUT_SIZE = 1;
const uint8_t MAX_INPUT_SIZE = 128;
char input[INPUT_BUFFER_SIZE];
uint8_t output[PASS_SIZE + 1];
const uint16_t ENCRYPTION_BUFFER_SIZE = 8192;
uint8_t encryptionByteBuffer[ENCRYPTION_BUFFER_SIZE];
const unsigned long ENCRYPTION_TIMEOUT = 300;
unsigned long encryptionLastDataTime = 0;
unsigned long encryptionBytesRead = 0;

void processSerial(void);
void processInput();
bool validateInput();
void typePassword();
void typeOtpCode();
void readSerial();
void waitingCommand();
void handleCommand();
void handleEncryption(InternalState state);
void onEncryptionEnd();
const char* getSelectedSeed();


/////////////////////////
// TTP223 TOUCH SENSOR //
/////////////////////////

void onSingleTouch() {
  switch (internalState) {
    case IDLE:
      {
        ledManager.showNextColor();
        break;
      }
    case PASSWORD_READY:
      {
        if (output[0] != 0) {
          typePassword();
        } else {
          Serial.println("<PASSWORD-ERROR>");
          ledManager.setOn();
          internalState = IDLE;
        }
        break;
      }
    case OTP_READY:
      {
        if (otpManager.getCurrentOtpCode() > 0) {
          typeOtpCode();
        } else {
          Serial.println("<OTP-ERROR>");
          ledManager.setOn();
          internalState = IDLE;
        }
        break;
      }
    default:
      {
      }
  }
}

void onLongTouchStart() {
  if (internalState == IDLE) {
    internalState = TOUCHING;
    ledManager.setFadeOutOnce(2);
  }
}

void onLongTouchEnd() {
  if (internalState == TOUCHING) {
    bool result = kdf.derivatePass(output, PASS_SIZE, const_cast<char*>("default"), getSelectedSeed());
    if (result && output[0] != 0) {
      typePassword();
    } else {
      memset(output, 0, sizeof(output));
      internalState = IDLE;
    }
  }
}

void onLongTouchCancelled() {
  if (internalState == TOUCHING) {
    internalState = IDLE;
    ledManager.setOn();
  }
}


///////////
// Input //
///////////

void readSerial() {
  switch (internalState) {
    case IDLE:
      {
        waitingCommand();
        break;
      }
    case EXECUTING_CMD:
      {
        handleCommand();
        break;
      }
    case ENCRYPTING:
      {
        handleEncryption(ENCRYPTING);
        break;
      }
    case DECRYPTING:
      {
        handleEncryption(DECRYPTING);
        break;
      }
    default:
      {
      }
  }
}

void waitingCommand() {
  static uint16_t index = 0;
  while (Serial.available() > 0) {
    char readChar = Serial.read();
    if (readChar != '\n') {
      if (index < INPUT_BUFFER_SIZE) input[index++] = readChar;
    } else {
      input[index] = '\0';  // terminate the char array
      index = 0;
      internalState = EXECUTING_CMD;
    }
  }
}

void handleCommand() {
  switch (input[0]) {
    case 'i':
      {
        deviceInfo();
        internalState = IDLE;
        break;
      }
    case '/':
      {
        size_t inputLength = strlen(input);
        if (inputLength <= MIN_INPUT_SIZE || inputLength > MAX_INPUT_SIZE + 1) {
          Serial.println("<PASSWORD-INVALID-LENGTH>");
          internalState = IDLE;
          break;
        }
        for (size_t i = 1; i < inputLength; i++) {
          if (!isHexadecimalDigit(input[i])) {
            Serial.println("<PASSWORD-INVALID-INPUT>");
            internalState = IDLE;
            break;
          }
        }
        bool result = kdf.derivatePass(output, PASS_SIZE, input + 1, getSelectedSeed());
        if (result) {
          Serial.println("<PASSWORD-READY>");
          ledManager.setPulsing();
          internalState = PASSWORD_READY;
        } else {
          Serial.println("<PASSWORD-ERROR>");
          memset(output, 0, sizeof(output));
          internalState = IDLE;
        }
        break;
      }
    case '\\':
      {
        size_t inputLength = strlen(input);
        if (inputLength <= MIN_INPUT_SIZE || inputLength > MAX_INPUT_SIZE + 1) {
          Serial.println("<PASSWORD-INVALID-LENGTH>");
          internalState = IDLE;
          break;
        }
        for (size_t i = 1; i < inputLength; i++) {
          if (!isHexadecimalDigit(input[i])) {
            Serial.println("<PASSWORD-INVALID-INPUT>");
            internalState = IDLE;
            break;
          }
        }
        bool result = kdf.derivatePassWithSymbols(output, PASS_SIZE, input + 1, getSelectedSeed());
        if (result) {
          Serial.println("<PASSWORD-READY>");
          ledManager.setPulsing();
          internalState = PASSWORD_READY;
        } else {
          Serial.println("<PASSWORD-ERROR>");
          memset(output, 0, sizeof(output));
          internalState = IDLE;
        }
        break;
      }
    case '+':
      {
        bool result = otpManager.addOtpSecretToEEPROM(input + 1, getSelectedSeed());
        if (result) {
          Serial.println("<OTP-ADDED>");
        } else {
          Serial.println("<OTP-ERROR>");
        }
        internalState = IDLE;
        break;
      }
    case '@':
      {
        bool result = otpManager.getOtpCodeWithSecretFromEEPROM(input + 1, getSelectedSeed());
        if (result) {
          ledManager.setFadeOutLoop(30, time(NULL));
          Serial.println("<OTP-READY>");
          internalState = OTP_READY;
        } else {
          Serial.println("<OTP-ERROR>");
          internalState = IDLE;
        }
        break;
      }
    case '?':
      {
        otpManager.readAllSavedData();
        internalState = IDLE;
        break;
      }
    case '*':
      {
        otpManager.factoryReset();
        Serial.println("<OTP-RESET>");
        internalState = IDLE;
        break;
      }
    case '>':
      {
        bool result = encryption.init(input + 1, getSelectedSeed());
        if (result) {
          ledManager.setBlinking();
          Serial.println("<ENCRYPTING>");
          internalState = ENCRYPTING;
        } else {
          encryption.clear();
          Serial.println("<ENCRYPTING-ERROR>");
          internalState = IDLE;
        }
        break;
      }
    case '<':
      {
        bool result = encryption.init(input + 1, getSelectedSeed());
        if (result) {
          ledManager.setBlinking();
          Serial.println("<DECRYPTING>");
          internalState = DECRYPTING;
        } else {
          encryption.clear();
          Serial.println("<DECRYPTING-ERROR>");
          internalState = IDLE;
        }
        break;
      }
    default:
      {
        ledManager.setOn();
        Serial.println("<UNKNOWN>");
        internalState = IDLE;
        break;
      }
  }
  // clear input and reset execution state
  memset(input, '\0', INPUT_BUFFER_SIZE);
}

const char* getSelectedSeed() {
  return seedArray[ledManager.getColorIndex()];
}


////////////////
// Encryption //
////////////////

void handleEncryption(InternalState state) {
  // clear the buffer before reading new data
  memset(encryptionByteBuffer, 0, ENCRYPTION_BUFFER_SIZE);
  // reset the last data time
  encryptionLastDataTime = millis();

  while (Serial.available() > 0 || millis() - encryptionLastDataTime < ENCRYPTION_TIMEOUT) {
    // check if there is data available to read or if the timeout has not expired
    if (Serial.available() > 0) {
      size_t bytesToRead = min(Serial.available(), ENCRYPTION_BUFFER_SIZE);  // update bytesToRead
      size_t bytesReadNow = Serial.readBytes(encryptionByteBuffer, bytesToRead);
      encryptionBytesRead += bytesReadNow;  // increment the total number of bytes read

      if (bytesReadNow > 0) {
        // process the data (encrypt or decrypt) if bytes are read
        switch (state) {
          case ENCRYPTING:
            // encrypt
            if (!encryption.encrypt2serial(encryptionByteBuffer, bytesReadNow)) {
              onEncryptionEnd();
              return;
            }
            break;
          case DECRYPTING:
            // decrypt
            if (!encryption.decrypt2serial(encryptionByteBuffer, bytesReadNow)) {
              onEncryptionEnd();
              return;
            }
            break;
          default:
            break;
        }
      }
      // update the last data time
      encryptionLastDataTime = millis();
    } else {
      // no data available, check for timeout
      if (millis() - encryptionLastDataTime >= ENCRYPTION_TIMEOUT) {
        // timeout expired, end transmission
        onEncryptionEnd();
        return;
      }
    }
  }
  if (encryptionBytesRead == 0) {
    // no bytes available
    onEncryptionEnd();
  }
}

void onEncryptionEnd() {
  memset(encryptionByteBuffer, 0, ENCRYPTION_BUFFER_SIZE);
  encryptionBytesRead = 0;
  encryption.clear();
  ledManager.setOn();
  internalState = IDLE;
}


//////////////////////
// USB HID Keyboard //
//////////////////////

void typePassword() {
  if (memchr(output, 0, sizeof(output)) != NULL) {
    internalState = TYPING;
    ledManager.setBlinking();
    Keyboard.write(output, sizeof(output));
    memset(output, 0, sizeof(output));
    delay(100);  // delay blinking state
    ledManager.setOn();
  }
  internalState = IDLE;
}

void typeOtpCode() {
  uint32_t otpCode = otpManager.getCurrentOtpCode();
  if (otpCode > 0) {
    internalState = TYPING;
    ledManager.setBlinking();
    char dst[7];  // buffer to hold the 6 digit otp code (including null terminator)
    snprintf(dst, sizeof(dst), "%06u", otpCode);
    Keyboard.print(dst);
    otpManager.resetCurrentOtp();
    delay(100);  // delay blinking state
    ledManager.setOn();
  }
  internalState = IDLE;
}


//////////////////////////
// Thread 0: First core //
//////////////////////////

void setup() {
  Serial.begin(115200);
  Keyboard.begin();
  otpManager.begin(SIZE_EEPROM);

#if defined(__TURTLPASS_PIN_TTP223__)
  ttp223.begin();
#endif
}

void loop() {
  otpManager.loop();

#if defined(__TURTLPASS_PIN_TTP223__)
  ttp223.loop(FastLED.getBrightness());
#else
  bootselButton.loop(FastLED.getBrightness());
#endif
  
  readSerial();
}


///////////////////////////
// Thread 1: Second core //
///////////////////////////

void setup1() {
  delay(1000);  // power-up safety delay
  ledManager.setup();
  randomSeed(analogRead(A0));
  ledManager.setColorIndex(random(NUM_COLORS));
}

void loop1() {
  ledManager.loop();
}
