# AI Explorer S3 — Board Variant Definition

**Board:** OceanLabz AI Explorer S3
**SoC:** ESP32-S3-WROOM-1-N16R8
**Form Factor:** Custom PCB v1.0

This directory contains board variant files for the **AI Explorer S3** base model. These files are used by the Arduino core and PlatformIO to correctly configure pin mappings, flash size, PSRAM, and partition schemes.

## Contents

| File | Purpose |
|------|---------|
| `pins_arduino.h` | Arduino core pin mapping (ESP32-S3 variant) |
| `board.txt` | Arduino board definition |
| `boards.txt` | (if applicable) PlatformIO board alias |

## Pin Map Reference

| Peripheral | Pins | Notes |
|------------|------|-------|
| **RGB LED** | GPIO14 (WS2812B) | Shared with Camera VSYNC |
| **INMP441 Mic** | WS=39, SCK=40, SD=41 | I²S Port 0 |
| **MAX98357A Amp** | LRCK=21, BCLK=48, DIN=47 | I²S Port 1 |
| **SD_MMC** | CMD=3, CLK=2, D0=46 | 1-bit mode only |
| **TFT Display** | BL=42, MOSI=44, CLK=43, DC=38, RST=1, CS=45 | SPI |
| **OLED Display** | SDA=43, SCL=44 | I²C (shares pins with TFT) |
| **Camera** | Full parallel interface | PSRAM required |

## Specs

- **Flash:** 16 MB Quad SPI
- **PSRAM:** 8 MB Octal SPI (OPI)
- **USB:** Native USB + USB-Serial-JTAG (UART0)
- **Power:** 5V via USB-C or VIN (3.3V LDO regulator)
- **Dimensions:** 60 mm × 40 mm

> **Note:** For variant files targeting the **AI Explorer S3 Pro**, see [`variants/ai-explorer-s3-pro/`](../ai-explorer-s3-pro/README.md).
