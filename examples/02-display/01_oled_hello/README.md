# 01 — OLED Hello World

## Description

Display "OceanLabz AI Explorer S3" and a live uptime counter on an I²C OLED display (SSD1306 / SH1106). This is the first display example and teaches the basics of I²C communication and text rendering on a graphic OLED.

## Required Hardware

- OceanLabz AI Explorer S3 Board
- I²C OLED display (SSD1306 or SH1106, 128×64)
- Female-to-female jumper wires (4)

## Required Libraries

| Library | Version | Author | Install via Library Manager |
|---------|:-------:|--------|----------------------------|
| **Adafruit SSD1306** | ≥ 2.5.7 | Adafruit | `Adafruit SSD1306` |
| **Adafruit GFX** | ≥ 1.11.5 | Adafruit | `Adafruit GFX` |

## Wiring

| OLED Pin | AI Explorer S3 GPIO |
|----------|:-------------------:|
| SDA | **43** |
| SCL | **44** |
| VCC | **3.3V** |
| GND | **GND** |

> ⚠️ **Address:** Most OLED modules default to I²C address `0x3C`. If yours uses `0x3D`, change `OLED_ADDR` in the sketch.
>
> ⚠️ **Shared pins:** GPIO43 and GPIO44 are also used by the SPI TFT display. Do **not** connect both an OLED and a TFT at the same time.

## Steps

1. Open Arduino IDE **Library Manager** (**Tools → Manage Libraries**)
2. Install **Adafruit SSD1306** and **Adafruit GFX**
3. Connect the OLED as per the wiring table
4. Open [`01_oled_hello.ino`](01_oled_hello.ino)
5. Select **Board**: `ESP32S3 Dev Module`
6. Select **Port**: your board's COM port
7. Click **Upload**
8. Open **Serial Monitor** at **115200 baud** to see debug output

## Expected Output

### OLED Display

```
|----------------------|
| OceanLabz AI        |
| Explorer S3         |
|                     |
| Uptime: 5s          |
|----------------------|
```

The display refreshes every second with an updated counter.

### Serial Monitor

```
OLED Hello — starting...
OLED initialised OK
Ready.

Uptime: 1s
Uptime: 2s
Uptime: 3s
...
```

## Code Structure

```
01_oled_hello.ino
├── setup()              → Init I²C, init OLED, draw static splash, print status
│   ├── Wire.begin()     → I²C bus on pins 43 (SDA) / 44 (SCL)
│   ├── display.begin()  → Initialise SSD1306 driver
│   └── display.display()→ Show "OceanLabz / AI / Explorer S3" for 1.5 s
└── loop()               → Every 1 second:
    ├── display.clearDisplay()
    ├── display.setCursor() + display.print()   → Draw text
    └── display.display()                       → Flush to OLED
```

## Troubleshooting

| Problem | Solution |
|---------|----------|
| No display, "OLED initialisation FAILED" in Serial | Check wiring (SDA→43, SCL→44, VCC→3.3V, GND→GND) |
| No display but wiring looks correct | Try address `0x3D` instead of `0x3C` |
| Garbled text | Update **Adafruit SSD1306** and **Adafruit GFX** libraries |
| Dim display | Some OLEDs have a contrast register; adjust `display.setTextColor()` |
| Upload fails | Hold **BOOT** → press **RESET** → release **BOOT** (download mode) |
| No port appears | Try a different USB cable; install CP210x drivers if needed |

## Learning Path

After this sketch works, move on to:
- [`02_tft_graphics`](../02_tft_graphics/README.md) — graphics and colour on the SPI TFT display
- [`03_tft_camera_viewfinder`](../03_tft_camera_viewfinder/README.md) — live camera feed on the TFT
