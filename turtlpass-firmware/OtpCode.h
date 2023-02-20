/**
  TOTP (RFC-6238, Google Authenticator)
*/
#ifndef OTP_CODE_H
#define OTP_CODE_H

#include "Arduino.h"
#include <Base32-Decode.h> // https://github.com/dirkx/Arduino-Base32-Decode
#include "Sha1.h"

class OtpCode {
  public:
    uint32_t generateOtpCode(char* otpSecret, uint32_t timestamp, uint32_t durationInSeconds);
    uint32_t generateOtpCode(uint8_t* otpSecretBase32, uint16_t otpSecretLength, uint32_t timestamp, uint32_t durationInSeconds);
};

#endif // OTP_CODE_H
