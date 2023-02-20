#ifndef ENCRYPTION_MANAGER_H
#define ENCRYPTION_MANAGER_H

#include <SoftwareSerial.h>
#include <stdlib.h>
#include <stdint.h>
#include <cstring>
#include <Crypto.h>
#include <ChaChaPoly.h>
#include "Kdf.h"

#define ENCRYPTION_KEY_SIZE 32  // 32 bytes = 256-bit
#define ENCRYPTION_IV_SIZE 12   // 12 bytes = 96-bit (both 64-bit and 96-bit nonces are supported)

/*
  IETF ChaCha uses a 96-bit nonce. Original ChaCha uses a 64-bit nonce.
*/
class EncryptionManager {
public:
  EncryptionManager();
  bool init(char* hash, const char* seed);
  bool encrypt(uint8_t* src, size_t length);
  bool decrypt(uint8_t* src, size_t length);
  void clear();

private:
  ChaChaPoly chachapoly;
  Kdf kdf;
  const char* seed;
};

#endif  // ENCRYPTION_MANAGER_H
