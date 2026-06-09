# 🖥️ Display Guide — OLED & TFT

This guide covers setting up both display options for the OceanLabz AI Explorer S3 Board.

---

## Display Options

| Type | Interface | Resolution | Library |
|------|-----------|:----------:|---------|
| **I²C OLED** (SSD1306/SH1106) | I²C | 128×64 | Adafruit SSD1306 |
| **SPI TFT** (ST7735) | SPI | 128×160 | Adafruit ST7735 + Adafruit GFX |

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
| MOSI | **43** |
| CLK | **44** |
| DC | **38** |
| RST | **1** |
| CS | **45** |
| VCC | 3.3V |
| GND | GND |

> **Pin note:** The reference pin header labels TFT-MOSI as GPIO44 and TFT-CLK as GPIO43, but the **working pins on this board are reversed**. Use MOSI=43, CLK=44 as shown above.

### Required Libraries
Install via Library Manager:
- **Adafruit ST7735** by Adafruit (v1.10.0+) — includes all dependencies
- **Adafruit GFX** by Adafruit (v1.11.0+) — automatically installed as a dependency

### ⚡ Why Adafruit_ST7735 (not TFT_eSPI)?

The TFT_eSPI library suffers from a `StoreProhibited` crash on certain ESP32-S3 boards, including the AI Explorer S3. The **Adafruit_ST7735** library works reliably with this hardware and provides the same essential graphics primitives via the widely-used **Adafruit_GFX** API.

### Configuration

The [`TFT Graphics`](../../examples/02-display/02_tft_graphics/) example is **fully self-contained** — all pin mappings, the SPI bus instance, and the display object are defined directly in the `.ino` file.

**No library configuration files are required.** Just install Adafruit_ST7735 and upload the sketch.

The sketch uses:
- **`tft.initR(INITR_BLACKTAB)`** — correct init sequence for the 1.8" blue-tab display
- **`SPIClass tftSPI(FSPI)`** — FSPI (SPI2) bus, the standard ESP32-S3 SPI peripheral
- **`tft.setRotation(1)`** — landscape orientation (USB port on the left)

If your display is a different variant, change the init parameter:

```cpp
tft.initR(INITR_BLACKTAB);       // Blue tab 1.8" 128×160  (default)
// tft.initR(INITR_GREENTAB);     // Green tab 1.8" 128×160
// tft.initR(INITR_144GREENTAB);  // Green tab 1.44" 128×128
// tft.initR(INITR_MINI160x80);   // Mini 160×80
```

> 💡 **Init variants:** ST7735 displays have different initialisation sequences depending on the PCB variant. If your display shows a blank screen or garbled content, try the other init variants listed above.

### Power-up sequence

The sketch enables the backlight **before** calling `tft.initR()`:

```cpp
pinMode(TFT_BL, OUTPUT);
digitalWrite(TFT_BL, HIGH);
delay(100);                // settling time for the display controller
tftSPI.begin(TFT_SCLK, TFT_MISO, TFT_MOSI, TFT_CS);
tft.initR(INITR_BLACKTAB);
```

### Example
➡️ [TFT Graphics](../../examples/02-display/02_tft_graphics/)

---

## Troubleshooting

| Problem | Likely Cause | Solution |
|---------|--------------|----------|
| OLED: no display | Wrong I²C address | Run I²C scanner sketch; try 0x3C and 0x3D |
| OLED: garbled text | Wrong library version | Update Adafruit SSD1306 and GFX |
| TFT: white screen | Wrong library | This board requires **Adafruit_ST7735**, not TFT_eSPI |
| TFT: backlight but no image | Wrong init variant | Try `INITR_BLACKTAB` ↔ `INITR_GREENTAB` ↔ `INITR_144GREENTAB` ↔ `INITR_MINI160x80` |
| TFT: backlight but no image | CS/RST pin mismatch | Check Adafruit_ST7735 pin definitions match wiring |
| TFT: wrong colours | Init variant | Change the `initR()` parameter to a different tab variant |
| TFT: StoreProhibited crash | TFT_eSPI library | **Switch to Adafruit_ST7735** — TFT_eSPI crashes on this board |
| TFT: display is dim | Backlight resistor | The BL pin drives the backlight directly; brightness is fixed |
| Both: display flickers | Power supply issue | Ensure 3.3V supply is stable; disconnect camera if attached |
| Both: no display at all | GPIO conflict | Are both OLED and TFT connected? Remove one. |
| Both: upload fails | Board not in download mode | Hold **BOOT** → tap **RESET** → release **BOOT** |
