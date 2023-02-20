#include "Kdf.h"

bool Kdf::derivatePass(uint8_t *dst, size_t dstLength, char *input, const char *seed) {
  size_t keyLength = base62InputLength(dstLength);
  uint8_t *key = (uint8_t *)malloc(keyLength + 1);
  if (key == NULL) {
    return false;  // memory allocation failed
  }
  if (!derivateKey(key, keyLength, input, seed)) {
    free(key);
    return false;
  }
  key2Charset(dst, key, keyLength);
  free(key);
  return true;
}

bool Kdf::derivateKey(uint8_t *dst, size_t dstLength, char *input, const char *seed) {
  size_t srcLength = strlen(input);
  uint8_t *src = (uint8_t *)malloc(srcLength);
  if (src == NULL) {
    return false;  // memory allocation failed
  }
  memcpy(src, input, srcLength);
  src[srcLength] = '\0';  // ensure null-terminated
  // salt
  const size_t seedLength = strlen(seed);
  const size_t saltLength = seedLength / 2;
  uint8_t *salt = (uint8_t *)malloc(saltLength);
  if (salt == NULL) {
    free(src);
    return false;  // memory allocation failed
  }
  for (size_t i = 0; i < saltLength; i++) {
    char buf[3] = { seed[i * 2], seed[i * 2 + 1], '\0' };
    salt[i] = (uint8_t)strtoul(buf, NULL, 16);
  }
  // hkdf
  hkdf(dst, dstLength, src, srcLength, salt, saltLength);
  free(src);
  free(salt);
  return true;
}

// HMAC-based Extract-and-Expand Key Derivation Function (HKDF)
void Kdf::hkdf(uint8_t *dst, size_t dstLength, const uint8_t *src, size_t srcLength, const uint8_t *salt, size_t saltLength) {
  // create an instance of the HKDF class using SHA-512 as the underlying hash algorithm
  HKDF<SHA512> hkdf;
  // set the key and salt values for the HKDF session
  hkdf.setKey(src, srcLength, salt, saltLength);
  const char *info = "turtlpass";
  const size_t infoLen = strlen(info);
  // extract the derived password from the HKDF session
  hkdf.extract(dst, dstLength, info, infoLen);
  // clear the HKDF instance to remove any sensitive information from memory
  hkdf.clear();
}

// based on base62 encoding with 'z' char as padding instead of '='
void Kdf::key2Charset(uint8_t *dst, const uint8_t *src, size_t srcLength) {
  static const char *charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  // use bit manipulation operations to encode 3 bytes as 4 base62 characters
  for (size_t i = 0; i < srcLength; i += 3) {
    uint32_t bits = (src[i] << 16) | (src[i + 1] << 8) | src[i + 2];
    for (size_t j = 0; j < 4; j++) {
      if (i + j < srcLength) {
        dst[i / 3 * 4 + j] = charset[(bits >> (6 * (3 - j))) & 0x3F];
      } else {
        // pad the output if the input is not a multiple of 3 bytes
        dst[i / 3 * 4 + j] = 'z';
      }
    }
  }
}

size_t Kdf::base62InputLength(size_t encodedLength) {
  return (encodedLength * 6) / 8;  // round down to the nearest whole number
}
