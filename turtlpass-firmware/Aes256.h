/**
   AES-256 (CBC) Encryption using MbedTLS library
   CBC mode is widely used and considered secure if the encryption key remains secret

   Online tool for output validation: https://cryptii.com/pipes/aes-encryption
*/
#ifndef AES_256_H
#define AES_256_H

#include <stdlib.h>
#include <stdint.h>
#include <cstring>
#include <arduino_config.h> // Using library 'Seeed_Arduino_mbedtls' v3.0.1
#include <mbedtls/aes.h>

#define AES_256 256
#define AES_LENGTH 64
#define IV_SIZE 16

class Aes256 {
  public:
    Aes256();
    bool encrypt(const uint8_t *aesKey256, const uint8_t *aesIv, int dataLength, unsigned char* src, unsigned char* dst);
    bool decrypt(const uint8_t *aesKey256, const uint8_t *aesIv, int dataLength, unsigned char* src, unsigned char* dst);
};

#endif // AES_256_H
