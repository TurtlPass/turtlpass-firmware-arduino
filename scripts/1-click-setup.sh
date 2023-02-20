#!/bin/bash
# chmod +x 1-click-setup.sh

brew update
brew install arduino-cli
# or
# curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | BINDIR=~/.local/bin sh

arduino-cli config init --overwrite
arduino-cli core update-index
arduino-cli core install --additional-urls https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json rp2040:rp2040
arduino-cli config set library.enable_unsafe_install true
arduino-cli lib install --git-url git@github.com:rweather/arduinolibs.git
arduino-cli lib install --git-url git@github.com:Seeed-Studio/Seeed_Arduino_mbedtls.git
arduino-cli lib install --git-url git@github.com:dirkx/Arduino-Base32-Decode.git
arduino-cli lib install --git-url git@github.com:FrankBoesing/FastCRC.git
arduino-cli lib install --git-url git@github.com:dojyorin/arduino_base64.git
arduino-cli lib install --git-url git@github.com:FastLED/FastLED.git
