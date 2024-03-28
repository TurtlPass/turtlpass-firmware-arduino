#include "OtpManager.h"

void OtpManager::begin(size_t eepromSize) {
  eepromMate.begin(eepromSize);
}

void OtpManager::loop() {
  if (!isDateTimeSet) return;
  unsigned long now = millis();
  if (now - this->lastUpdateMillis > 1000) {  // 1 sec
    uint32_t timestamp = time(NULL);
    if (timestamp % this->durationOtp == 0) {
      if (memchr(this->currentSharedSecret, 0, sizeof(this->currentSharedSecret)) == NULL) return;
      this->currentOtp = otp.generateOtpCode(this->currentSharedSecret, timestamp, this->durationOtp);
    }
    this->lastUpdateMillis = now;
  }
}

uint32_t OtpManager::getCurrentOtpCode() {
  return currentOtp;
}

void OtpManager::resetCurrentOtp() {
  currentOtp = 0;
  memset(currentSharedSecret, 0, sizeof(currentSharedSecret));
}

bool OtpManager::addOtpSecretToEEPROM(char *input, const char *seed) {
  int inputLen = strlen(input);
  if (inputLen < (SHA_512_LENGTH + MIN_OTP_LENGTH)) {
    Serial.println("inputLen too small");
    return false;  // inputLen too small
  }
  if (inputLen > (SHA_512_LENGTH + 1 + MAX_OTP_LENGTH)) {
    Serial.println("inputLen too long");
    return false;  // inputLen too long
  }
  int keyLength = SHA_512_LENGTH;
  int valueLength = inputLen - SHA_512_LENGTH - 1;
  if ((valueLength % 16) != 0) {
    Serial.println("invalid value length");
    return false;  // invalid value length
  }
  char keyInput[keyLength + 1];
  char valueInput[valueLength + 1];
  splitCharArray(input, keyInput, valueInput);

  if (!encryption.init(keyInput, seed)) {
    return false;
  }
  uint8_t *key = reinterpret_cast<uint8_t *>(keyInput);
  uint8_t *value = reinterpret_cast<uint8_t *>(valueInput);
  bool result = writeDataEncryptedToEEPROM(key, keyLength, value, valueLength);
  encryption.clear();
  return result;
}

bool OtpManager::writeDataEncryptedToEEPROM(uint8_t *key, uint8_t keyLength, uint8_t *value, uint16_t valueLength) {
  uint8_t *encryptedValue = (uint8_t *)malloc(valueLength);
  if (encryptedValue == NULL) {
    return false;  // memory allocation failed
  }
  // perform encryption
  encryption.encrypt(encryptedValue, value, valueLength);
  // write encrypted
  if (!eepromMate.writeData(key, keyLength, encryptedValue, valueLength)) {
    // failed to write
    free(encryptedValue);
    return false;
  }
  // free memory allocated for encrypted data
  free(encryptedValue);
  return true;
}

bool OtpManager::getOtpCodeWithSecretFromEEPROM(char *input, const char *seed) {
  int inputLen = strlen(input);
  if (inputLen != (SHA_512_LENGTH + 1 + TIMESTAMP_LENGTH)) {
    return false;  // inputLen invalid
  }
  char hashInput[SHA_512_LENGTH + 1];
  char timestampInput[TIMESTAMP_LENGTH + 1];
  splitCharArray(input, hashInput, timestampInput);

  long timestamp = parseTimestampAndUpdateDateTime(timestampInput);
  if (timestamp <= 0) {
    return false;  // parseTimestamp Error
  }
  return readOtpSecretAndGenerateCode(hashInput, timestamp, seed);
}

long OtpManager::parseTimestampAndUpdateDateTime(char *input) {
  int inputLength = strlen(input);
  if (inputLength != TIMESTAMP_LENGTH) return false;
  for (size_t i = 0; i < inputLength - 1; i++) {
    if (!isDigit(input[i])) {
      Serial.println("input contains non-valid digit");
      return -1;
    }
  }
  long timestamp = atol(input);
  updateSystemTime(timestamp);
  isDateTimeSet = true;
  return timestamp;
}

bool OtpManager::readOtpSecretAndGenerateCode(char *input, uint32_t timestamp, const char *seed) {
  int keyLength = strlen(input);
  uint8_t *key = reinterpret_cast<uint8_t *>(input);
  uint16_t valueLength = eepromMate.readValueLength(key, keyLength);
  if (valueLength == -1) return false;
  uint8_t dstValue[valueLength];

  if (!encryption.init(input, seed)) {
    return false;
  }
  bool result = readValueDecryptedFromEEPROM(key, keyLength, dstValue, valueLength);
  if (result) {
    convertUint8ArrayToCharArray(dstValue, currentSharedSecret, valueLength);
    currentOtp = otp.generateOtpCode(currentSharedSecret, timestamp, durationOtp);
  }
  encryption.clear();
  return result;
}

bool OtpManager::readValueDecryptedFromEEPROM(uint8_t *key, uint8_t keyLength, uint8_t *dstValue, uint16_t dstValueLength) {
  uint8_t *encryptedValue = (uint8_t *)malloc(dstValueLength);
  if (encryptedValue == NULL) {
    return false;  // memory allocation failed
  }
  // read the encrypted value
  if (!eepromMate.readValue(key, keyLength, encryptedValue, dstValueLength)) {
    free(encryptedValue);
    return false;  // failed to read value
  }
  uint8_t *decryptedValue = (uint8_t *)malloc(dstValueLength);
  if (decryptedValue == NULL) {
    return false;  // memory allocation failed
  }
  // perform decryption
  encryption.decrypt(decryptedValue, encryptedValue, dstValueLength);
  // copy the decrypted value to the destination
  memcpy(dstValue, decryptedValue, dstValueLength);
  // free memory allocated
  free(encryptedValue);
  free(decryptedValue);
  return true;
}

void OtpManager::updateSystemTime(long timestamp) {
  struct timeval tv;
  tv.tv_sec = timestamp;
  tv.tv_usec = 0;
  settimeofday(&tv, nullptr);
}

bool OtpManager::readAllSavedData() {
  return eepromMate.readAllSavedData();
}

void OtpManager::factoryReset() {
  eepromMate.factoryReset();
}

void OtpManager::splitCharArray(const char *src, char *dst1, char *dst2) {
  int srcLen = strlen(src);
  int colonIndex = -1;
  for (int i = 0; i < srcLen; i++) {
    if (src[i] == ':') {
      colonIndex = i;
      break;
    }
  }
  if (colonIndex == -1) {
    strcpy(dst1, src);
    dst2[0] = '\0';
    return;
  }
  strncpy(dst1, src, colonIndex);
  dst1[colonIndex] = '\0';
  strcpy(dst2, src + colonIndex + 1);
}

void OtpManager::convertUint8ArrayToCharArray(uint8_t *uint8Array, char *charArray, int length) {
  for (int i = 0; i < length; i++) {
    charArray[i] = static_cast<char>(uint8Array[i]);
  }
  charArray[length] = '\0';
}
