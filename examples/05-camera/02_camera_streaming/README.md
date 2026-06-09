# Camera Streaming — 02_camera_streaming

## Description

Stream live video from the OV2640 camera over WiFi to any web browser. This example creates a **WiFi Access Point** and serves a full web interface with:

- **Live MJPEG streaming** (`/stream` on port 81)
- **Single-frame JPEG capture** (`/capture` on port 80)
- **BMP capture** (`/bmp`)
- **Camera sensor controls** — resolution, quality, brightness, contrast, saturation, white balance, exposure, gain, orientation, special effects, and more

Adapted from the Espressif [CameraWebServer](https://github.com/espressif/esp32s3-camera-webserver) example for the OceanLabz AI Explorer S3 Board. Face detection/recognition and LED flash features have been removed for simplicity.

## Required Hardware

- OceanLabz AI Explorer S3 Board
- OV2640 camera module (connected via FPC ribbon cable)
- USB power supply (≥3A recommended for stable streaming)

## Required Libraries

All libraries are built into the ESP32 Arduino Core (no additional installs needed):

- `esp_camera` — Camera driver
- `esp_http_server` — HTTP server
- `WiFi` — WiFi AP mode
- `img_converters` — JPEG/BMP conversion

## File Structure

```
02_camera_streaming/
├── 02_camera_streaming.ino   — Main sketch: camera init + WiFi AP + server start
├── camera_pins.h             — Pin definitions for AI Explorer S3
├── app_httpd.cpp             — HTTP handlers (stream, capture, control, status, index)
├── camera_index.h            — Web UI HTML page (stored in PROGMEM)
└── README.md                 — This file
```

## Wiring

Connect the OV2640 camera module via the FPC ribbon cable (contacts facing the board). The pin mapping is defined in `camera_pins.h`:

| Signal  | GPIO |
|---------|------|
| XCLK    | 15   |
| PCLK    | 13   |
| VSYNC   | 6    |
| HREF    | 7    |
| SIOD    | 4    |
| SIOC    | 5    |
| D0–D7   | 11,9,8,10,12,18,17,16 |
| PWDN    | N/C  |
| RESET   | N/C  |

## Steps

1. In the Arduino IDE, set **Tools → Board → ESP32S3 Dev Module**
2. Set **Tools → PSRAM → OPI PSRAM** (required for UXGA streaming)
3. Set **Tools → Flash Size → 8MB** (or larger)
4. Upload the sketch (all 4 files must be in the same folder)
5. Open Serial Monitor at **115200 baud**
6. Note the AP IP address printed (default: `192.168.4.1`)
7. Connect your phone/computer to the WiFi AP:
   - **SSID:** `OceanLabz-CAM`
   - **Password:** `12345678`
8. Open a browser and navigate to `http://192.168.4.1`

## Default AP Credentials

| Setting   | Value          |
|-----------|----------------|
| SSID      | OceanLabz-CAM  |
| Password  | 12345678       |
| IP        | 192.168.4.1    |

You can change these in the `.ino` sketch (`ssid` and `password` variables).

## Expected Output

**Serial Monitor:**
```
AP IP address: 192.168.4.1
Camera Ready! Use 'http://192.168.4.1' to connect
```

**Web Browser:**
- Dark-themed OceanLabz-branded UI loads automatically
- Live stream starts automatically
- Control panel on the right side allows adjusting all camera parameters
- "Capture" button takes a single frame; "Save" downloads it as JPEG

## API Endpoints

| Endpoint   | Port | Description                        |
|------------|------|------------------------------------|
| `/`        | 80   | Web UI HTML page                   |
| `/capture` | 80   | Single JPEG frame                  |
| `/bmp`     | 80   | Single BMP frame                   |
| `/status`  | 80   | JSON with current sensor settings  |
| `/control` | 80   | Set parameter (`?var=NAME&val=VAL`)|
| `/stream`  | 81   | MJPEG boundary stream (live video) |

## Camera Control Parameters

The `/control` endpoint accepts these `var` names:

| Parameter      | Range         | Description              |
|----------------|---------------|--------------------------|
| `framesize`    | 2–10          | QQVGA to UXGA            |
| `quality`      | 1–63          | JPEG quality (lower=better)|
| `brightness`   | -2 to 2      | Brightness offset        |
| `contrast`     | -2 to 2      | Contrast offset          |
| `saturation`   | -2 to 2      | Saturation offset        |
| `hmirror`      | 0/1           | Horizontal mirror        |
| `vflip`        | 0/1           | Vertical flip            |
| `awb`          | 0/1           | Auto white balance       |
| `awb_gain`     | 0/1           | AWB gain control         |
| `wb_mode`      | 0–3           | White balance mode       |
| `aec`          | 0/1           | Auto exposure control    |
| `aec2`         | 0/1           | Auto exposure (dsp)      |
| `ae_level`     | -2 to 2      | AE level offset          |
| `aec_value`    | 0–1200        | Manual exposure value    |
| `agc`          | 0/1           | Auto gain control        |
| `agc_gain`     | 0–30          | Manual gain value        |
| `gainceiling`  | 0–6           | Gain ceiling             |
| `special_effect`| 0–6          | No effect / negative / grayscale / tint / sepia |
| `colorbar`     | 0/1           | Color bar test pattern   |
| `bpc`          | 0/1           | Bad pixel correction     |
| `wpc`          | 0/1           | White pixel correction   |
| `raw_gma`      | 0/1           | Raw gamma                |
| `lenc`         | 0/1           | Lens correction          |
| `dcw`          | 0/1           | Downsize window          |

## Troubleshooting

| Problem | Solution |
|---------|----------|
| Camera init fails (0x105) | Enable **OPI PSRAM**; check FPC ribbon cable orientation |
| No WiFi visible | Board may be in STA mode — check sketch uses `WiFi.softAP()` |
| Web page loads but no image | Reduce resolution to SVGA or VGA; check PSRAM is enabled |
| Stream is choppy / low FPS | Lower resolution; increase JPEG quality number (lower quality); use a 3A power supply |
| Stream stops after a few seconds | Power supply insufficient — use ≥3A USB supply |
| Browser can't connect to stream | Stream is on port 81 — ensure no firewall blocking; try `http://192.168.4.1:81/stream` directly |
| Compile errors on `app_httpd.cpp` | Ensure all 4 files are in the same sketch folder; select ESP32S3 board |
| Sketch too large for flash | Set **Tools → Flash Size → 8MB**; or reduce HTML in `camera_index.h` |

## Notes

- **PSRAM is required** for resolutions above SVGA. Without PSRAM, the sketch falls back to SVGA in DRAM.
- The stream runs on **port 81** (separate HTTP server instance) to avoid blocking the control/capture server on port 80.
- The web UI auto-starts the stream on page load.
- This example uses **AP mode** by default. To use **STA mode** (connect to your router), replace `WiFi.softAP()` with `WiFi.begin(ssid, password)` in the sketch.
