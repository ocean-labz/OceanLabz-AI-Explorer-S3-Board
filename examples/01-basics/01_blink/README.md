# 01 — Blink (RGB LED)

## Description

The **"Hello World"** of embedded programming — and the **very first example** every user should run after receiving their OceanLabz AI Explorer S3 Board.

This sketch drives the onboard **WS2812B RGB LED** (GPIO14) through **7 different light effects**, selectable by changing a single variable at the top of the sketch.

### Effects

| # | Name     | Description |
|---|----------|-------------|
| 0 | Normal   | 1 s on / 1 s off (classic blink) |
| 1 | Fast     | 200 ms on / 200 ms off |
| 2 | Heartbeat | Double-pulse rhythm ♥ |
| 3 | SOS      | International Morse distress signal (`... --- ...`) |
| 4 | Fade     | Smooth HSV colour hue sweep over ~6.5 s |
| 5 | Random   | Random colour + random interval |
| 6 | **DEMO** | Auto-cycles through effects 0–5 every 10 s |

## Required Hardware

- OceanLabz AI Explorer S3 Board
- USB Type-C cable (data-capable, not charge-only)

## Required Libraries

| Library | Version | Install via |
|---------|---------|-------------|
| `Adafruit NeoPixel` | ≥ 1.12 | Arduino Library Manager |

## Wiring

No external wiring required. The RGB LED (WS2812B) is permanently connected to **GPIO14** on the board.

## Steps

1. Connect the board to your computer via USB
2. Launch **Arduino IDE**
3. Install the **Adafruit NeoPixel** library: *Tools → Manage Libraries → search "Adafruit NeoPixel" → Install*
4. Open the sketch: *File → Open → `01_blink.ino`*
5. Select **Board**: `ESP32S3 Dev Module`
6. Configure **Tools** menu:
   - **Upload Speed**: `921600`
   - **USB CDC On Boot**: `Enabled`
   - **Flash Size**: `16MB (128Mb)`
   - **Partition Scheme**: `16M Flash (3M APP/9.9MB FATFS)`
   - **PSRAM**: `OPI PSRAM`
7. Select **Port**: your board's COM port
8. Click **Upload**

> ⚠️ **First time?** If the upload fails, hold **BOOT** → press **RESET** → release **BOOT** to enter download mode.

## Changing the Effect

At the top of [`01_blink.ino`](01_blink.ino), find this line:

```cpp
static uint8_t currentEffect = 6;          // <<< CHANGE ME (0-6)
```

Change the number (0–6) and re-upload. The DEMO mode (6) is the default so you can see all effects without repeated uploads.

## Expected Output

- The RGB LED lights up with the selected effect
- **Serial Monitor** (115200 baud) prints status messages for each effect:

| Effect  | Serial Output |
|---------|---------------|
| Normal  | `ON` / `OFF` every second |
| Fast    | `fast blink` |
| Heartbeat | `♥` |
| SOS     | `SOS` |
| Fade    | `fade cycle complete` once per revolution |
| Random  | *(no regular message — random timing)* |
| DEMO    | `>>> DEMO: starting effect ...` / `>>> DEMO: switching to effect ...` |

## Troubleshooting

| Problem | Solution |
|---------|----------|
| LED stays dark | Check **Tools → Port** selection; verify USB cable is data-capable |
| Upload fails | Enter **download mode**: hold BOOT → press RESET → release BOOT |
| `Adafruit_NeoPixel.h: No such file` | Install the library via **Tools → Manage Libraries** |
| Wrong colours | WS2812B uses **GRB** order; this sketch uses `NEO_GRB` which is correct |
| LED too bright | Reduce `strip.setBrightness(50)` in `setup()` — range is 0–255 |
| Serial Monitor shows garbage | Set baud rate to **115200** in the Serial Monitor |

## Learning Path

After this sketch works, move on to:
- [`02_serial_test`](../../01-basics/02_serial_test/README.md) — print board info to the Serial Monitor
- [`03_hello_world`](../../01-basics/03_hello_world/README.md) — interact with GPIO pins and serial input

## Code Structure

```
01_blink.ino
├── setup()              → Initialise NeoPixel, print banner
├── loop()               → Dispatch to active effect
├── effectNormalBlink()  →  1 s on / 1 s off
├── effectFastBlink()    →  200 ms on / 200 ms off
├── effectHeartbeat()    →  Double-pulse rhythm
├── effectSos()          →  ... --- ...
├── effectFade()         →  HSV colour cycling
├── effectRandom()       →  Random colour + random timing
├── effectDemo()         →  Auto-cycle through 0–5
├── setLed()             →  Write colour to NeoPixel
├── dot() / dash()       →  Morse code primitives
├── fadeTo()             →  Smooth brightness ramp
└── printEffectName()    →  Serial debug helper
