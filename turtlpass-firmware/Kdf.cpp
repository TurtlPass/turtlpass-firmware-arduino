#include "Kdf.h"

// base94
bool Kdf::derivatePassWithSymbols(uint8_t *dst, size_t dstLength, char *input, const char *seed) {
  // validate input pointers
  if (!dst || !input || !seed) {
    return false;
  }
  // calculate the length of the input string
  size_t inputLength = strlen(input);
  // allocate memory for the intermediate key
  size_t keyLength = base94InputLength(dstLength);
  std::vector<uint8_t> key(keyLength);
  if (key.empty()) {
    return false;  // Memory allocation failed
  }
  // derive the key using the provided input and seed
  if (!derivateKey(key.data(), keyLength, input, seed)) {
    return false;
  }
  // encode the derived key using base94 encoding
  Base94 base94;
  std::string encodedKey;
  if (!base94.encode(key, encodedKey)) {
    return false;
  }
  // copy the encoded key to the destination buffer
  size_t copyLength = std::min(encodedKey.size(), dstLength);
  std::memcpy(dst, encodedKey.data(), copyLength);
  dst[copyLength] = '\0';  // ensure null-termination
  return true;
}

uint32_t Kdf::base94InputLength(size_t encodedLength) {
    // calculate the number of output blocks
    uint32_t outputBlocks = (encodedLength + BASE94_OUTPUT_BLOCK_SIZE - 1) / BASE94_OUTPUT_BLOCK_SIZE;
    // calculate the number of input blocks
    uint32_t inputBlocks = outputBlocks * BASE94_INPUT_BLOCK_SIZE;
    // return the total input length
    return inputBlocks;
}

// base62
bool Kdf::derivatePass(uint8_t *dst, size_t dstLength, char *input, const char *seed) {
  // validate input pointers
  if (!dst || !input || !seed) {
    return false;
  }
  // calculate the length of the input string
  size_t inputLength = strlen(input);
  // allocate memory for the intermediate key
  size_t keyLength = base62InputLength(dstLength);
  uint8_t *key = (uint8_t *)malloc(keyLength);
  if (key == NULL) {
    return false;  // memory allocation failed
  }
  // derive the key using the provided input and seed
  if (!derivateKey(key, keyLength, input, seed)) {
    free(key);
    return false;
  }
  // encode the derived key using base62 encoding
  size_t encodedKeyLength = (keyLength * 2) + 1;  // assumes worst-case scenario for encoding
  char *encodedKey = base62_encode((char *)malloc(encodedKeyLength), encodedKeyLength, key, keyLength);
  free(key);
  // check if base62 encoding was successful
  if (encodedKey == NULL) {
    return false;
  }
  // copy the encoded key to the destination buffer
  strncpy((char *)dst, encodedKey, dstLength);
  dst[dstLength] = '\0';  // ensure null-termination
  // free allocated memory
  free(encodedKey);
  return true;
}

uint32_t Kdf::base62InputLength(size_t encodedLength) {
  // round down to the nearest whole number
  return (encodedLength * BASE62_ENCODED_BITS) / BITS_PER_BYTE;
}

bool Kdf::derivateKey(uint8_t *dst, size_t dstLength, char *input, const char *seed) {
  // validate input pointers
  if (!dst || !input || !seed) {
    return false;
  }
  size_t srcLength = strlen(input);
  uint8_t *src = (uint8_t *)malloc(srcLength + 1);
  if (src == nullptr) {
    return false;  // memory allocation failed
  }
  memcpy(src, input, srcLength);
  src[srcLength] = '\0';  // ensure null-terminated

  const size_t seedLength = strlen(seed);
  const size_t saltLength = seedLength / 2;
  // allocate memory for salt
  uint8_t *salt = (uint8_t *)malloc(saltLength);
  if (salt == nullptr) {
    free(src);
    return false;  // memory allocation failed
  }
  // convert seed to salt
  for (size_t i = 0; i < saltLength; i++) {
    char buf[3] = { seed[i * 2], seed[i * 2 + 1], '\0' };
    unsigned long value = strtoul(buf, NULL, 16);
    if (value > UINT8_MAX) {
      free(src);
      free(salt);
      return false;  // value exceeds uint8_t range
    }
    salt[i] = (uint8_t)value;
  }
  // execute hkdf
  hkdf(dst, dstLength, src, srcLength, salt, saltLength);
  // free allocated memory
  free(src);
  free(salt);
  return true;
}

// HMAC-based Extract-and-Expand Key Derivation Function (HKDF)
void Kdf::hkdf(uint8_t *dst, size_t dstLength, const uint8_t *src, size_t srcLength, const uint8_t *salt, size_t saltLength) {
  // validate input pointers
  if (!dst || !src || !salt) {
    return;
  }
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
