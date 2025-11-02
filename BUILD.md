# 🧑‍💻 Building TurtlPass Firmware from Source

This guide explains how to compile and customize the **TurtlPass Firmware** using [**PlatformIO**](https://platformio.org/).  
Most users can rely on pre-built releases — building from source is ideal for developers or power users.

---

## 📚 Table of Contents

* [⚙️ Overview](#️-overview)
* [🧰 Prerequisites](#-prerequisites)
* [🚀 Quick Start](#-quick-start)
* [⚡ Building and Uploading](#-building-and-uploading)
* [🖥️ Supported Boards / Environments](#️-supported-boards--environments)
* [⚙️ Optional Build Flags](#️-optional-build-flags)
* [🔄 Building for Other Boards](#-building-for-other-boards)
* [👆 Using a Touch Sensor (Optional)](#-using-a-touch-sensor-optional)
* [🧪 Testing](#-testing)
* [⚙️ Advanced PlatformIO Commands](#️-advanced-platformio-commands)
* [💡 VS Code Integration](#-vs-code-integration)
* [🧱 Troubleshooting](#-troubleshooting)
* [🚀 Next Steps](#-next-steps)

---

## ⚙️ Overview

* **Core:** [Earle Philhower’s Arduino-Pico](https://github.com/earlephilhower/arduino-pico)
* **Platform:** [Max Gerhardt’s RP2040 fork](https://github.com/maxgerhardt/platform-raspberrypi)

TurtlPass firmware supports **RP2040** and **RP2350** microcontrollers across a wide range of boards while maintaining full Arduino compatibility.

---

## 🧰 Prerequisites

* [PlatformIO Core or VSCode Extension](https://platformio.org/install/ide?install=vscode)
* Supported RP2040 / RP2350 board
* USB cable for flashing (with data support)
* *(Optional)* TTP-223 capacitive touch sensor

---

## 🚀 Quick Start

1. **Clone the repository**

   ```bash
   git clone https://github.com/TurtlPass/turtlpass-firmware-arduino.git
   cd turtlpass-firmware-arduino/firmware
   ```

2. **Build the firmware**

   ```bash
   pio run -e pico
   ```

3. **(Optional) Upload to your board**

   ```bash
   pio run -e pico -t upload
   ```

> 💡 **Tip:** PlatformIO automatically handles toolchains, dependencies, and builds — no manual setup required.

---

## ⚡ Building and Uploading

```bash
# Build firmware
pio run -e <env>

# Upload firmware
pio run -e <env> -t upload
```

Replace `<env>` with one of the [supported environments](#️-supported-boards--environments).

---

## 🖥️ Supported Boards / Environments

| Board                                 | Environment                | LED Type     |
| :------------------------------------ | :------------------------- | :----------- |
| Raspberry Pi Pico (RP2040)            | `pico`                     | Single-color |
| Raspberry Pi Pico W (RP2040 + Wi-Fi)  | `rpipicow`                 | Single-color |
| Raspberry Pi Pico 2 (RP2350)          | `rpipico2`                 | Single-color |
| Raspberry Pi Pico 2W (RP2350 + Wi-Fi) | `rpipico2w`                | Single-color |
| Adafruit Feather RP2040               | `adafruit_feather`         | RGB          |
| Adafruit QT Py RP2040                 | `adafruit_qtpy`            | RGB          |
| Adafruit Trinkey QT2040               | `adafruit_trinkeyrp2040qt` | RGB          |
| Seeed Xiao RP2040                     | `seeed_xiao_rp2040`        | RGB          |
| Seeed Xiao RP2350                     | `seeed_xiao_rp2350`        | RGB          |
| Waveshare RP2040-Zero                 | `waveshare_rp2040_zero`    | RGB          |
| Waveshare RP2350-Zero                 | `waveshare_rp2350_zero`    | RGB          |
| **Generic RP2040**                    | `generic`                  | Single/RGB   |
| **Generic RP2350**                    | `generic_rp2350`           | Single/RGB   |

---

## ⚙️ Optional Build Flags

Customize firmware parameters via `build_flags`:

| Flag                        | Description                     | Default       |
| :-------------------------- | :------------------------------ | :------------ |
| `__TURTLPASS_VERSION__`     | Firmware version string         | `"3.0.0"`     |
| `__TURTLPASS_LED_PIN__`     | Onboard LED pin number          | `LED_BUILTIN` |
| `__TURTLPASS_LED_IS_RGB__`  | Enable RGB LED (`true`/`false`) | `false`       |
| `__TURTLPASS_EEPROM_SIZE__` | Emulated EEPROM size (bytes)    | `4096`        |
| `__TURTLPASS_PIN_TTP223__`  | GPIO pin for touch sensor       | *undefined*   |

### 💡 Inline Override Example

```bash
pio run -e pico -D__TURTLPASS_EEPROM_SIZE__=2048
```

---

## 🔄 Building for Other Boards

TurtlPass supports any board based on **RP2040** or **RP2350** — including fully custom configurations.

---

### 🧩 Supported Boards Reference

See the full list here:
👉 [maxgerhardt/platform-raspberrypi — boards](https://github.com/maxgerhardt/platform-raspberrypi/tree/develop/boards)

To build for one, create a new environment in your `platformio.ini` file.

#### ⚙️ Example — Waveshare RP2040 Zero

```ini
[env:waveshare_rp2040_zero]
extends = env:base
board = waveshare_rp2040_zero
build_flags =
    ${env:base.build_flags}
    -D__TURTLPASS_LED_IS_RGB__=true ; RGB LED
```

Then build it:

```bash
pio run -e waveshare_rp2040_zero
```

---

### 🧠 Custom Boards

> 🧭 **Tip:**
> Use `generic` for RP2040 or `generic_rp2350` for RP2350, and manually define LED pin and RGB settings.

#### Example

```bash
pio run -e generic \
    -D__TURTLPASS_LED_PIN__=16 \
    -D__TURTLPASS_LED_IS_RGB__=true
```

**Notes:**

* `__TURTLPASS_LED_PIN__` — sets the LED pin number
* `__TURTLPASS_LED_IS_RGB__` — set `true` for RGB LED

---

## 👆 Using a Touch Sensor (Optional)

Attach a **TTP-223 capacitive touch sensor** to replace the BOOTSEL button.

```ini
-D__TURTLPASS_PIN_TTP223__=2
```

This configures the firmware to use the external touch input instead of **BOOTSEL**.

### 🔌 Circuit Diagram

```
 RP2040/RP2350            Touch Sensor
 +-----------+            +----------+
 |       GND | ---------- | GND      |
 |    GPIO 2 | ---------- | I/O      |
 |      3.3V | ---------- | VCC      |
 +-----------+            +----------+
```

**Connection steps:**

1. Connect **GND → GND**
2. Connect **GPIO 2 → I/O**
3. Connect **3.3V → VCC**

---

## 🧪 Testing

TurtlPass includes both **embedded** (hardware-level) and **native** (host-level) tests.

### 🧭 Embedded Tests — Run on Hardware

```bash
pio test -e pico-tests
pio test -e pico-tests --filter embedded/test_seedmanager_basic
```

### 💻 Native Tests — Run on Host Machine

```bash
pio test -e native
pio test -e native --filter native/test_key_derivation
```

> ⚡ **Note:** Native tests run on your PC — fast, reproducible, and ideal for CI pipelines.

---

## ⚙️ Advanced PlatformIO Commands

### 🔄 Clean and Rebuild

```bash
pio run -t clean
pio run -e pico
```

### 📦 Generate `.uf2` Firmware (Manual Flash)

```bash
pio run -e pico --target buildfs
```

Output file:

```
.pio/build/<env>/firmware.uf2
```

---

## 💡 VS Code Integration

When using **PlatformIO in VS Code**:

* ▶️ **Build** compiles the firmware
* 🔌 **Upload** flashes it automatically
* 🧩 Auto-detects connected devices and can enter **BOOTSEL mode**

> 🪫 **If upload fails:**
> Drag `.pio/build/pico/firmware.uf2` onto the **RPI-RP2** drive manually.

---

## 🧱 Troubleshooting

### 🧭 General Checks

* Run with verbose output: `pio run -v`
* Hold **BOOTSEL** while connecting
* Check connected devices:

  ```bash
  pio device list
  ```

### ❌ `Unknown board` or `Missing platform`

Install the correct platform fork:

```bash
pio platform install https://github.com/maxgerhardt/platform-raspberrypi.git
```

### ⚠️ `Upload failed`

1. Hold **BOOTSEL**
2. Connect USB
3. Release when it mounts as a drive
   Then rerun:

```bash
pio run -e pico -t upload
```

### ⚙️ Missing Arduino Core

Add this line to enforce:

```ini
board_build.core = earlephilhower
```

---

## 🚀 Next Steps

Now that you’ve built the **TurtlPass Firmware**, explore the rest of the [TurtlPass Ecosystem](https://github.com/TurtlPass) to unlock its full potential.

Happy hacking! ✨🐢
