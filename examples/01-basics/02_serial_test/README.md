# 02 — Serial Test

## Description

The **second example** every user should run. This sketch prints **complete system information** over the USB serial connection and provides a **command-line interface** for exploring every facet of the board.

Type commands into the **Serial Monitor** to:
- View CPU, flash, and PSRAM details
- Test PSRAM read/write speed
- List flash partition layout
- Show WiFi MAC addresses
- Control the onboard RGB LED
- Run a peripheral sanity check
- Soft-restart the board

## Required Hardware

- OceanLabz AI Explorer S3 Board
- USB Type-C cable (data-capable)

## Required Libraries

| Library | Version | Install via |
|---------|---------|-------------|
| `Adafruit NeoPixel` | ≥ 1.12 | Arduino Library Manager |

## Wiring

No external wiring required.

## Steps

1. Open [`02_serial_test.ino`](02_serial_test.ino) in Arduino IDE
2. Select **Board**: `ESP32S3 Dev Module`
3. Configure **Tools** menu:
   - **Upload Speed**: `921600`
   - **USB CDC On Boot**: `Enabled`
   - **Flash Size**: `16MB (128Mb)`
   - **Partition Scheme**: `16M Flash (3M APP/9.9MB FATFS)`
   - **PSRAM**: `OPI PSRAM`
4. Select **Port**: your board's COM port
5. Click **Upload**
6. Open **Serial Monitor** (**Tools → Serial Monitor**) at **115200 baud**

> ⚠️ **First time?** If the upload fails, hold **BOOT** → press **RESET** → release **BOOT** to enter download mode.

## Available Commands

| Command      | Description |
|-------------|-------------|
| `help`      | Print the command menu |
| `info`      | Print full system information |
| `psram`     | Test PSRAM read/write with speed benchmark |
| `flash`     | Show flash chip model, size, and partition table |
| `wifi`      | Print WiFi STA and SoftAP MAC addresses |
| `led on`    | Turn the RGB LED green |
| `led off`   | Turn the RGB LED off |
| `blink`     | Blink the RGB LED 5 times |
| `heap`      | Show free heap, free PSRAM, and minimum free heap |
| `restart`   | Soft-reset the board (LED turns red first) |
| `test`      | Run a 5-step peripheral sanity check |

## Expected Output

When you first open the Serial Monitor you'll see:

```
╔══════════════════════════════════════════════════════╗
║         🌊 OceanLabz AI Explorer S3 Board            ║
║              02 — Serial Test                       ║
╠══════════════════════════════════════════════════════╣
║  Firmware:  Jan 31 2024              ║
║  Compiler:  GCC 5.2.0                ║
║  SDK:       v5.1.4                   ║
╚══════════════════════════════════════════════════════╝

── System Information ──
  Chip Model:         ESP32-S3 rev.0
  Cores:              2
  CPU Freq:           240 MHz
  Features:           WiFi BLE
  Flash Size:         16 MB
  PSRAM Size:         8 MB
  PSRAM Free:         7.9 MB
  Free Heap:          287456 bytes
  Base MAC:           84:F7:03:XX:XX:XX
  Internal Temp:      32.5 °C
  Reset Reason:       POWER-ON / POR

Type 'help' for available commands.
>
```

An auto-status line is printed every 10 seconds:

```
──────────────────────────────────────────
  [STATUS]  Heap: 247832 B  |  PSRAM: 8118272 B  |  Temp: 33.1 °C
>
```

## Command Examples

### `psram` — PSRAM Benchmark

```
PSRAM:  detected
  Total size: 8388608 KB
  Write: 12.4 ms (10579 KB/s)
  Verify: 8.2 ms (15980 KB/s)
  Result:  PASS
  Free:   8118 KB
```

### `flash` — Partition Table

```
Flash Chip:
  Size:  16 MB
  Model: GD25Q128
  Mode:  QIO
  Partitions:
    0x1000  size=16 KB   "phy_init"      [data]
    0x8000  size=4 KB    "nvs"           [data]
    0xc000  size=4 KB    "otadata"       [data]
    0x10000 size=1536 KB "app0"          [app, factory]
    0x190000 size=1536 KB "app1"          [app, OTA0]
    0x310000 size=10112KB "ffat"          [data]
  Sketch size: 142 / 1536 KB used
```

### `test` — Peripheral Check

```
Running peripheral sanity check...

1. RGB LED ... PASS
2. PSRAM  ... PASS
3. ADC    ... PASS (value=2048)
4. Temp   ... 32.5 °C (internal sensor)
5. Heap   ... 247832 bytes free

All checks complete — board looks healthy!
```

## Troubleshooting

| Problem | Solution |
|---------|----------|
| Serial Monitor blank | Set baud to **115200**; ensure `USB CDC On Boot = Enabled` |
| Garbled text | Check both Serial Monitor and sketch are set to **115200 baud** |
| No port appears | Try a different USB cable (not charge-only); install CP210x drivers if needed |
| `PSRAM: not enabled` | Set **Tools → PSRAM → OPI PSRAM** and re-upload |
| Upload fails | Enter **download mode**: hold BOOT → press RESET → release BOOT |
| Commands not working | Ensure you type the command **exactly** and press **Enter**; matches are case-insensitive |
| LED doesn't respond | Install **Adafruit NeoPixel** library via Library Manager |

## Learning Path

After this sketch works, move on to:
- [`03_hello_world`](../../01-basics/03_hello_world/README.md) — interact with GPIO pins and serial input
- [`01_oled_hello`](../../02-display/01_oled_hello/README.md) — display text on the I²C OLED

## Code Structure

```
02_serial_test.ino
├── setup()                    → Print banner + system info
├── loop()                     → Auto-status + serial processing
├── processSerialInput()       → Read one command from Serial
├── executeCommand()           → Dispatch to command handler
│
├── cmdHelp()                  → Print command menu
├── cmdPsram()                 → PSRAM alloc/write/verify benchmark
├── cmdFlash()                 → Flash chip + partition table
├── cmdWifi()                  → MAC addresses
├── cmdBlink()                 → Blink LED 5 times
├── cmdTest()                  → 5-step peripheral sanity check
│
├── printBanner()              → OceanLabz branded header
├── printSystemInfo()          → Full chip/flash/PSRAM/heap dump
├── printStatusLine()          → Auto-update every 10 seconds
├── printResetReason()         → Human-readable reset cause
├── printPadded()              → Fixed-width string alignment
├── setLedColor()              → NeoPixel colour setter
└── showPrompt()               → "> " prompt character
