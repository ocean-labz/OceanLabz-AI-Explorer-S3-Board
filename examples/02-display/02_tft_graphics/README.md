# 02 — TFT Graphics

## Description

Draw colourful shapes, text at multiple sizes, geometric patterns, gradients, and a checkerboard on a 1.8" SPI TFT display. This example introduces the **Adafruit_ST7735** and **Adafruit_GFX** libraries with the most common graphics primitives you'll need for any display project.

The sketch cycles through **5 demo screens** automatically, changing every 3 seconds:

| # | Demo | What it shows |
|:-:|------|--------------|
| 1 | **Colour Bars** | 8 vertical colour bars with centred overlay text |
| 2 | **Shapes** | Rounded rectangles, circles, triangles, filled/outlined rects |
| 3 | **Text Demo** | Font sizes 1–3, centred text, multi-colour and inverse text |
| 4 | **Lines & Gradient** | Colour-ramped horizontal lines, cross-hair, diagonals |
| 5 | **Checkerboard** | 8×6 grid of alternating blue/yellow cells |

## Required Hardware

- OceanLabz AI Explorer S3 Board
- **1.8" SPI TFT display (ST7735, 128×160)** — blue-tab variant
- Female-to-female jumper wires (8)

## Required Libraries

| Library | Version | Author | Install via Library Manager |
|---------|:-------:|--------|----------------------------|
| **Adafruit ST7735** | ≥ 1.10.0 | Adafruit | `Adafruit ST7735` |
| **Adafruit GFX** | ≥ 1.11.0 | Adafruit | `Adafruit GFX` |

> Both libraries are installed together via Library Manager when you search for `Adafruit ST7735`.

## Wiring

| TFT Pin | AI Explorer S3 GPIO |
|---------|:-------------------:|
| BL (Backlight) | **42** |
| MOSI | **43** |
| CLK | **44** |
| DC | **38** |
| RST | **1** |
| CS | **45** |
| VCC | **3.3V** |
| GND | **GND** |

> ⚠️ **Shared pins:** GPIO43 and GPIO44 are also used by the I²C OLED. Do **not** connect both an OLED and a TFT at the same time.

> ⚠️ **MISO pin:** The 1.8" ST7735 display does **not** have a MISO pin. The sketch sets `TFT_MISO = -1` to disable it on the SPI bus.

### ⚡ Pin note

The AI Explorer S3's reference pin header labels TFT-MOSI as **GPIO44** and TFT-CLK as **GPIO43** — but the **actual working pins on this board are reversed**: MOSI = **GPIO43**, CLK = **GPIO44**. The sketch uses the correct working values.

### ⚡ Self-contained sketch

All pin mappings, the SPI bus instance, and the display object are defined **directly in the sketch** — no external `User_Setup.h` or library configuration files needed. Just install Adafruit_ST7735, open the sketch, and upload.

### ⚡ FSPI bus

The sketch creates a dedicated `SPIClass tftSPI(FSPI)` instance on the default **FSPI (SPI2)** peripheral. On the ESP32-S3 this works correctly with the Adafruit_ST7735 library.

## Steps

1. Install **Adafruit ST7735** (and its dependency **Adafruit GFX**) via Library Manager
2. Connect the TFT display as per the wiring table
3. Open [`02_tft_graphics.ino`](02_tft_graphics.ino)
4. Select **Board**: `ESP32S3 Dev Module`
5. Select **Port**: your board's COM port
6. Click **Upload**
7. The TFT will immediately start cycling through demos

## Expected Output

The TFT display cycles through 5 screens, each shown for 3 seconds:

1. **Colour Bars** — 8 vertical stripes (red, green, blue, yellow, cyan, magenta, orange, white) with "Colour Bars" centred overlay
2. **Shapes** — blue rounded rect border, red filled rect, green outlined rect, yellow circle, cyan triangle, magenta label
3. **Text Demo** — "Size 1: ABC abc 123", "Size 2", "S3" in size 3, centred board name, "Red on Yellow", "White on Blue"
4. **Lines & Gradient** — colour gradient from top to bottom, white cross-hair, yellow diagonals
5. **Checkerboard** — 8×6 grid of alternating blue and yellow squares with a red border

Serial Monitor (115200 baud):
```
TFT Graphics — starting...
Screen size: 160 x 128
Ready. Cycling through graphics...
```

## Code Structure

```
02_tft_graphics.ino
├── #include <Adafruit_GFX.h>         → Adafruit graphics core
├── #include <Adafruit_ST7735.h>       → ST7735 driver
├── #define TFT_CS 45, TFT_DC 38, ... → Pin mappings
├── SPIClass tftSPI(FSPI)             → FSPI bus instance
├── Adafruit_ST7735 tft(...)           → Display object
├── drawCentred()                      → Helper: centred text via getTextBounds()
├── setup()                            → Backlight on → SPI init → initR → splash
│   ├── digitalWrite(TFT_BL, HIGH)
│   ├── tftSPI.begin(SCLK, MISO, MOSI, CS)
│   ├── tft.initR(INITR_BLACKTAB)
│   └── tft.setRotation(1)
└── loop()                             → Cycle step 0..4 every 3 s
    ├── drawColourBars()               → 8 vertical colour stripes
    ├── drawShapes()                   → Rects, circle, triangle, rounded rect
    ├── drawTextDemo()                 → Font sizes 1–3, colours, centring
    ├── drawLinesAndGradient()         → Gradient lines, cross-hair, diagonals
    └── drawCheckerboard()             → 8×6 alternating grid
```

## Troubleshooting

| Problem | Solution |
|---------|----------|
| White screen (backlight on) | Wrong driver — Adafruit_ST7735 only works with ST7735 displays |
| Backlight stays off | Check BL pin wired to GPIO42 |
| Wrong / inverted colours | Try a different init variant: `INITR_BLACKTAB` ↔ `INITR_GREENTAB` ↔ `INITR_144GREENTAB` ↔ `INITR_MINI160x80` |
| Blank display (backlight on) | Try a different `initR()` variant (see above) |
| Flickering display | Check CS=45 is correct; no other peripheral using same SPI pins |
| No image / scrambled | Reduce SPI speed by lowering clock (not yet configurable in this sketch) |
| Compile error: "SPI.h not found" | Install **Adafruit ST7735** library — it pulls in SPI as a dependency |
| StoreProhibited crash | **Switch to Adafruit_ST7735** — this sketch uses it instead of TFT_eSPI |
| Upload fails | Hold **BOOT** → press **RESET** → release **BOOT** (download mode) |
| No port appears | Try a different USB cable; install CP210x drivers if needed |

## Learning Path

After this sketch works, move on to:
- [`03_tft_camera_viewfinder`](../03_tft_camera_viewfinder/README.md) — live camera feed on the TFT display
- [`01_mic_recorder`](../../03-audio/01_mic_recorder/README.md) — capture audio from the INMP441 microphone
