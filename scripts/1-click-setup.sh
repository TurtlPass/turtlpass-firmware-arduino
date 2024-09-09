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

arduino-cli lib install --git-url git@github.com:FastLED/FastLED.git

arduino-cli lib install --git-url git@github.com:FrankBoesing/FastCRC.git

# rweather/arduinolibs (Crypto)
curl -JLO "https://github.com/rweather/arduinolibs/archive/refs/heads/master.zip"
arduino-cli lib install --zip-path arduinolibs-master.zip

# dirkx/Arduino-Base32-Decode
b32_latest_version=$(curl -s https://api.github.com/repos/dirkx/Arduino-Base32-Decode/releases/latest | grep tag_name  | cut -d '"' -f 4)
curl -LO "https://github.com/dirkx/Arduino-Base32-Decode/archive/refs/tags/${b32_latest_version}.zip"
arduino-cli lib install --zip-path "${b32_latest_version}.zip"

# dojyorin/arduino_base64
b64_latest_version=$(curl -s https://api.github.com/repos/dojyorin/arduino_base64/releases/latest | grep tag_name  | cut -d '"' -f 4)
curl -LO "https://github.com/dojyorin/arduino_base64/archive/refs/tags/${b64_latest_version}.zip"
arduino-cli lib install --zip-path "${b64_latest_version}.zip"
