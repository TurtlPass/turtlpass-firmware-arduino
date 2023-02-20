#include "OtpCode.h"

uint32_t OtpCode::generateOtpCode(char* otpSecret, uint32_t timestamp, uint32_t durationInSeconds) {
  uint16_t otpSecretLength = base32decode(otpSecret, NULL, 0); // figure out the lenght we're going to need
  otpSecretLength += 1; // keep room for terminating \0
  uint8_t otpSecretBase32[otpSecretLength];
  int r = base32decode(otpSecret, (uint8_t*) otpSecretBase32, otpSecretLength);
  if (r < 0) return 0;
  otpSecretBase32[otpSecretLength - 1] = '\0';
  return generateOtpCode(otpSecretBase32, otpSecretLength, timestamp, durationInSeconds);
}

uint32_t OtpCode::generateOtpCode(uint8_t* otpSecretBase32, uint16_t otpSecretLength, uint32_t timestamp, uint32_t durationInSeconds) {
  // calculating counter value
  uint32_t steps = (timestamp) / durationInSeconds;
  uint8_t counter[8];
  counter[0] = 0x00;
  counter[1] = 0x00;
  counter[2] = 0x00;
  counter[3] = 0x00;
  counter[4] = (uint8_t) ((steps >> 24) & 0xFF);
  counter[5] = (uint8_t) ((steps >> 16) & 0xFF);
  counter[6] = (uint8_t) ((steps >> 8) & 0XFF);
  counter[7] = (uint8_t) ((steps & 0XFF));

  Sha1.initHmac(otpSecretBase32, otpSecretLength);
  Sha1.writeBytes(counter, 8);
  uint8_t *hash = Sha1.resultHmac();

  // apply dynamic truncation to obtain a 4-bytes string
  uint8_t offset = hash[20 - 1] & 0xF;
  uint32_t truncatedHash = 0;

  for (uint8_t j = 0; j < 4; ++j) {
    truncatedHash <<= 8;
    truncatedHash |= hash[offset + j];
  }
  // compute the OTP value
  truncatedHash &= 0x7FFFFFFF;
  truncatedHash %= 1000000;
  return truncatedHash;
}
