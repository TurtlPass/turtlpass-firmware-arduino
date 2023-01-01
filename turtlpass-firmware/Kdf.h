#ifndef KDF_H
#define KDF_H

#include <Arduino.h>
#include "SHA512.h"
#include "HKDF.h"
#include "Seed.h"
#include "HexUtil.h"

bool processKeyDerivation(uint8_t *output, size_t outputLength, char *input);
void hkdf(const uint8_t *password, size_t passwordLength, const uint8_t *salt, size_t saltLength, uint8_t *key, size_t keyLength);
void hex2Password(const uint8_t *src, size_t srcLength, uint8_t *dst);

#endif // KDF_H
