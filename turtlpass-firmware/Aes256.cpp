/**
   AES-256 (CBC) Encryption using MbedTLS library
   CBC mode is widely used and considered secure if the encryption key remains secret

   Online tool for output validation: https://cryptii.com/pipes/aes-encryption
*/
#include "Aes256.h"

Aes256::Aes256() {}

bool Aes256::encrypt(const uint8_t *aesKey256, const uint8_t *aesIv, int dataLength, unsigned char* src, unsigned char* dst) {
  if ((dataLength % 16) != 0) return false;
  mbedtls_aes_context aes;
  mbedtls_aes_init(&aes);
  int result = mbedtls_aes_setkey_enc(&aes, aesKey256, AES_256);
  if (result == 0) {
    uint8_t tmpIv[IV_SIZE];
    memcpy(tmpIv, aesIv, IV_SIZE);
    result = mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_ENCRYPT, dataLength, tmpIv, src, dst);
  }
  mbedtls_aes_free(&aes);
  return result == 0;
}

bool Aes256::decrypt(const uint8_t *aesKey256, const uint8_t *aesIv, int dataLength, unsigned char* src, unsigned char* dst) {
  if ((dataLength % 16) != 0) return false;
  mbedtls_aes_context aes;
  mbedtls_aes_init(&aes);
  int result = mbedtls_aes_setkey_dec(&aes, aesKey256, AES_256);
  if (result == 0) {
    uint8_t tmpIv[IV_SIZE];
    memcpy(tmpIv, aesIv, IV_SIZE);
    result = mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT, dataLength, tmpIv, src, dst);
  }
  mbedtls_aes_free(&aes);
  return result == 0;
}
