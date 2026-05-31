# 📗 Uploading Your First Sketch

This guide walks you through connecting your OceanLabz AI Explorer S3 Board and uploading your first program.

---

## Step 1: Connect the Board

1. Plug the board into your computer using a **USB Type-C** cable
2. You should see a new COM port appear (Windows) or a `/dev/ttyACM0` or `/dev/ttyUSB0` device (macOS/Linux)
3. The onboard RGB LED may light up briefly

> **⚠️ Use a data-capable USB cable.** Charge-only cables will not work for programming.

---

## Step 2: Select Port and Board in Arduino IDE

1. Open Arduino IDE
2. Go to **Tools** → **Port** → select the COM port for your board
3. Go to **Tools** → **Board** → **ESP32S3 Dev Module**
4. Verify settings:
   - **USB CDC On Boot**: `Enabled`
   - **PSRAM**: `OPI PSRAM`
   - **Flash Size**: `16MB (128Mb)`

---

## Step 3: Open the Blink Example

1. Go to **File** → **Open**
2. Navigate to the repository folder: `examples/01-basics/01_blink/`
3. Select `01_blink.ino`
4. The sketch will open in a new Arduino IDE window

---

## Step 4: Upload

1. Click the **→ (Upload)** button in the top-left toolbar
2. Watch the bottom console for progress:

```
Connecting...
Chip is ESP32-S3
Uploading...
Writing at 0x00000000... (100%)
Done!
```

3. If you see `Connecting..._____..._____...` for more than 10 seconds:
   - Hold the **BOOT** button on the board
   - Press and release **RESET**
   - Release **BOOT**
   - Arduino IDE should connect and upload

---

## Step 5: Verify

After successful upload:
- The RGB LED on the board should blink (on for 1 second, off for 1 second)
- Open **Serial Monitor** (**Tools** → **Serial Monitor**) at **115200 baud** to see serial output

---

## Troubleshooting

| Problem | Solution |
|---------|----------|
| "No port found" | Try a different USB cable; install drivers; check Device Manager |
| Upload hangs at "Connecting" | Enter download mode: hold BOOT → press RESET → release BOOT |
| "A fatal error occurred" | Lower Upload Speed to 115200; check PSRAM setting |
| Serial Monitor is blank | Set baud rate to 115200; check USB CDC On Boot = Enabled |

---

## Success! 🎉

You've uploaded your first sketch to the OceanLabz AI Explorer S3 Board. Now explore:

- [Examples Index](../../examples/README.md)
- [Display Guide](../guides/display/README.md)
- [Audio Guide](../guides/audio/README.md)