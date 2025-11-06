# 🧑‍💻 Building TurtlPass Firmware from Source

This guide explains how to **compile, customize, and flash** the **TurtlPass Firmware** using [**PlatformIO**](https://platformio.org/).  
Most users can rely on the **prebuilt UF2 releases** — building from source is ideal for developers or power users.

> 🧠 **TurtlPass now supports over 130 RP2040 / RP2350-based boards**, with automatic driver and LED configuration.

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
* **Platform:** [Max Gerhardt’s Raspberry Pi RP2040 fork](https://github.com/maxgerhardt/platform-raspberrypi)
* **Supported MCUs:** RP2040, RP2350
* **Boards:** 130+ officially supported, including **Raspberry Pi Pico**, **Adafruit**, **Seeed**, **Waveshare**, and many others.

TurtlPass auto-detects compatible drivers and LED configurations at build time, ensuring a unified experience across all supported boards.

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

TurtlPass supports **over 130 boards** based on the **RP2040** and **RP2350** microcontrollers.  
Each board automatically selects the correct **LED driver** (Analog, Cyw43, RGB3Pin, FastLED).


### 🍓 Official Raspberry Pi Pico Boards

| Board                     | Environment | LED Driver | MCU    |
| :------------------------ | :---------- | :--------- | :----- |
| **Raspberry Pi Pico**     | `pico`      | Analog     | RP2040 |
| **Raspberry Pi Pico W**   | `rpipicow`  | Cyw43      | RP2040 |
| **Raspberry Pi Pico 2**   | `rpipico2`  | Analog     | RP2350 |
| **Raspberry Pi Pico 2 W** | `rpipico2w` | Cyw43      | RP2350 |

> 🏷️ *These are the officially supported Raspberry Pi boards, with both RP2040 and RP2350 MCU variants.*


### 🧠 Generic / Custom Boards

Use these environments for any other **RP2040** or **RP2350** design (custom PCBs, dev boards, etc.).  
LED pins and drivers can be configured manually through `build_flags`.

| Board              | Environment      | LED Type | MCU    |
| :----------------- | :--------------- | :------- | :----- |
| **Generic RP2040** | `generic`        | Mono/RGB | RP2040 |
| **Generic RP2350** | `generic_rp2350` | Mono/RGB | RP2350 |

> ⚙️ *Ideal for custom or unlisted RP2040 / RP2350-based boards.*  
> *Configure `TP_PIN_LED` and `TP_RGB_LED` manually via `build_flags`.*


### 🌐 Community & Partner Boards

<details>
<summary>(Click to expand)</summary>

| Board | Environment |
| :---- | :---------- |
| 0xCB Helios | `0xcb_helios` |
| Adafruit Feather RP2040 Adalogger | `adafruit_feather_adalogger` |
| Adafruit Feather RP2040 CAN | `adafruit_feather_can` |
| Adafruit Feather RP2040 DVI | `adafruit_feather_dvi` |
| Adafruit Feather RP2040 Prop-Maker | `adafruit_feather_prop_maker` |
| Adafruit Feather RP2040 RFM | `adafruit_feather_rfm` |
| Adafruit Feather RP2350 Adalogger | `adafruit_feather_rp2350_adalogger` |
| Adafruit Feather RP2350 HSTX | `adafruit_feather_rp2350_hstx` |
| Adafruit Feather RP2040 SCORPIO | `adafruit_feather_scorpio` |
| Adafruit Feather RP2040 ThinkINK | `adafruit_feather_thinkink` |
| Adafruit Feather RP2040 USB Host | `adafruit_feather_usb_host` |
| Adafruit Feather RP2040 | `adafruit_feather` |
| Adafruit Floppsy | `adafruit_floppsy` |
| Adafruit Fruit Jam RP2350 | `adafruit_fruitjam` |
| Adafruit ItsyBitsy RP2040 | `adafruit_itsybitsy` |
| Adafruit KB2040 | `adafruit_kb2040` |
| Adafruit MacroPad RP2040 | `adafruit_macropad2040` |
| Adafruit Metro RP2350 | `adafruit_metro_rp2350` |
| Adafruit Metro RP2040 | `adafruit_metro` |
| Adafruit QT Py RP2040 | `adafruit_qtpy` |
| Adafruit STEMMA Friend RP2040 | `adafruit_stemmafriend` |
| Adafruit Trinkey RP2040 QT | `adafruit_trinkeyrp2040qt` |
| METE HOCA Akana R1 | `akana_r1` |
| Amken BunnyBoard | `amken_bunny` |
| Amken Revelop eS | `amken_revelop_es` |
| Amken Revelop Plus | `amken_revelop_plus` |
| Amken Revelop | `amken_revelop` |
| Arduino Nano RP2040 Connect | `arduino_nano_connect` |
| ArtronShop RP2 Nano | `artronshop_rp2_nano` |
| BIGTREETECH SKR-Pico | `bigtreetech_SKR_Pico` |
| BridgeTek IDM2040-43A | `bridgetek_idm2040_43a` |
| BridgeTek IDM2040-7A | `bridgetek_idm2040_7a` |
| iLabs Challenger 2040 LoRa | `challenger_2040_lora` |
| iLabs Challenger 2040 LTE | `challenger_2040_lte` |
| iLabs Challenger 2040 SD/RTC | `challenger_2040_sdrtc` |
| iLabs Challenger 2040 SubGHz | `challenger_2040_subghz` |
| iLabs Challenger 2040 UWB | `challenger_2040_uwb` |
| iLabs Challenger 2350 WiFi/BLE | `challenger_2350_wifi6_ble5` |
| iLabs Connectivity 2040 LTE/WiFi/BLE | `connectivity_2040_lte_wifi_ble` |
| Cytron IRIV IO Controller | `cytron_iriv_io_controller` |
| Cytron Maker Nano RP2040 | `cytron_maker_nano_rp2040` |
| Cytron Maker Pi RP2040 | `cytron_maker_pi_rp2040` |
| Cytron Maker Uno RP2040 | `cytron_maker_uno_rp2040` |
| Cytron Motion 2350 Pro | `cytron_motion_2350_pro` |
| DatanoiseTV PicoADK v2 | `datanoisetv_picoadk_v2` |
| DatanoiseTV PicoADK | `datanoisetv_picoadk` |
| Degz Robotics Suibo RP2040 | `degz_suibo` |
| DFRobot Beetle RP2040 | `dfrobot_beetle_rp2040` |
| L'atelier d'Arnoz DudesCab | `DudesCab` |
| ElectronicCats HunterCat NFC RP2040 | `electroniccats_huntercat_nfc` |
| EVN Alpha | `evn_alpha` |
| ExtremeElectronics RC2040 | `extelec_rc2040` |
| DeRuiLab FlyBoard2040Core | `flyboard2040_core` |
| GeeekPi RP2040 Plus | `geeekpi_rp2040_plus` |
| GroundStudio Marble Pico | `groundstudio_marble_pico` |
| iLabs RPICO32 | `ilabs_rpico32` |
| Architeuthis Flux Jumperless | `jumperless_v1` |
| Architeuthis Flux Jumperless V5 | `jumperless_v5` |
| Melopero Cookie RP2040 | `melopero_cookie_rp2040` |
| Melopero Shake RP2040 | `melopero_shake_rp2040` |
| Makerbase MKS THR36 | `mksthr36` |
| Makerbase MKS THR42 | `mksthr42` |
| MyMakers RP2040 | `MyRP_bot` |
| Arduino Arduino Nano RP2040 Connect | `nanorp2040connect` |
| Neko Systems BL2040 Mini | `nekosystems_bl2040_mini` |
| Newsan Archi | `newsan_archi` |
| nullbits Bit-C PRO | `nullbits_bit_c_pro` |
| Olimex Pico2XL | `olimex_pico2xl` |
| Olimex Pico2XXL | `olimex_pico2xxl` |
| Olimex RP2040-Pico30 | `olimex_rp2040pico30` |
| Raspberry Pi Raspberry Pi Pico | `pico` |
| Pimoroni Explorer | `pimoroni_explorer` |
| Pimoroni PGA2040 | `pimoroni_pga2040` |
| Pimoroni PGA2350 | `pimoroni_pga2350` |
| Pimoroni PicoPlus2 | `pimoroni_pico_plus_2` |
| Pimoroni PicoPlus2W | `pimoroni_pico_plus_2w` |
| Pimoroni Plasma2040 | `pimoroni_plasma2040` |
| Pimoroni Plasma2350 | `pimoroni_plasma2350` |
| Pimoroni Servo2040 | `pimoroni_servo2040` |
| Pimoroni Tiny2040 | `pimoroni_tiny2040` |
| Pimoroni Tiny2350 | `pimoroni_tiny2350` |
| Pintronix PinMax | `pintronix_pinmax` |
| RAKwireless RAK11300 | `rakwireless_rak11300` |
| redscorp RP2040-Eins | `redscorp_rp2040_eins` |
| redscorp RP2040-ProMini | `redscorp_rp2040_promini` |
| Generic Sea-Picro | `sea_picro` |
| Seeed INDICATOR RP2040 | `seeed_indicator_rp2040` |
| Seeed XIAO RP2040 | `seeed_xiao_rp2040` |
| Seeed XIAO RP2350 | `seeed_xiao_rp2350` |
| Silicognition RP2040-Shim | `silicognition_rp2040_shim` |
| Soldered Electronics NULA RP2350 | `soldered_nula_rp2350` |
| Solder Party RP2040 Stamp | `solderparty_rp2040_stamp` |
| Solder Party RP2350 Stamp XL | `solderparty_rp2350_stamp_xl` |
| Solder Party RP2350 Stamp | `solderparty_rp2350_stamp` |
| SparkFun IoT Node LoRaWAN | `sparkfun_iotnode_lorawanrp2350` |
| SparkFun IoT RedBoard RP2350 | `sparkfun_iotredboard_rp2350` |
| SparkFun MicroMod RP2040 | `sparkfun_micromodrp2040` |
| SparkFun ProMicro RP2040 | `sparkfun_promicrorp2040` |
| SparkFun ProMicro RP2350 | `sparkfun_promicrorp2350` |
| SparkFun Thing Plus RP2040 | `sparkfun_thingplusrp2040` |
| SparkFun Thing Plus RP2350 | `sparkfun_thingplusrp2350` |
| SparkFun XRP Controller (Beta) | `sparkfun_xrp_controller_beta` |
| SparkFun XRP Controller | `sparkfun_xrp_controller` |
| uPesy RP2040 DevKit | `upesy_rp2040_devkit` |
| VCC-GND YD RP2040 | `vccgnd_yd_rp2040` |
| Viyalab Mizu RP2040 | `viyalab_mizu` |
| Waveshare RP2040 LCD 0.96 | `waveshare_rp2040_lcd_0_96` |
| Waveshare RP2040 LCD 1.28 | `waveshare_rp2040_lcd_1_28` |
| Waveshare RP2040 Matrix | `waveshare_rp2040_matrix` |
| Waveshare RP2040 One | `waveshare_rp2040_one` |
| Waveshare RP2040 PiZero | `waveshare_rp2040_pizero` |
| Waveshare RP2040 Plus | `waveshare_rp2040_plus` |
| Waveshare RP2040 Zero | `waveshare_rp2040_zero` |
| Waveshare RP2350 LCD 0.96 | `waveshare_rp2350_lcd_0_96` |
| Waveshare RP2350 PiZero | `waveshare_rp2350_pizero` |
| Waveshare RP2350 Plus | `waveshare_rp2350_plus` |
| Waveshare RP2350 Zero | `waveshare_rp2350_zero` |
| WIZnet W5100S-EVB-Pico | `wiznet_5100s_evb_pico` |
| WIZnet W5100S-EVB-Pico2 | `wiznet_5100s_evb_pico2` |
| WIZnet W5500-EVB-Pico | `wiznet_5500_evb_pico` |
| WIZnet W5500-EVB-Pico2 | `wiznet_5500_evb_pico2` |
| WIZnet W55RP20-EVB-Pico | `wiznet_55rp20_evb_pico` |
| WIZnet W6300-EVB-Pico | `wiznet_6300_evb_pico` |
| WIZnet W6300-EVB-Pico2 | `wiznet_6300_evb_pico2` |
| WIZnet WizFi360-EVB-Pico | `wiznet_wizfi360_evb_pico` |
</details>

---

## ⚙️ Optional Build Flags

Customize firmware parameters via `build_flags`:

| Flag             | Description                     | Default       |
| :--------------- | :------------------------------ | :------------ |
| `TP_VERSION`     | Firmware version string         | `"3.1.0"`     |
| `TP_PIN_LED`     | Onboard LED pin number          | `LED_BUILTIN` |
| `TP_RGB_LED`     | Enable RGB LED (`1`/`0`)        | `0` (false)   |
| `TP_EEPROM_SIZE` | Emulated EEPROM size (bytes)    | `4096`        |
| `TP_PIN_TTP223`  | GPIO pin for touch sensor       | *undefined*   |


### 💡 Inline Override Example

```bash
pio run -e pico -DTP_EEPROM_SIZE=2048
```

---

## 🔄 Building for Other Boards

🧭 **Tip:**
Use `generic` for RP2040 or `generic_rp2350` for RP2350, and manually define LED pin and RGB settings.

### Example

```bash
pio run -e generic \
    -DTP_PIN_LED=16 \
    -DTP_RGB_LED=1
```

**Notes:**

* `TP_PIN_LED` — sets the LED pin number
* `TP_RGB_LED` — set `1` (true) for RGB LED

---

## 👆 Using a Touch Sensor (Optional)

Attach a **TTP-223 capacitive touch sensor** to replace the BOOTSEL button.

```ini
-DTP_PIN_TTP223=2
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
