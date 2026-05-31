# 🖥️ Display Guide — OLED & TFT

This guide covers setting up both display options for the OceanLabz AI Explorer S3 Board.

---

## Display Options

| Type | Interface | Resolution | Library |
|------|-----------|:----------:|---------|
| **I²C OLED** (SSD1306/SH1106) | I²C | 128×64 | Adafruit SSD1306 |
| **SPI TFT** (ILI9341/ST7789/ST7735) | SPI | Up to 320×240 | TFT_eSPI |

> **⚠️ Only one display type can be used at a time.** GPIO43 and GPIO44 are shared between SPI (CLK/MOSI) and I²C (SCL/SDA).

---

## OLED (I²C)

### Wiring
| OLED Pin | GPIO |
|----------|:----:|
| SDA | **43** |
| SCL | **44** |
| VCC | 3.3V |
| GND | GND |

> Most OLED modules also have an address select pin (ADDR). Default is 0x3C. Set to 0x3D by connecting to VCC.

### Required Library
Install via Library Manager:
- **Adafruit SSD1306** by Adafruit (v2.5.7+)
- **Adafruit GFX** by Adafruit (v1.11.5+)

### Example
➡️ [OLED Hello World](../../examples/02-display/01_oled_hello/)

---

## TFT (SPI)

### Wiring
| TFT Pin | GPIO |
|---------|:----:|
| BL (Backlight) | **42** |
| MOSI | **44** |
| CLK | **43** |
| DC | **38** |
| RST | **1** |
| CS | **45** |
| VCC | 3.3V |
| GND | GND |

### Required Library
Install via Library Manager:
- **TFT_eSPI** by Bodmer (v2.5.0+)

### Configuration
TFT_eSPI requires a `User_Setup.h` file. Copy the one from the example folder to your TFT_eSPI library folder:

```
Arduino/libraries/TFT_eSPI/User_Setup.h
```

The setup file must define:
```cpp
#define ILI9341_DRIVER       // or ST7789, ST7735
#define TFT_CS   45
#define TFT_DC   38
#define TFT_RST  1
#define TFT_MOSI 44
#define TFT_SCLK 43
#define TFT_BL   42
```

### Example
➡️ [TFT Graphics](../../examples/02-display/02_tft_graphics/)

---

## Troubleshooting

| Problem | Likely Cause | Solution |
|---------|--------------|----------|
| OLED: no display | Wrong I²C address | Run I²C scanner sketch; try 0x3C and 0x3D |
| OLED: garbled text | Wrong library version | Update Adafruit SSD1306 and GFX |
| TFT: white screen | No config or wrong driver | Verify User_Setup.h driver selection |
| TFT: backlight but no image | CS/RST pin mismatch | Check TFT_eSPI pin definitions match wiring |
| Both: display flickers | Power supply issue | Ensure 3.3V supply is stable; disconnect camera if attached |
| Both: no display at all | GPIO conflict | Are both OLED and TFT connected? Remove one. |