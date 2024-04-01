<p align="center">
<img src="assets/icon.png" alt="logo" width=90>
<h3 align="center">TurtlPass Firmware</h3>
<p align="center">
TurtlPass Firmware offers a straightforward and secure method for creating robust passwords. It employs a deterministic key derivation function along with unique seed material stored in flash memory. Once activated with a touch, it automatically types the password for you.</p>
<p align="center">
<a href="https://github.com/TurtlPass/turtlpass-firmware-arduino/releases"><img src="https://img.shields.io/github/v/release/TurtlPass/turtlpass-firmware-arduino?color=green&label=Arduino%20Firmware&logo=arduino" alt="Releases"/></a>
</p>
<p align="center">
<a href="https://github.com/TurtlPass/turtlpass-android"><img src="https://img.shields.io/github/v/release/TurtlPass/turtlpass-android?color=blue&label=Android%20App&logo=android" alt="Android Repo"/></a>
<a href="https://github.com/TurtlPass/turtlpass-chrome-extension"><img src="https://img.shields.io/github/v/release/TurtlPass/turtlpass-chrome-extension?color=blue&label=Chrome%20Extension&logo=googlechrome" alt="Chrome Extension Repo"/></a>
<a href="https://github.com/TurtlPass/turtlpass-python"><img src="https://img.shields.io/github/v/release/TurtlPass/turtlpass-python?color=blue&label=Python%20CLI&logo=python" alt="Python CLI Repo"/></a>
</p>


## ⚡ Features

* **Hardware Password Generator**
  * Unlimited passwords are generated on the device
  * Passwords are 100 characters long, including a combination of lowercase and uppercase letters, as well as numbers (plus symbols if you choose to)
  * Automatically types the password for you, so you don't have to
* **Hardware 2FA Manager**
  * One-time passwords are generated on the device
  * Automatically types the OTP code whenever you're ready
  * Shared secrets are encrypted with `ChaCha20` algorithm in the `EEPROM`
* **Hardware Encryption**
  * Files encrypted on the device using the `ChaCha20` algorithm
  * Speed: ~80 kB/s @ 133 Mhz


## 🏗️ Circuit Diagram

```
  RP2040-Zero               Touch Sensor
 +-----------+              +----------+
 |           | GND -------- | GND      |
 |  RGB-LED  |              |          |
 | (GPIO 16) | GPIO 2 ----- | I/O      |
 |           |              |          |
 |           | 3.3V ------- | VCC      |
 +-----------+              +----------+
```

**Connect the Touch Sensor (TTP-223) to RP2040-Zero:**

1. Connect the GND pin of the touch sensor to a ground (GND) pin on the RP2040-Zero board.
2. Connect the I/O pin of the touch sensor to GPIO 2 on the RP2040-Zero board.
3. Connect the VCC pin of the touch sensor to a 3.3V power source on the RP2040-Zero board.


## 🔌 Plug and Play (PnP)

You can connect TurtlPass to pretty much anywhere via USB. To type a default password, *long-touch* the sensor (TTP-223) until the LED fades out completely.

From a security point of view, this feature is useful as it provides decoy passwords for the curious hands of strangers. From the grandma's point of view, is easy to use as it provides very strong passwords without installing any app.


## 💡 LED Color / Seed

**Switch Seed** and **LED Color** by *single-touching* the sensor (TTP-223).

__9 different color/SEED pairs available:__  
1. 🐢 Turtle Green  
2. ☀️ Sunny Yellow  
3. 🍒 Cherry Red  
4. 💙 Electric Blue  
5. ❄️ Snow White  
6. 🔮 Mystic Violet  
7. 🔥 Fire Blaze  
8. 🌊 Aqua Breeze  
9. 🎀 Bubblegum Pink  

<details>
  <summary>💡 LED State Meaning</summary>
  
* `ON`
	* Idle
* `PULSING`
	* Password/OTP ready to type
* `BLINKING` (fast blink)
	* Typing/Encrypting 
* `OFF`
	* No power input
</details>


## 💿 Flashing RP2040

### 1. INSTALL and setup `arduino-cli`

**Run the script** `1-click-setup.sh` to install automatically the `arduino-cli`, RP2040 boards, and all the libraries required. 


### 2. SEED generation

**Run the script** `generate-seed.sh` to generate your unique seed on your local machine. The output file `Seed.cpp` will be added to your codebase.

**IMPORTANT**: Make sure you delete `turtlpass-firmware/Seed.cpp` file once you're done!


### 3. BUILD your custom TurtlPass Firmware

**Run the following command to compile the firmware to your RP2040 board:**

_Option A:_ If you have a touch sensor TTP-223 wired to the PIN number `2`

```
$ arduino-cli compile --clean \
--fqbn "rp2040:rp2040:generic" \
--output-dir ../turtlpass-firmware/build/ \
--build-property "build.extra_flags=\"-D__TURTLPASS_VERSION__=\"2.0.0\"\"" \
--build-property "build.extra_flags=\"-D__TURTLPASS_PIN_TTP223__=2\"" \
../turtlpass-firmware/turtlpass-firmware.ino
```

_Option B:_ If you don't have a touch sensor TTP-223, fallback to built-in `BOOTSEL` button

```
$ arduino-cli compile --clean \
--fqbn "rp2040:rp2040:generic" \
--output-dir ../turtlpass-firmware/build/ \
--build-property "build.extra_flags=\"-D__TURTLPASS_VERSION__=\"2.0.0\"\"" \
../turtlpass-firmware/turtlpass-firmware.ino
```


### 4. UPLOAD TurtlPass Firmware to your RP2040 Board

**Run the following command to upload the firmware to your RP2040 board:**

`$ arduino-cli upload --fqbn "rp2040:rp2040:generic" -i ../turtlpass-firmware/build/turtlpass-firmware.ino.bin -p <PORT>`
	
*Example:*

```
$ arduino-cli upload \
--fqbn "rp2040:rp2040:generic" \
-i ../turtlpass-firmware/build/turtlpass-firmware.ino.bin \
-p /dev/cu.usbmodem14101
```

**IMPORTANT**: Make sure you delete `turtlpass-firmware/build/` directory once you're done!


## 💾 Backup

Just like having multiple copies of your car key for backup, having multiple TurtlPass-RP2040 devices with the same seed provides redundancy and peace of mind. You can keep one device in a secure location as a backup while using another one for daily use.

Alternatively, you can store the seed file offline in a secure location, but for maximum security, it's recommended to store it only inside a TurtlPass-RP2040 device, keeping it offline and inaccessible to potential threats.


## 🛡️ Security

It's important to remember that if someone has physical access to the device, they may be able to compromise the seeds with enough time and effort. However, even if the seeds are compromised, it's impossible to recreate the passwords generated by TurtlPass without access to the hash result of the inputs, such as the PIN code, Domain Name, and Account ID (via the client app).
  
  
<details>
  <summary>Cloning RP2040</summary>
  
It is important to note that the firmware binary on the Raspberry Pi Pico RP2040 can be easily extracted using [picotool](https://github.com/raspberrypi/picotool) due to its utilization of external ROM.  
    
Example of how to do just that:

```
$ picotool save firmware.uf2
Saving file: [==============================]  100%
Wrote 369000 bytes to firmware.uf2
```
</details>

<details>
  <summary>Secure Element (SE)</summary>
  
Using a secure element such as the **ATECC608A/B** or **OPTIGA Trust X/M** for password generation can be a secure solution due to its physical tamper-resistance and isolation from the rest of the system. However, there are limitations to consider such as the difficulty in using them across multiple devices or platforms, and lack of backup or recovery options in case of loss or damage. Additionally, it may be difficult to manage and control access to the password in situations where multiple users need to access it.
</details>


## 🔑 TurtlPass ≠ FIDO

TurtlPass is **not** a **FIDO** Security Key and does **not** intend to be one. If you're looking for that, check [pico-fido](https://github.com/polhenarejos/pico-fido) project. **TurtlPass** is intended for **all the other websites/apps** that don't support hardware security keys, the ones with a `password` field :)


## 📚 Libraries

<details>
  <summary>[Raspberry Pi Pico Arduino core](https://github.com/earlephilhower/arduino-pico)</summary>
  
* Port of the RP2040 (Raspberry Pi Pico processor) to the Arduino ecosystem. 
* It uses the bare Raspberry Pi Pico SDK and a custom GCC 10.3/Newlib 4.0 toolchain.
* _LGPL 2.1 license_
</details>

<details>
  <summary>[Arduino Cryptography Library](https://github.com/rweather/arduinolibs)</summary>
  
* Libraries to perform cryptography operations on Arduino devices
* _MIT license_
</details>

<details>
  <summary>[Keyboard Library for Arduino](https://github.com/arduino-libraries/Keyboard)
</summary>
  
* Library allows an Arduino board with USB capabilities to act as a keyboard
* _LGPL 3.0 license_
</details>


## 📄 License

TurtlPass Firmware is released under the [GPL 3.0 license](https://github.com/TurtlPass/turtlpass-firmware-arduino/blob/main/LICENSE).
