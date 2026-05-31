# Installing the ESP32 Arduino Core

This guide explains how to add ESP32-S3 support to your Arduino IDE so you can program the OceanLabz AI Explorer S3 Board.

---

## Step 1: Open Arduino IDE Preferences

1. Launch **Arduino IDE**
2. Go to **File** → **Preferences** (Windows/Linux) or **Arduino IDE** → **Preferences** (macOS)
3. Locate the field labeled **"Additional Boards Manager URLs"**

---

## Step 2: Add the ESP32 Board URL

1. Click the small icon to the right of the text field (or paste directly)
2. Add the following URL:

   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-packages/package_esp32_index.json
   ```

3. If you already have URLs in this field, separate them with a comma

   **Example with multiple URLs:**
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-packages/package_esp32_index.json, https://adafruit.github.io/arduino-board-index/package_adafruit_index.json
   ```

4. Click **OK**

---

## Step 3: Install the ESP32 Board Package

1. Go to **Tools** → **Board** → **Boards Manager**
2. In the search box, type `ESP32`
3. Find **"esp32 by Espressif Systems"**
4. Click **Install**
5. Wait for the installation to complete (may take several minutes)

---

## Step 4: Select Your Board

1. Go to **Tools** → **Board** → **ESP32 Arduino** (or **ESP32S3**)
2. Select **"ESP32S3 Dev Module"**

> If a custom "AI Explorer S3" board entry is available in a future release, use that instead. For now, "ESP32S3 Dev Module" is fully compatible.

---

## Step 5: Configure Board Settings

Set the following options in **Tools** menu:

| Setting | Value |
|---------|-------|
| Board | `ESP32S3 Dev Module` |
| Upload Speed | `921600` (or `115200` if upload fails) |
| USB CDC On Boot | `Enabled` |
| Flash Size | `16MB (128Mb)` |
| Partition Scheme | `16M Flash (3M APP/9.9MB FATFS)` |
| PSRAM | `OPI PSRAM` |
| Arduino Runs On | `Core 1` |
| Events Run On | `Core 1` |

> **⚠️ PSRAM must be set to "OPI PSRAM"** for camera examples to work.

---

## Troubleshooting

| Problem | Solution |
|---------|----------|
| "Board not found" error | Ensure the URL is correct and try restarting Arduino IDE |
| Installation fails | Check internet connection; try using a VPN if behind a firewall |
| Old ESP32 version | Re-open Boards Manager and check for updates |
| Upload fails at 921600 | Reduce Upload Speed to 115200 |

---

## Verification

To verify installation, go to **File** → **Examples** → **ESP32** and check if example sketches are available. If you see them, the installation was successful!

---

## Next Steps

➡️ Proceed to [Uploading Your First Sketch](uploading-first-sketch.md)