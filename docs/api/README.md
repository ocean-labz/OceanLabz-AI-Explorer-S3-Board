# 🛠️ API Reference

This section documents the pin definitions, helper functions, and constants used across all examples and sketches for the OceanLabz AI Explorer S3 Board.

---

## 📍 Pin Definitions

The following pin assignments are fixed by the board design. Use these `#define` constants in your sketches for readability and portability.

### Pin Definition Header (`pins_ai_explorer_s3.h`)

A centralized header file is provided in [`tools/pin_configuration/`](../../tools/pin_configuration/) for easy inclusion in any project.

```cpp
// ========================
// OceanLabz AI Explorer S3
// Board Pin Definitions
// ========================

// --- Microphone (INMP441) ---
#define MIC_WS      39
#define MIC_SCK     40
#define MIC_SD      41

// --- Speaker Amplifier (MAX98357A) ---
#define SPEAKER_LRCK  21
#define SPEAKER_BCLK  48
#define SPEAKER_DIN   47

// --- SD_MMC ---
#define SD_CMD     3
#define SD_CLK     2
#define SD_D0      46

// --- SPI TFT Display ---
#define TFT_BL     42
#define TFT_MOSI   44
#define TFT_CLK    43
#define TFT_DC     38
#define TFT_RST    1
#define TFT_CS     45

// --- I2C OLED Display ---
#define OLED_SDA   43
#define OLED_SCL   44

// --- RGB LED (WS2812B / NeoPixel) ---
#define RGB_LED    14

// --- Camera (OV2640 / OV3660 / OV5640) ---
#define CAM_PWDN   -1
#define CAM_RESET  -1
#define CAM_XCLK   15
#define CAM_SIOD   4
#define CAM_SIOC   5
#define CAM_Y9     6
#define CAM_Y8     7
#define CAM_Y7     8
#define CAM_Y6     9
#define CAM_Y5     10
#define CAM_Y4     11
#define CAM_Y3     12
#define CAM_Y2     13
#define CAM_VSYNC  14
#define CAM_HREF   16
#define CAM_PCLK   17
```

---

## 📦 Helper Functions

Common utility functions used across examples (to be documented as firmware is developed):

| Function | Description | Inputs | Outputs |
|----------|-------------|--------|---------|
| `initRGB()` | Initialize NeoPixel RGB LED | None | None |
| `setRGB(r, g, b)` | Set RGB LED color | r,g,b (0-255) | None |
| `initI2S_Mic()` | Initialize I²S for microphone | Sample rate, bits | `i2s_port_t` |
| `initI2S_Speaker()` | Initialize I²S for speaker | Sample rate, bits | `i2s_port_t` |
| `initSDMMC()` | Initialize SD_MMC in 1-bit mode | None | `bool` |
| `initTFT()` | Initialize SPI TFT display | None | `bool` |
| `initOLED()` | Initialize I²C OLED display | Address | `bool` |
| `initCamera()` | Initialize camera module | Model enum | `bool` |

---

## 📟 Constants

```cpp
// Display configuration
#define TFT_WIDTH   240
#define TFT_HEIGHT  320
#define OLED_WIDTH  128
#define OLED_HEIGHT 64

// Audio configuration
#define MIC_SAMPLE_RATE    16000
#define SPEAKER_SAMPLE_RATE 44100
#define AUDIO_BITS         16

// SD card
#define SDMMC_1BIT_MODE true

// Camera frame sizes
#define CAM_FRAMESIZE_QVGA   FRAMESIZE_QVGA    // 320x240
#define CAM_FRAMESIZE_VGA    FRAMESIZE_VGA     // 640x480
#define CAM_FRAMESIZE_SVGA   FRAMESIZE_SVGA    // 800x600
#define CAM_FRAMESIZE_UXGA   FRAMESIZE_UXGA    // 1600x1200
```

---

## 🔌 Library Compatibility

| Peripheral | Recommended Library | API Compatibility |
|------------|-------------------|-------------------|
| OLED (I²C) | `Adafruit_SSD1306` | `display.begin()`, `display.display()` |
| TFT (SPI) | `TFT_eSPI` | `tft.begin()`, `tft.fillScreen()`, `tft.drawPixel()` |
| Camera | `esp_camera` | `esp_camera_init()`, `esp_camera_fb_get()` |
| SD Card | `SD_MMC` | `SD_MMC.begin()`, `SD_MMC.open()` |
| RGB LED | `Adafruit_NeoPixel` | `strip.begin()`, `strip.show()` |
| I²S Audio | `esp32-hal-i2s` | `i2s_install()`, `i2s_read()`, `i2s_write()` |

---

## 📁 File Locations

| Resource | Path |
|----------|------|
| Pin Definition Header | [`tools/pin_configuration/pins_ai_explorer_s3.h`](../../tools/pin_configuration/pins_ai_explorer_s3.h) |
| Board Test Sketch | [`tools/board_test/`](../../tools/board_test/) |
| Flashing Helpers | [`tools/flashing_helpers/`](../../tools/flashing_helpers/) |