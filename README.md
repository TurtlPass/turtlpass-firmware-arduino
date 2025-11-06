<p align="center">
  <img src="https://raw.githubusercontent.com/TurtlPass/turtlpass-firmware-arduino/master/assets/icon.png" alt="Logo" width="133"/>
</p>

<h2 align="center">🔗 TurtlPass Ecosystem</h2>

<p align="center">
  🐢 <a href="https://github.com/TurtlPass/turtlpass-firmware-arduino"><b>Firmware</b></a> •
  💾 <a href="https://github.com/TurtlPass/turtlpass-protobuf"><b>Protobuf</b></a> •
  💻 <a href="https://github.com/TurtlPass/turtlpass-python"><b>Host</b></a> •
  🌐 <a href="https://github.com/TurtlPass/turtlpass-chrome-extension"><b>Chrome</b></a> •
  📱 <a href="https://github.com/TurtlPass/turtlpass-android"><b>Android</b></a>
</p>

---

# 🐢 TurtlPass Firmware

[![](https://img.shields.io/github/v/release/TurtlPass/turtlpass-firmware-arduino?color=green&label=Release&logo=arduino)](https://github.com/TurtlPass/turtlpass-firmware-arduino/releases/latest "GitHub Release")
[![](https://img.shields.io/badge/Build-PlatformIO-blue?logo=platformio)](https://platformio.org/ "PlatformIO")
[![](https://img.shields.io/badge/130%2B_Boards-Supported-success?logo=raspberrypi)](https://github.com/TurtlPass/turtlpass-firmware-arduino#-supported-boards "130+ Boards Supported")
[![](https://img.shields.io/badge/UF2-Prebuilt%20Binaries%20Available-brightgreen?logo=usb)](https://github.com/TurtlPass/turtlpass-firmware-arduino/releases "Precompiled UF2 Files Available")
[![](https://img.shields.io/badge/Documentation-green?label=GitBook&logo=gitbook)](https://ryanamaral.gitbook.io/turtlpass "GitBook Documentation")
[![](https://img.shields.io/badge/License-GPL%203.0-green.svg)](https://opensource.org/license/GPL-3.0 "License: GPL-3.0")


**TurtlPass** is a hardware-based password generator built for **simplicity, security, and portability**.
It deterministically derives passwords from **encrypted seeds stored directly in the microcontroller’s flash**, keeping all sensitive data self-contained and offline.

With a single button and integrated LED feedback, TurtlPass can **generate and type passwords directly into any device** — no software required.
For advanced management, account-specific passwords, or seamless integration, TurtlPass connects with companion tools:

* **[TurtlPass Host (Python CLI)](https://github.com/TurtlPass/turtlpass-python)** — Manage and configure TurtlPass devices via USB from the terminal.
* **[TurtlPass Chrome Extension](https://github.com/TurtlPass/turtlpass-chrome-extension)** — Securely auto-type passwords from Chrome using your device.
* **[TurtlPass Android](https://github.com/TurtlPass/turtlpass-android)** — Generate and auto-type passwords via USB on Android devices.

---

## ⚡ Features

### 🔐 Hardware Password Generator

* **Deterministic & unique:** Generates reproducible passwords directly on the device — perfect for multiple accounts.
* **Flexible length & complexity:** Passwords can be **1–128 characters** (default 100) and include **numbers, letters, or symbols**, like any password generator.
* **Instant input:** Passwords are typed automatically into any active field via the device — no software required.
* **Offline & secure:** Completely offline — no cloud, no sync, no leaks.

### 🧬 Seed Management

* **Secure & encrypted:** Each seed is stored in emulated EEPROM and encrypted with **ChaCha20**.
* **Multiple slots:** Each LED color represents a unique seed, allowing multiple identities or accounts.
* **Reliable backups:** Backup-friendly — reflash, duplicate, or mnemonic restore.
* **Self-contained storage:** Seeds never leave the device — no cloud storage required.

### 🔌 Plug & Play Simplicity

* **Instant usability:** Connect via USB and **long press the device button** to type your default password automatically.
* **Intuitive controls:** Single press cycles through seed slots; long press types the password. LED colors provide clear visual feedback.
* **Secure convenience:** Provides decoy passwords for curious onlookers while keeping strong, unique passwords at your fingertips.
* **Cross-platform ready:** Works anywhere with USB support — no drivers or software installation required.

---

## 🧠 Mnemonic-Based Seeds

The **[TurtlPass Host (Python CLI)](https://github.com/TurtlPass/turtlpass-python)** allows you to create or restore **512-bit seed** from **24-word BIP-39 mnemonics** — the same standard used by many hardware wallets.

This provides several benefits:

* **Reproducibility:** Generate the same deterministic passwords across multiple devices.
* **Safe backups:** Store your seed securely offline on paper or metal.
* **Device recovery:** Restore your TurtlPass device if it’s lost or damaged, using only the mnemonic.

---

## 💡 LED Behavior

Each LED color corresponds to a **unique seed slot**, providing visual feedback for selection and activity.

| Color     | Seed Slot |
| :-------- | :-------- |
| 🟢 Green  | 1         |
| 🟡 Yellow | 2         |
| 🔴 Red    | 3         |
| 🔵 Blue   | 4         |
| ⚪ White  | 5         |
| 🟣 Violet | 6         |
| 🟠 Orange | 7         |
| 🩵 Aqua   | 8         |
| 🩷 Pink   | 9         |

**Touch Controls:**

* *Single press*: Switch to the next seed/color.
* *Long press*: Type the default password for the current seed.

---

## ✨ LED States

| LED State      | Meaning            |
| -------------- | ------------------ |
| 💡 **ON**      | Idle               |
| 🌟 **PULSING** | Password ready     |
| ⚡ **BLINKING** | Typing in progress |
| ⚫️ **OFF**     | No power           |

---

## 🚀 Supported Boards

TurtlPass adds **full LED and firmware support for 130 boards**, including the most popular RP2040 and RP2350 designs:

| Vendor / Family      | Examples                                                         |
| -------------------- | ---------------------------------------------------------------- |
| **Raspberry Pi**     | Pico, Pico W, Pico 2, Pico 2W                                    |
| **Adafruit**         | Feather RP2040, ItsyBitsy, QT Py, Trinkey                        |
| **Seeed Studio**     | Xiao RP2040, Xiao RP2350, Xiao Indicator RP2040                  |
| **SparkFun**         | Thing Plus RP2040, ProMicro RP2040, MicroMod RP2040              |
| **Pimoroni**         | Tiny2040, Tiny2350, Explorer, PicoPlus2, PicoPlus2W              |
| **Waveshare**        | RP2040-Zero, RP2040-One, RP2040-Plus                             |
| **Generic / Custom** | All RP2040 and RP2350 boards with matching pinout and bootloader |

Each supported board automatically selects the appropriate **LED driver** and configuration at runtime.

---

## ⬇️ Download

TurtlPass Firmware now includes **precompiled UF2 files for 130 RP2040 and RP2350-based boards** 🎉 — including the Raspberry Pi Pico, Pico 2, Pico W, Pico 2W, and dozens of community and manufacturer variants.

Each UF2 build is optimized for its target board, with:

* Correct **pin mapping** and **LED driver selection** (Analog, Cyw43, RGB3Pin, FastLED)
* Built-in **USB VID/PID** identifiers
* Verified **PlatformIO and Arduino SDK** compatibility

You can browse and download all available UF2 binaries from the
👉 [**Releases page**](https://github.com/TurtlPass/turtlpass-firmware-arduino/releases)

> **Note:**
> UF2 builds use TurtlPass-assigned USB identifiers: **VID:** `0x1209` and **PID:** `0xFA55`.  
> The official IDs are registered to TurtlPass via [pid.codes](https://pid.codes/1209/FA55/).

---

## 🧑‍💻 Building from Source

TurtlPass Firmware uses **[PlatformIO](https://platformio.org/)** for building, flashing, and dependency management.
Developers can easily compile and upload **custom builds** targeting any supported board or their own hardware variant.

For step-by-step setup and build instructions, see the full guide:
👉 [**BUILD.md**](./BUILD.md)

---

## 📥 Cloning the RP2040

The RP2040 firmware can be extracted using [**picotool**](https://github.com/raspberrypi/picotool):

```bash
$ picotool save firmware.uf2
Saving file: [==============================]  100%
Wrote 369000 bytes to firmware.uf2
```

> **Note:** Extracting the firmware does not allow backup of the seed, as it is encrypted using the unique board ID. Passwords also cannot be recreated from the firmware alone, since they are derived from a combination of seed, domain, account ID, and user PIN, ensuring irrecoverability.

---

## 🧩 Architecture Overview

```
┌──────────────────────────────────────────────────────────────┐
│ User Interaction (Button / Touch)                            │
│        │                                                     │
│        ▼                                                     │
│ ┌─────────────────────────────────────────────────────────┐  │
│ │   🐢 TurtlPass Core (RP2040 / RP2350 Microcontroller)   │  │
│ │─────────────────────────────────────────────────────────│  │
│ │ 🔐 Seed Storage (Encrypted Flash / EEPROM)              │  │
│ │    └─ Encrypted with ChaCha20                           │  │
│ │    └─ Salt = Unique Board ID                            │  │
│ │                                                         │  │
│ │ ⚙️ Password Derivation                                  │  │
│ │    └─ Inputs: {Seed, Domain, Account ID, PIN}           │  │
│ │    └─ Deterministic generator                           │  │
│ │                                                         │  │
│ │ 🌈 LED Controller (FastLED)                             │  │
│ │    └─ Indicates seed slot & status                      │  │
│ │                                                         │  │
│ │ ⌨️ USB HID Keyboard Interface                           │  │
│ │    └─ Types password into host system                   │  │
│ └─────────────────────────────────────────────────────────┘  │
│        │ USB / Serial Connection                             │
│        ▼                                                     │
│ ┌─────────────────────────────────────────────────────────┐  │
│ │                    Host Integrations                    │  │
│ │  💻 Python CLI  |  🌐 Chrome Extension  |  📱 Android  │  │
│ └─────────────────────────────────────────────────────────┘  │
│        │                                                     │
│        ▼                                                     │
│ Websites / Apps / Login Fields (Password typed automatically)│
└──────────────────────────────────────────────────────────────┘
```

---

## 💾 Backups

* **Redundant storage:** Flash the same seed to multiple devices to ensure availability in case of loss or failure.
* **Offline mnemonic:** Optionally, keep a **mnemonic of the seed offline** for secure recovery.
* **Device-first security:** Keeping seeds only on the device is the **most secure option**, minimizing exposure to online threats.

---

## 🛡️ Security Notes

* **Strong, deterministic passwords:** Even with physical access, extracting the seed alone is **not enough** to reproduce passwords.
* **Unique per account:** Passwords are derived from a combination of **seed, domain, account ID, and user PIN**, ensuring uniqueness and irrecoverability.
* **Offline-first security:** All cryptographic operations occur **on-device**, eliminating cloud exposure and minimizing attack surfaces.
* **Optional backups:** Redundant seeds or offline mnemonics provide recovery options without compromising security.

---

## 📚 Dependencies

TurtlPass Firmware relies on the following libraries and platforms:

* **Arduino-Pico** – [Earle Philhower’s core](https://github.com/earlephilhower/arduino-pico) for RP2040 Arduino support
* **RP2040 Platform** – [Max Gerhardt’s fork](https://github.com/maxgerhardt/platform-raspberrypi) for additional board support
* [Arduino Cryptography Library](https://github.com/rweather/arduinolibs) – Provides cryptographic primitives for Arduino devices
* [FastLED](https://github.com/FastLED/FastLED) – Library for controlling RGB LEDs
* [nanopb](https://github.com/nanopb/nanopb) – Protocol Buffers implementation for embedded systems
* [Base32-Decode](https://github.com/ekscrypto/Base32/) – Library for Base32 encoding/decoding
* [Base62](https://github.com/ericherman/base62) – Library for Base62 encoding/decoding
* [Base94](https://github.com/Holmojan/base94) – Library for Base94 encoding/decoding

> **Credits:** Thanks to the maintainers of these open-source projects for enabling secure, portable, and efficient firmware development.

---

## 📜 License

This repository is licensed under the [GPL 3.0 license](./LICENSE).
