#include "Kdf.h"

const uint8_t KDF_SIZE = 75; // 3/4 of 100 characters

/////////
// KDF //
/////////

bool processKeyDerivation(uint8_t *output, size_t outputLength, char *input) {
  size_t inputLength = strlen(input);
  uint8_t *src = (uint8_t *)malloc(inputLength);
  memcpy(src, input, inputLength);
  // salt
  const size_t seedLength = strlen(seed);
  const size_t saltLength = seedLength / 2;
  uint8_t salt[saltLength];
  for (size_t i = 0; i < saltLength; i++) {
    char buf[3] = {seed[i * 2], seed[i * 2 + 1], '\0'};
    salt[i] = (uint8_t)strtoul(buf, NULL, 16);
  }
  uint8_t result[KDF_SIZE];
  hkdf(src, inputLength, salt, saltLength, result, KDF_SIZE);
  hex2Password(result, sizeof(result), output);
  return true;
}

/**
   HMAC-based Extract-and-Expand Key Derivation Function (HKDF)
*/
void hkdf(const uint8_t *password, size_t passwordLength, const uint8_t *salt, size_t saltLength, uint8_t *key, size_t keyLength) {
  // Create an instance of the HKDF class using SHA-512 as the underlying hash algorithm
  HKDF<SHA512> hkdf;

  // Set the key and salt values for the HKDF session
  hkdf.setKey(password, passwordLength, salt, saltLength);

  const char *info = "turtlpass-firmware";
  const size_t infoLen = strlen(info);

  // Extract the derived password from the HKDF session
  hkdf.extract(key, keyLength, info, infoLen);

  // Clear the HKDF instance to remove any sensitive information from memory
  hkdf.clear();
}

void hex2Password(const uint8_t *src, size_t srcLength, uint8_t *dst) {
  static const char *charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789az";
  // Use bit manipulation operations to encode 3 bytes as 4 base64 characters
  for (size_t i = 0; i < srcLength; i += 3) {
    uint32_t bits = (src[i] << 16) | (src[i + 1] << 8) | src[i + 2];
    for (size_t j = 0; j < 4; j++) {
      dst[i / 3 * 4 + j] = charset[(bits >> (6 * (3 - j))) & 0x3F];
    }
  }
  // Pad the output with characters if necessary
  if (srcLength % 3 == 1) {
    dst[srcLength / 3 * 4 + 2] = 'a';
    dst[srcLength / 3 * 4 + 3] = 'a';
  } else if (srcLength % 3 == 2) {
    dst[srcLength / 3 * 4 + 3] = 'z';
  }
}
