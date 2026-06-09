<!-- OceanLabz AI Explorer S3 Board — Official Repository -->

<div align="center">
  <!-- <img src="assets/logos/oceanlabz_logo.png" alt="OceanLabz Logo" width="200"/> -->
  <h1> OceanLabz AI Explorer S3 Board</h1>
  <p><strong>Your Gateway to AI, Audio, and Embedded Vision — on ESP32-S3</strong></p>

  <!-- Badge Row -->
  <p>
    <a href="LICENSE"><img src="https://img.shields.io/badge/license-MIT-blue.svg" alt="License: MIT"/></a>
    <a href="https://www.arduino.cc/"><img src="https://img.shields.io/badge/Arduino-IDE-00979D?logo=arduino" alt="Arduino IDE"/></a>
    <a href="https://docs.espressif.com/projects/arduino-esp32/en/latest/"><img src="https://img.shields.io/badge/ESP32--S3-Arduino%20Core-E7352C?logo=espressif" alt="ESP32-S3 Arduino Core"/></a>
    <a href="https://github.com/ocean-labz/OceanLabz-AI-Explorer-S3-Board/releases"><img src="https://img.shields.io/github/v/release/ocean-labz/OceanLabz-AI-Explorer-S3-Board" alt="Latest Release"/></a>
    <a href="https://github.com/ocean-labz/OceanLabz-AI-Explorer-S3-Board/issues"><img src="https://img.shields.io/badge/contributions-welcome-brightgreen.svg" alt="Contributions Welcome"/></a>
  </p>

  <p>
    <a href="#-overview">Overview</a> •
    <a href="#-features">Features</a> •
    <a href="#-hardware-overview">Hardware</a> •
    <a href="#-pin-mapping">Pin Map</a> •
    <a href="#-getting-started">Getting Started</a> •
    <a href="#-examples">Examples</a> •
    <a href="#-documentation">Docs</a> •
    <a href="#-troubleshooting">Troubleshooting</a>
  </p>
</div>

---

## 📖 Overview

The **OceanLabz AI Explorer S3 Board** is a feature-rich development platform built around the **Espressif ESP32-S3** microcontroller. Designed for students, hobbyists, and makers, it integrates audio input/output, camera support, display interfaces, SD card storage, and battery management — all on a single board.

This repository provides everything you need to get started: Arduino-compatible example sketches, documentation, pin configurations, and community resources.

> **Target Audience:** Students • Hobbyists • Makers
> **Primary Framework:** [Arduino IDE](https://www.arduino.cc/)
> **Repository:** [github.com/ocean-labz/OceanLabz-AI-Explorer-S3-Board](https://github.com/ocean-labz/OceanLabz-AI-Explorer-S3-Board)

---

## ✨ Features

### 🎤 Audio
| Component | Interface | Pins |
|-----------|-----------|------|
| **INMP441** Digital Microphone | I²S | WS=GPIO39, SCK=GPIO40, SD=GPIO41 |
| **MAX98357A** Speaker Amplifier (3W) | I²S | LRCK=GPIO21, BCLK=GPIO48, DIN=GPIO47 |

### 🖥️ Display (Choose One)
| Type | Interface | Pins |
|------|-----------|------|
| **SPI TFT** (e.g., ILI9341) | 4-wire SPI | BL=42, MOSI=44, CLK=43, DC=38, RST=1, CS=45 |
| **I²C OLED** (e.g., SSD1306) | I²C | SDA=GPIO43, SCL=GPIO44 |

### 📷 Camera
| Sensor | Interface |
|--------|-----------|
| OV2640 | Camera Parallel |
| OV3660 | Camera Parallel |
| OV5640 | Camera Parallel |

### 💾 Storage
| Type | Interface | Pins |
|------|-----------|------|
| **microSD Card** | SD_MMC (4-bit) | CMD=GPIO3, CLK=GPIO2, D0=GPIO46 |

### 🔌 Connectivity & Power
- **USB Type-C** — power, programming, and serial
- **Battery Charging Circuit** — Li-Po / Li-Ion support
- **3A Power Rail** — sufficient for cameras + audio + display
- **3.3V Regulator** — clean supply for all peripherals
- **RGB LED** — GPIO14 (WS2812B / NeoPixel compatible)
- **AI / Voice Assistant Ready** — dual-core 240 MHz + vector extensions

---

## 🧩 Hardware Overview

```
 ┌─────────────────────────────────────────────┐
 │             OceanLabz AI Explorer S3         │
 │  ┌──────────┐  ┌──────────┐  ┌──────────┐  │
 │  │  INMP441  │  │ MAX98357A│  │  Camera  │  │
 │  │  (Mic)    │  │ (Speaker)│  │  (OVx)   │  │
 │  └────┬─────┘  └────┬─────┘  └────┬─────┘  │
 │       │              │             │         │
 │  ┌────┴──────────────┴─────────────┴──────┐ │
 │  │          ESP32-S3 (Main SoC)           │ │
 │  │  Xtensa LX7 Dual-Core @ 240 MHz        │ │
 │  │  512KB SRAM · 16MB Flash · PSRAM       │ │
 │  └────────────────┬───────────────────────┘ │
 │       │            │            │            │
 │  ┌────┴────┐ ┌────┴────┐ ┌────┴────┐       │
 │  │  TFT /  │ │ microSD │ │  USB-C  │       │
 │  │  OLED   │ │ (SD_MMC)│ │ + Bat.  │       │
 │  └─────────┘ └─────────┘ └─────────┘       │
 └─────────────────────────────────────────────┘
```

> **Note:** The SPI TFT and I²C OLED share pins (MOSI/CLK with SDA/SCL). Only one display type should be connected at a time.

---

## 📍 Pin Mapping

### Microphone — INMP441 (I²S)
| Signal | GPIO Pin |
|--------|:--------:|
| WS (Word Select / LRCLK) | **39** |
| SCK (Serial Clock / BCLK) | **40** |
| SD (Serial Data / DOUT) | **41** |

### Speaker Amplifier — MAX98357A (I²S)
| Signal | GPIO Pin |
|--------|:--------:|
| LRCK (Left-Right Clock) | **21** |
| BCLK (Bit Clock) | **48** |
| DIN (Data In) | **47** |

### SD_MMC Card
| Signal | GPIO Pin |
|--------|:--------:|
| CMD | **3** |
| CLK | **2** |
| D0  | **46** |

### SPI TFT Display
| Signal | GPIO Pin |
|--------|:--------:|
| BL (Backlight) | **42** |
| MOSI (Master Out Slave In) | **44** |
| CLK (Serial Clock) | **43** |
| DC (Data / Command) | **38** |
| RST (Reset) | **1** |
| CS (Chip Select) | **45** |

### I²C OLED Display
| Signal | GPIO Pin |
|--------|:--------:|
| SDA (Data) | **43** |
| SCL (Clock) | **44** |

> **⚠️ Display Conflict:** GPIO43 and GPIO44 are shared between SPI TFT (CLK/MOSI) and I²C OLED (SCL/SDA). **Do not connect both displays at the same time.**

### RGB LED
| Signal | GPIO Pin |
|--------|:--------:|
| DIN (NeoPixel Data) | **14** |

### USB Type-C
| Signal | Notes |
|--------|-------|
| D+/D- | Connected directly to ESP32-S3 USB Serial/JTAG |
| VBUS | Powers the board + battery charger |
| ID | Ground for OTG host mode detection |

---

## 📦 Supported Displays

| Display | Interface | Resolution | Library |
|---------|-----------|:----------:|---------|
| **SSD1306** OLED 128×64 | I²C | 128×64 | `Adafruit SSD1306` |
| **SH1106** OLED 128×64 | I²C | 128×64 | `Adafruit SH1106` |
| **ILI9341** TFT 240×320 | SPI | 240×320 | `TFT_eSPI` |
| **ST7789** TFT 240×240 | SPI | 240×240 | `TFT_eSPI` |
| **ST7735** TFT 128×160 | SPI | 128×160 | `TFT_eSPI` |

> Refer to the [Display Guide](docs/guides/display/README.md) for wiring and configuration details.

---

## 📷 Supported Cameras

| Sensor | Resolution | Features |
|--------|:----------:|----------|
| **OV2640** | 2MP (1600×1200) | Best balance of cost, availability, and quality. Recommended for beginners. |
| **OV3660** | 3MP (2048×1536) | Higher resolution, slightly higher power consumption. |
| **OV5640** | 5MP (2592×1944) | Highest resolution. Requires careful PSRAM management. |

> 💡 **Start with the OV2640** — it is the most widely supported, well-documented, and easiest to get running.

> Refer to the [Camera Guide](docs/guides/camera/README.md) for wiring, configuration, and troubleshooting.

---

## 🚀 Getting Started

### 🔧 What You'll Need

| Item | Notes |
|------|-------|
| OceanLabz AI Explorer S3 Board | — |
| USB Type-C Cable | Data-capable (not charge-only) |
| Computer | Windows, macOS, or Linux |
| Arduino IDE | v2.x recommended ([download](https://www.arduino.cc/en/software)) |

### Step 1: Install Arduino IDE
Download and install the latest version from [arduino.cc](https://www.arduino.cc/en/software).
Version 2.x is recommended for its modern editor, serial monitor, and board manager.

### Step 2: Add ESP32 Board Support
1. Open Arduino IDE → **File** → **Preferences**
2. Add this URL to *Additional Boards Manager URLs*:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. Go to **Tools** → **Board** → **Boards Manager**
4. Search for `ESP32` and install **esp32 by Espressif Systems**
5. Select **AI Explorer S3** from the board list (or `ESP32S3 Dev Module`)

> See [Installing ESP32 Board Package](docs/getting-started/installing-esp32-arduino-core.md) for detailed instructions.

### Step 3: Install Required Libraries
Open the Library Manager (**Tools** → **Manage Libraries**) and install:

| Library | Purpose | Author |
|---------|---------|--------|
| `TFT_eSPI` | SPI TFT display driver | Bodmer |
| `Adafruit SSD1306` | I²C OLED driver | Adafruit |
| `Adafruit GFX` | Graphics primitives | Adafruit |
| `ESP32-audioI2S` | I²S audio playback | schreibfaul1 |
| `SD_MMC` | SD card access | Espressif (built-in) |

### Step 4: Connect Your Board
1. Plug the board into your computer via USB Type-C
2. Install drivers if needed (CP210x / CH340 — the ESP32-S3 uses native USB, so no extra driver is typically needed)
3. Select the correct **Port** in Arduino IDE (**Tools** → **Port**)

### Step 5: Upload Your First Sketch
1. Open **File** → **Examples** → **01.Basics** → **01_Blink**
2. Click **Upload** (→ button)
3. Watch the RGB LED blink!

> 🎉 **You're up and running!** Explore the [Examples](#-examples) section for more projects.

---

## 📚 Examples

All examples are organized by complexity and feature. Open them directly from the repository or copy the folders into your Arduino `sketchbook` directory.

### 🟢 Beginner
| # | Example | Description | Folder |
|---|---------|-------------|--------|
| 1 | **Blink** | Blink the onboard RGB LED | [01_blink](examples/01-basics/01_blink/) |
| 2 | **Serial Test** | Print sensor data over USB serial | [02_serial_test](examples/01-basics/02_serial_test/) |
| 3 | **Hello World** | Basic pin I/O and serial output | [03_hello_world](examples/01-basics/03_hello_world/) |

### 🟡 Intermediate
| # | Example | Description | Folder |
|---|---------|-------------|--------|
| 4 | **OLED Hello** | Display text on I²C OLED | [01_oled_hello](examples/02-display/01_oled_hello/) |
| 5 | **TFT Graphics** | Draw shapes, text, and images on SPI TFT | [02_tft_graphics](examples/02-display/02_tft_graphics/) |
| 6 | **SD Card Test** | Read/write files on microSD via SD_MMC | [01_sdmmc_card_test](examples/04-storage/01_sdmmc_card_test/) |
| 7 | **Mic Recorder** | Record audio from INMP441 to serial plotter | [01_mic_recorder](examples/03-audio/01_mic_recorder/) |
| 8 | **Speaker Test** | Play a tone through MAX98357A | [02_speaker_test](examples/03-audio/02_speaker_test/) |

### 🔴 Advanced
| # | Example | Description | Folder |
|---|---------|-------------|--------|
| 9 | **Camera Test** | Capture and display camera feed | [01_camera_test](examples/05-camera/01_camera_test/) |
| 10 | **Camera Streaming** | Stream camera over WiFi (web browser) | [02_camera_streaming](examples/05-camera/02_camera_streaming/) |
| 11 | **MP3 Player** | Play MP3 files from SD card via speaker | [01_mp3_player](examples/06-advanced/01_mp3_player/) |
| 12 | **Voice Recorder** | Record audio to SD card and play it back | [02_voice_recorder](examples/06-advanced/02_voice_recorder/) |
| 13 | **Voice Assistant** | Basic voice command recognition demo | [03_voice_assistant_demo](examples/03-audio/03_voice_assistant_demo/) |

> 🔗 Full details in the [Examples Index](examples/README.md).

---

## 📖 Documentation

| Guide | Description |
|-------|-------------|
| [📘 Getting Started](docs/getting-started/) | Board setup, driver installation, first upload |
| [📗 Installing ESP32 Board Package](docs/getting-started/installing-esp32-arduino-core.md) | Detailed ESP32 Arduino Core setup |
| [📗 Uploading First Sketch](docs/getting-started/uploading-first-sketch.md) | Step-by-step upload walkthrough |
| [🖥️ Display Guide](docs/guides/display/README.md) | TFT & OLED wiring, libraries, configuration |
| [🎤 Audio Guide](docs/guides/audio/README.md) | I²S microphone and speaker setup |
| [📷 Camera Guide](docs/guides/camera/README.md) | Camera selection, wiring, tuning |
| [💾 SD Card Guide](docs/guides/storage/README.md) | SD_MMC usage, formatting, troubleshooting |
| [📡 Communication Guide](docs/guides/communication/README.md) | WiFi, BLE, USB Serial |
| [🛠️ API Reference](docs/api/) | Pin definitions, helper functions, constants |
| [🤝 Contributing](docs/contributing/) | How to contribute to this repository |
| [❓ Troubleshooting](docs/troubleshooting/README.md) | Common issues and solutions |

---

## ❓ Troubleshooting

### Board Not Detected
- Ensure you are using a **data-capable** USB cable
- Try a different USB port (USB 2.0 preferred)
- Press the **Reset** button on the board
- Put the board into **Download Mode**: hold **BOOT** → press **RESET** → release **RESET** → release **BOOT**

### Upload Fails
- Select the correct **Port** and **Board** in Arduino IDE
- Ensure **ESP32 Arduino Core v2.0.14+** is installed
- Try lowering the upload speed to **115200** in **Tools** → **Upload Speed**

### Display Not Working
- Verify you are using the correct library for your display
- Check that GPIO43/44 are not shared between TFT and OLED (use only one)
- For TFT: configure `TFT_eSPI` with the `User_Setup.h` file provided in the example

### Audio Not Working
- Ensure the speaker is connected to the MAX98357A terminal block
- Check I²S pin configuration matches the pin mapping table above
- For microphone: verify WS/SCK/SD wiring — they are NOT 5V tolerant

### Camera Not Working
- Confirm the camera ribbon cable is fully inserted (contacts facing the board)
- Set the correct camera model in the sketch (`CAMERA_MODEL_AI_EXPLORER` or custom PIN mapping)
- Ensure PSRAM is enabled in **Tools** → **PSRAM** → **"OPI PSRAM"**

> 🔍 See the full [Troubleshooting Guide](docs/troubleshooting/README.md) for more solutions.

---

## 🌍 Community

| Resource | Link |
|----------|------|
| 📖 Documentation | [docs/](docs/) |
| 💬 Discussions | [GitHub Discussions](https://github.com/ocean-labz/OceanLabz-AI-Explorer-S3-Board/discussions) |
| 🐛 Issue Tracker | [GitHub Issues](https://github.com/ocean-labz/OceanLabz-AI-Explorer-S3-Board/issues) |
| 📦 Releases | [Releases Page](https://github.com/ocean-labz/OceanLabz-AI-Explorer-S3-Board/releases) |
| 🛠️ Tools | [tools/](tools/) |
| ▶️ Video Tutorials | [YouTube Playlist](https://youtube.com/@ocean-labz) (placeholder) |

---

## 🤝 Contributing

We welcome contributions from the community! Whether it's fixing a typo, improving documentation, adding an example, or reporting a bug — every contribution helps.

Please see our [Contributing Guide](docs/contributing/CONTRIBUTING.md) and [Code of Conduct](docs/contributing/CODE_OF_CONDUCT.md) to get started.

---

## 📄 License

This repository is licensed under the **MIT License**.
See the [LICENSE](LICENSE) file for full details.

*Hardware designs and PCB files are not included in this repository.*

---

<div align="center">
  <p>Made with 🌊 by <a href="https://github.com/ocean-labz">OceanLabz</a></p>
  <p>
    <a href="https://github.com/ocean-labz/OceanLabz-AI-Explorer-S3-Board">GitHub</a> •
    <a href="https://youtube.com/@ocean-labz">YouTube</a> •
    <a href="https://github.com/ocean-labz/OceanLabz-AI-Explorer-S3-Board/discussions">Forum</a>
  </p>
</div>
