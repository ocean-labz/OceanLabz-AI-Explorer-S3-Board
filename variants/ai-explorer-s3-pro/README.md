# AI Explorer S3 Pro — Board Variant Definition

**Board:** OceanLabz AI Explorer S3 Pro
**SoC:** ESP32-S3-WROOM-1-N16R8
**Form Factor:** Custom PCB v1.1 (Pro)

> **Heads up:** This variant is in the **planning / prototyping** stage. Pin assignments and features are subject to change. Check back or open an issue for the latest status.

## Planned Improvements over the Base Model

| Feature | Base (AI Explorer S3) | Pro (AI Explorer S3 Pro) |
|---------|----------------------|--------------------------|
| **SD_MMC** | 1-bit mode (D0 only) | 4-bit mode (D0–D3) |
| **Audio Codec** | INMP441 + MAX98357A | I²S Audio Codec (e.g., ES8311) with headphone out |
| **Onboard Mic** | 1 × INMP441 | 2 × INMP441 (stereo) |
| **Speaker** | External screw terminal | 3W built-in speaker + headphone jack |
| **TFT Display** | SPI 1.69″ 240×280 | SPI 2.0″ 320×480 or 1.69″ |
| **Battery** | No charger | IP5306 with 18650 Li-Ion |
| **IMU** | None | QMI8658 6-axis IMU |
| **Camera** | FPC-24P connector | Same with improved routing |
| **GPIO Expansion** | Minimal | Additional breakout pads |
| **USB** | USB-C (native + UART) | USB-C with FTDI for better compatibility |

## Variant Files (Pro)

| File | Purpose |
|------|---------|
| `pins_arduino.h` | Arduino core pin mapping |
| `board.txt` | Arduino board definition |

## Status

- [ ] Schematic review
- [ ] PCB layout
- [ ] Prototype assembly
- [ ] Firmware validation
- [ ] Documentation

> For the base model variant files, see [`variants/ai-explorer-s3/`](../ai-explorer-s3/README.md).
