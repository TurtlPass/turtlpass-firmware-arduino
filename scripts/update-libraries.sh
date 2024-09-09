#!/bin/bash
# chmod +x update-libraries.sh

brew update
brew upgrade arduino-cli

arduino-cli lib upgrade FastLED
arduino-cli lib upgrade FastCRC
arduino-cli lib upgrade Crypto
arduino-cli lib upgrade Base32-Decode
arduino-cli lib upgrade base64_encode
