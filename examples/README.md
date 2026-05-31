# 📚 Examples Index

All examples are organized by complexity (Beginner → Advanced) and by feature (Display, Audio, Storage, Camera).

---

## How to Use

1. **Copy** the entire example folder to your Arduino sketchbook folder:
   ```
   Documents/Arduino/ (Windows)
   ~/Arduino/ (macOS/Linux)
   ```
2. Open the `.ino` file from **File** → **Open** in Arduino IDE
3. Install any required libraries (listed in each example's `libraries.txt`)
4. Select **Board**: `ESP32S3 Dev Module`
5. Select **Port**: your board's COM port
6. Click **Upload**

---

## 🟢 Beginner Examples

| # | Example | Folder | Description | Est. Time |
|---|---------|--------|-------------|:---------:|
| 1 | [Blink](01-basics/01_blink/) | `01-basics/01_blink/` | Blink the onboard RGB LED | 5 min |
| 2 | [Serial Test](01-basics/02_serial_test/) | `01-basics/02_serial_test/` | Print data over USB serial | 5 min |
| 3 | [Hello World](01-basics/03_hello_world/) | `01-basics/03_hello_world/` | Basic pin I/O and serial output | 10 min |

**Learning Goals:**
- Install and configure Arduino IDE for ESP32-S3
- Upload a sketch to the board
- Read serial output
- Understand basic pin I/O

---

## 🟡 Intermediate Examples

### Display
| # | Example | Folder | Description |
|---|---------|--------|-------------|
| 4 | [OLED Hello](02-display/01_oled_hello/) | `02-display/01_oled_hello/` | Display text on I²C OLED |
| 5 | [TFT Graphics](02-display/02_tft_graphics/) | `02-display/02_tft_graphics/` | Shapes, text, and images on SPI TFT |
| 6 | [TFT Camera Viewfinder](02-display/03_tft_camera_viewfinder/) | `02-display/03_tft_camera_viewfinder/` | Live camera feed on TFT |

### Audio
| # | Example | Folder | Description |
|---|---------|--------|-------------|
| 7 | [Mic Recorder](03-audio/01_mic_recorder/) | `03-audio/01_mic_recorder/` | Record audio from INMP441 |
| 8 | [Speaker Test](03-audio/02_speaker_test/) | `03-audio/02_speaker_test/` | Play a tone through MAX98357A |

### Storage
| # | Example | Folder | Description |
|---|---------|--------|-------------|
| 9 | [SD Card Test](04-storage/01_sdmmc_card_test/) | `04-storage/01_sdmmc_card_test/` | Initialize and test microSD |
| 10 | [SD Card Logger](04-storage/02_sdmmc_logger/) | `04-storage/02_sdmmc_logger/` | Log data to CSV on SD card |

**Learning Goals:**
- Interface with I²C and SPI displays
- Capture and play audio via I²S
- Read/write files on SD card via SD_MMC
- Combine multiple peripherals

---

## 🔴 Advanced Examples

| # | Example | Folder | Description |
|---|---------|--------|-------------|
| 11 | [Camera Test](05-camera/01_camera_test/) | `05-camera/01_camera_test/` | Initialize and capture from OV2640 |
| 12 | [Camera Streaming](05-camera/02_camera_streaming/) | `05-camera/02_camera_streaming/` | Stream camera over WiFi |
| 13 | [Camera + SD Capture](05-camera/03_camera_sd_capture/) | `05-camera/03_camera_sd_capture/` | Capture photos to SD card |
| 14 | [MP3 Player](06-advanced/01_mp3_player/) | `06-advanced/01_mp3_player/` | Play MP3 from SD via speaker |
| 15 | [Voice Recorder](06-advanced/02_voice_recorder/) | `06-advanced/02_voice_recorder/` | Record mic to SD, play back |
| 16 | [WiFi Audio Streamer](06-advanced/03_wifi_audio_streamer/) | `06-advanced/03_wifi_audio_streamer/` | Stream audio over WiFi |
| 17 | [Voice Assistant Demo](03-audio/03_voice_assistant_demo/) | `03-audio/03_voice_assistant_demo/` | Voice command recognition |

**Learning Goals:**
- Camera initialization and image capture
- WiFi streaming and web server setup
- Audio file playback and recording
- Multi-peripheral integration

---

## Recommended Learning Path

```
Start Here → Blink → Serial Test → Hello World
                                         │
                    ┌────────────────────┼────────────────────┐
                    ▼                    ▼                    ▼
              OLED Hello           SD Card Test          Mic Recorder
                    │                    │                    │
                    ▼                    ▼                    ▼
            TFT Graphics          SD Card Logger         Speaker Test
                    │                                        │
                    ▼                                        ▼
       TFT Camera Viewfinder                    MP3 Player / Voice Recorder
                    │
                    ▼
             Camera Test
                    │
                    ▼
          Camera Streaming
```

---

## Library Requirements Summary

| Library | Used By | Install Via |
|---------|---------|-------------|
| `Adafruit SSD1306` | OLED examples | Library Manager |
| `Adafruit GFX` | OLED + TFT examples | Library Manager |
| `TFT_eSPI` | TFT examples | Library Manager |
| `ESP32-audioI2S` | MP3 Player, Voice Recorder | Library Manager |
| `WiFi.h` | Camera Streaming, WiFi Audio | Built-in (ESP32 Core) |
| `SD_MMC.h` | All SD card examples | Built-in (ESP32 Core) |
| `esp_camera.h` | All camera examples | Built-in (ESP32 Core) |

---

## Contributing Examples

Want to add an example? See the [Example Guidelines](../docs/contributing/CONTRIBUTING.md#example-contribution-guidelines) and [Documentation Standards](../docs/contributing/documentation-standards.md).