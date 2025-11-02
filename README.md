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
[![](https://img.shields.io/badge/Board-RP2040%20/%20RP2350%20based-lightgrey?logo=raspberrypi)](https://www.raspberrypi.com/products/raspberry-pi-pico/ "Raspberry Pi Pico")
[![](https://img.shields.io/badge/License-GPL%203.0-green.svg)](https://opensource.org/license/GPL-3.0 "License: GPL-3.0")
[![](https://img.shields.io/badge/Documentation-green?label=GitBook&logo=gitbook)](https://ryanamaral.gitbook.io/turtlpass "GitBook Documentation")

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

## 📥 Cloning the RP2040

The RP2040 firmware can be extracted using [**picotool**](https://github.com/raspberrypi/picotool):

```bash
$ picotool save firmware.uf2
Saving file: [==============================]  100%
Wrote 369000 bytes to firmware.uf2
```

> **Note:** Extracting the firmware does not allow backup of the seed, as it is encrypted using the unique board ID. Passwords also cannot be recreated from the firmware alone, since they are derived from a combination of seed, domain, account ID, and user PIN, ensuring irrecoverability.

---

## ⬇️ Download

If you own a **Raspberry Pi Pico (RP2040 or RP2350)**, you can download the precompiled **UF2 file** for your board from the [**Releases page**](https://github.com/TurtlPass/turtlpass-firmware-arduino/releases).

UF2 files are shipped with TurtlPass-specific USB VID/PID settings. You may customize these values if you build TurtlPass Firmware from source, but **do not distribute binaries using a VID/PID you do not own**.

> **Note:** The VID `0x1209` and PID `0xFA55` are registered to TurtlPass. Changing them allows custom identification but must comply with USB VID/PID regulations.

---

## 🧑‍💻 Building from Source

TurtlPass Firmware uses **[PlatformIO](https://platformio.org/)** for building, flashing, and dependency management.
Power users and developers can build and flash **custom versions** tailored to their boards or preferences.

For step-by-step instructions, see the full guide: [**BUILD.md**](./BUILD.md)

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
