#ifndef OTP_MANAGER_H
#define OTP_MANAGER_H

#include <stdlib.h>
#include <stdint.h>
#include <cstring>
#include <sys/time.h>
#include "EEPROMMate.h"
#include "OtpCode.h"
#include "Seed.h"
#include "Kdf.h"

class OtpManager {
  public:
    EEPROMMate eepromMate;
    OtpCode otp;
    Kdf kdf;
    void begin(size_t eepromSize);
    void loop();
    uint32_t getCurrentOtpCode();
    void resetCurrentOtp();
    bool addOtpSecretToEEPROM(char *input, const char* seed);
    bool getOtpCodeWithSecretFromEEPROM(char *input, const char* seed); //, LedController ledController
    bool readAllSavedData();

  private:
    long parseTimestampAndUpdateDateTime(char *input);
    void updateSystemTime(long timestamp);
    bool readOtpSecretAndGenerateCode(char *input, uint32_t timestamp, const char* seed);
    void convertUint8ArrayToCharArray(uint8_t *uint8Array, char *charArray, int length);
    void splitCharArray(const char *src, char *dst1, char *dst2);
    unsigned long previousMillis = 0;  // variable to store the last time `loop()` was executed
    bool isDateTimeSet = false;
    unsigned long lastUpdate;
    int updateInterval = 30000; // in ms
    const uint8_t SHA_512_LENGTH = 128; // SHA-512 hash value is 512 bits long = 128 hexadecimal digits
    const uint8_t MIN_OTP_LENGTH = 16;
    const uint16_t MAX_OTP_LENGTH = 256;
    const uint8_t TIMESTAMP_LENGTH = 10;
    uint32_t durationOtp = 30; // length of one time duration (e.g. 30 seconds)
    char currentSharedSecret[32 + 1] = {0};
    uint32_t currentOtp = 0;
    unsigned long lastUpdateMillis = 0;
};

#endif // OTP_MANAGER_H
