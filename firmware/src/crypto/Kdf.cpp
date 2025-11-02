#include "crypto/Kdf.h"

////////////
// Public //
////////////

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

bool Kdf::derivatePass(uint8_t *dst, size_t dstLength, const char *input, const char *seed) {
  auto keyLenFunc = base62InputLength;
  auto encodeFunc = [](const std::vector<uint8_t>& key, std::string& out) -> bool {
    size_t encodedKeyLength = (key.size() * 2) + 1;
    char *encoded = base62_encode((char *)malloc(encodedKeyLength), encodedKeyLength, key.data(), key.size());
    if (!encoded) return false;
    out.assign(encoded);
    free(encoded);
    return true;
  };
  return deriveAndEncode(dst, dstLength, input, seed, keyLenFunc, encodeFunc);
}

bool Kdf::derivatePassWithSymbols(uint8_t *dst, size_t dstLength, const char *input, const char *seed) {
  auto keyLenFunc = base94InputLength;
  auto encodeFunc = [](const std::vector<uint8_t>& key, std::string& out) -> bool {
    Base94 base94;
    return base94.encode(key, out);
  };
  return deriveAndEncode(dst, dstLength, input, seed, keyLenFunc, encodeFunc);
}

bool Kdf::derivatePassLettersOnly(uint8_t *dst, size_t dstLength, const char *input, const char *seed) {
  auto keyLenFunc = base52InputLength;
  return deriveAndEncode(
      dst,
      dstLength,
      input,
      seed,
      keyLenFunc,
      encodeLettersOnly
  );
}

bool Kdf::derivatePassNumbersOnly(uint8_t *dst, size_t dstLength, const char *input, const char *seed) {
  auto keyLenFunc = base10InputLength;
  return deriveAndEncode(
      dst,
      dstLength,
      input,
      seed,
      keyLenFunc,
      encodeNumbersOnly
  );
}


/////////////
// Private //
/////////////

bool Kdf::deriveAndEncode(
    uint8_t *dst,
    size_t dstLength,
    const char *input,
    const char *seed,
    size_t (*keyLengthFunc)(size_t), // baseXXInputLength
    std::function<bool(const std::vector<uint8_t>&, std::string&)> encodeFunc // baseXX encoder
) {
  if (!dst || !input || !seed) {
    return false;
  }

  // Prepare input with appended dstLength
  char lengthStr[32];
  snprintf(lengthStr, sizeof(lengthStr), "%zu", dstLength);
  std::string inputWithLength(input);
  inputWithLength += lengthStr;

  // Allocate key buffer
  size_t keyLength = keyLengthFunc(dstLength);
  std::vector<uint8_t> key(keyLength);
  if (key.empty()) {
    return false;
  }

  // Derive key
  if (!derivateKey(key.data(), keyLength, inputWithLength.data(), seed)) {
    return false;
  }

  // Encode key using provided encoding function
  std::string encodedKey;
  if (!encodeFunc(key, encodedKey)) {
    return false;
  }

  // Copy encoded result to destination
  size_t copyLength = std::min(encodedKey.size(), dstLength);
  std::memcpy(dst, encodedKey.data(), copyLength);
  dst[copyLength] = '\0';
  return true;
}

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

size_t Kdf::base62InputLength(size_t dstLength) {
    // Base62 encodes ~1 byte as 1.37 characters (log(256)/log(62) ≈ 1.37)
    // To get at least dstLength output chars, we need roughly dstLength / 1.37 bytes.
    // Using ceil to round up ensures the encoded output is long enough.
    static const double BASE62_RATIO = std::log(256.0) / std::log(62.0);
    size_t keyLen = static_cast<size_t>(std::ceil(dstLength / BASE62_RATIO));
    return keyLen < 2 ? 2 : keyLen;  // ensure a minimum of 2 bytes
}

size_t Kdf::base94InputLength(size_t encodedLength) {
    // calculate the number of output blocks
    uint32_t outputBlocks = (encodedLength + BASE94_OUTPUT_BLOCK_SIZE - 1) / BASE94_OUTPUT_BLOCK_SIZE;
    // calculate the number of input blocks
    uint32_t inputBlocks = outputBlocks * BASE94_INPUT_BLOCK_SIZE;
    // return the total input length
    return inputBlocks;
}

size_t Kdf::base52InputLength(size_t dstLength) {
    return dstLength; // 1:1 mapping
}

size_t Kdf::base10InputLength(size_t dstLength) {
    // Each byte is mapped to a digit (0–9)
    return dstLength; // 1:1 mapping
}

bool Kdf::encodeLettersOnly(const std::vector<uint8_t>& key, std::string& out) {
    const char LETTERS[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const size_t LETTERS_COUNT = sizeof(LETTERS) - 1; // 52

    out.clear();
    for (size_t i = 0; i < key.size(); ++i) {
        out += LETTERS[key[i] % LETTERS_COUNT];
    }
    return true;
}

bool Kdf::encodeNumbersOnly(const std::vector<uint8_t>& key, std::string& out) {
    const char DIGITS[] = "0123456789";
    const size_t DIGIT_COUNT = sizeof(DIGITS) - 1; // 10

    out.clear();
    out.reserve(key.size());
    for (size_t i = 0; i < key.size(); ++i) {
        out += DIGITS[key[i] % DIGIT_COUNT];
    }
    return true;
}
