# ❓ Troubleshooting Guide

Common issues and solutions for the OceanLabz AI Explorer S3 Board.

---

## Quick Reference

| Symptom | Most Likely Fix |
|---------|-----------------|
| Board not detected | Use data USB cable; enter download mode |
| Upload fails | Lower upload speed; check PSRAM setting |
| No serial output | Set baud to 115200; enable USB CDC On Boot |
| Display blank | Wrong library; GPIO conflict between OLED and TFT |
| No audio | Wrong I²S port; speaker not connected |
| Camera init fails | PSRAM not enabled; ribbon upside down |
| SD card fails | Format as FAT32; use 1-bit mode |
| WiFi won't connect | Check 2.4 GHz; SSID/password mismatch |

---

## Board Not Detected

| Cause | Solution |
|-------|----------|
| USB cable is charge-only | Use a cable known to transfer data |
| Wrong USB port | Try USB 2.0 port; avoid USB hubs |
| Driver not installed | ESP32-S3 uses native USB — no external driver needed on Win10/11, macOS, Linux |
| Board in wrong mode | Press RESET button to restart |
| Bootloader corrupted | Re-flash bootloader (see [Recovery](#recovery)) |

### Enter Download Mode
If the board is not responding:

1. Hold the **BOOT** button
2. Press and release **RESET**
3. Release **BOOT**
4. The board is now in download mode — try uploading again

---

## Upload Failures

| Error | Cause | Solution |
|-------|-------|----------|
| `Connecting..._____....._____.....` | Board not in download mode | Enter download mode (BOOT + RESET) |
| `A fatal error occurred: Failed to connect` | Wrong port selected | Check Tools → Port |
| `Invalid head of packet` | Upload speed too high | Reduce to 115200 in Tools → Upload Speed |
| `Flash encryption error` | Flash corrupted | Erase flash: `esptool.py erase_flash` |
| `No serial data received` | USB CDC On Boot disabled | Enable in Tools → USB CDC On Boot → Enabled |

---

## Display Issues

### OLED (I²C)
| Problem | Solution |
|---------|----------|
| No display | Run I²C scanner to check address (0x3C or 0x3D) |
| Garbled text | Update Adafruit SSD1306 + GFX libraries |
| Dim display | Increase contrast in `display.ssd1306_command(SSD1306_SETCONTRAST, 0xFF)` |
| Flickering | Add 100µF capacitor near OLED VCC/GND |

### TFT (SPI)
| Problem | Solution |
|---------|----------|
| White screen (backlight on) | TFT_eSPI `User_Setup.h` missing or wrong driver |
| No backlight | Check GPIO42 (BL) is set HIGH |
| Wrong colors | RGB/BGR order wrong in `User_Setup.h` |
| Screen flashes | CS pin conflict — ensure GPIO45 is not used elsewhere |

---

## Audio Issues

| Problem | Solution |
|---------|----------|
| Mic records silence | Verify WS=39 (LRCLK), SCK=40 (BCLK), SD=41 (DOUT) |
| Distorted speaker audio | Reduce volume in code; check speaker impedance (4-8Ω) |
| No sound from speaker | Check speaker connected to SPK+/SPK- terminals |
| High-pitched whine | Power supply noise — use battery or add 100µF capacitor near MAX98357A |
| Both mic/speaker fail | I²S port conflict — use different I²S port numbers |

---

## Camera Issues

| Problem | Solution |
|---------|----------|
| `Camera init failed` | Enable PSRAM: Tools → PSRAM → OPI PSRAM |
| `Camera init failed` | Check ribbon cable orientation (contacts face board) |
| `Camera init failed` | Verify `CAMERA_MODEL_OV2640` in sketch |
| `Out of memory` | Lower resolution to FRAMESIZE_QVGA or FRAMESIZE_VGA |
| Garbled image | Re-seat ribbon cable; check for bent pins |
| Low FPS | Reduce resolution; disable WiFi; use JPEG output |

---

## SD Card Issues

| Problem | Solution |
|---------|----------|
| `Card Mount Failed` | Format as FAT32 (not NTFS or exFAT) |
| `Card Mount Failed` | Use `SD_MMC.begin("/sdcard", true)` for 1-bit mode |
| `Card Mount Failed` | Clean card contacts; try different card |
| Slow write speed | Use Class 10 or UHS-I card |
| `File not found` | Paths are case-sensitive (`/data.txt` ≠ `/Data.txt`) |
| Files disappear | Card may be counterfeit — buy from reputable seller |

---

## Power Issues

| Problem | Solution |
|---------|----------|
| Board resets when using camera | Insufficient power — use 3A USB supply or battery |
| RGB LED dim | Low battery voltage |
| Battery not charging | Check charger IC; verify USB supply ≥ 500mA |
| Board hot | Possible short circuit — disconnect all peripherals |

---

## Recovery

If the board becomes unresponsive:

### Method 1: Re-flash Bootloader
1. Install Python and `esptool`:
   ```bash
   pip install esptool
   ```
2. Erase flash:
   ```bash
   esptool.py --chip esp32s3 --port COMxx erase_flash
   ```
3. Re-upload a sketch from Arduino IDE

### Method 2: Factory Reset
1. Enter download mode (BOOT + RESET)
2. Upload the **Blink** example at 115200 baud
3. If Blink works, other sketches should too

### Method 3: Serial Recovery
1. Connect via serial terminal (115200 baud)
2. Press RESET and watch boot messages
3. Look for `Brownout detector was triggered` (power issue) or `Guru Meditation Error` (code crash)

---

## Still Stuck?

| Resource | Link |
|----------|------|
| 💬 GitHub Discussions | [Open a discussion](https://github.com/ocean-labz/OceanLabz-AI-Explorer-S3-Board/discussions) |
| 🐛 File a Bug Report | [Create an issue](https://github.com/ocean-labz/OceanLabz-AI-Explorer-S3-Board/issues/new/choose) |
| 📹 YouTube Tutorials | [OceanLabz YouTube](https://youtube.com/@ocean-labz) (placeholder) |

When asking for help, please provide:
1. Arduino IDE version and ESP32 core version
2. Board settings (PSRAM, Flash, Partition Scheme)
3. Exact error message (copy-paste, not screenshot)
4. Minimal sketch that reproduces the issue
5. What peripherals are connected