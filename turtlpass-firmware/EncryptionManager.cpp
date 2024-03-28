#include "EncryptionManager.h"

EncryptionManager::EncryptionManager() {}

bool EncryptionManager::init(char* hash, const char* seed) {
  // derivate encryption key and initialization vector with kdf
  uint8_t encryptionKey[ENCRYPTION_KEY_SIZE];
  uint8_t encryptionInitializationVector[ENCRYPTION_IV_SIZE];
  kdf.derivateKey(encryptionKey, ENCRYPTION_KEY_SIZE, hash, seed);
  kdf.derivateKey(encryptionInitializationVector, ENCRYPTION_IV_SIZE, (char*)encryptionKey, seed);
  // init ChaCha20-Poly1305
  chachapoly.clear();
  chachapoly.setKey(encryptionKey, ENCRYPTION_KEY_SIZE);
  chachapoly.setIV(encryptionInitializationVector, ENCRYPTION_IV_SIZE);
  return true;
}

bool EncryptionManager::encrypt(uint8_t* dst, uint8_t* src, size_t length) {
  chachapoly.encrypt(dst, src, length);
  return true;
}

bool EncryptionManager::decrypt(uint8_t* dst, uint8_t* src, size_t length) {
  chachapoly.decrypt(dst, src, length);
  return true;
}

bool EncryptionManager::encrypt2serial(uint8_t* src, size_t length) {
  uint8_t* dst = (uint8_t*)malloc(length);
  if (dst == NULL) {
    return false;  // memory allocation failed
  }
  // perform encryption
  encrypt(dst, src, length);
  // send the encrypted data back over the serial port
  if (Serial.write(dst, length) != length) {
    // failed to write all data to serial port
    free(dst);
    return false;
  }
  // free memory allocated for encrypted data
  free(dst);
  return true;
}

bool EncryptionManager::decrypt2serial(uint8_t* src, size_t length) {
  uint8_t* dst = (uint8_t*)malloc(length);
  if (dst == NULL) {
    return false;  // memory allocation failed
  }
  // perform decryption
  decrypt(dst, src, length);
  // Send the decrypted data back over the serial port
  if (Serial.write(dst, length) != length) {
    // Failed to write all data to serial port
    free(dst);
    return false;
  }
  // free memory allocated for decrypted data
  free(dst);
  return true;
}

void EncryptionManager::clear() {
  chachapoly.clear();
}
