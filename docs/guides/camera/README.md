# 📷 Camera Guide

This guide covers connecting and using camera modules with the OceanLabz AI Explorer S3 Board.

---

## Supported Cameras

| Sensor | Max Resolution | FOV | Notes |
|--------|:--------------:|:---:|-------|
| **OV2640** | 1600×1200 (2MP) | 65° | ✅ **Recommended for beginners** - best balance of cost and quality |
| **OV3660** | 2048×1536 (3MP) | 70° | Higher resolution, slightly more power |
| **OV5640** | 2592×1944 (5MP) | 66° | Highest resolution, requires PSRAM optimization |

> 💡 **Start with the OV2640.** It is the most widely available, best documented, and easiest to configure.

---

## Camera Connection

### Ribbon Cable
The camera connects to the board via a **24-pin FPC ribbon cable**.

**Orientation:**
1. Lift the black latch on the board's camera connector
2. Insert the ribbon cable with **metal contacts facing down** (toward the board)
3. Press the latch down to secure

> ⚠️ **Common mistake:** Inserting the cable upside-down. The metal contacts must face the board, not away from it.

### Pin Mapping
The camera uses the ESP32-S3's **Camera Parallel Interface**. Pins are fixed by the board design and should be defined in your sketch:

```cpp
#define PWDN_GPIO_NUM    -1   // Not connected
#define RESET_GPIO_NUM   -1   // Not connected (shared with TFT)
#define XCLK_GPIO_NUM    15
#define SIOD_GPIO_NUM    4
#define SIOC_GPIO_NUM    5
#define Y9_GPIO_NUM      6
#define Y8_GPIO_NUM      7
#define Y7_GPIO_NUM      8
#define Y6_GPIO_NUM      9
#define Y5_GPIO_NUM      10
#define Y4_GPIO_NUM      11
#define Y3_GPIO_NUM      12
#define Y2_GPIO_NUM      13
#define VSYNC_GPIO_NUM   14   // Shared with RGB LED
#define HREF_GPIO_NUM    16
#define PCLK_GPIO_NUM    17
```

---

## Required Libraries

Install via Library Manager:
- **ESP32 Camera** by Espressif (built-in with ESP32 Arduino Core v2.0.14+)

Enable in Arduino IDE:
- **Tools** → **PSRAM** → **OPI PSRAM** (required for all camera operations)

---

## PSRAM Configuration

Camera frames require PSRAM. This is an **absolute requirement** — no PSRAM = no camera.

| Setting | Required Value |
|---------|:--------------:|
| PSRAM | `OPI PSRAM` |
| Flash Size | `16MB (128Mb)` |
| Partition Scheme | `16M Flash (3M APP/9.9MB FATFS)` |

---

## Camera Examples

| Example | Description | Level |
|---------|-------------|:-----:|
| [Camera Test](../../examples/05-camera/01_camera_test/) | Capture a single frame, display on Serial | Beginner |
| [Camera Streaming](../../examples/05-camera/02_camera_streaming/) | Stream video over WiFi to web browser | Intermediate |
| [Camera + TFT Viewfinder](../../examples/02-display/03_tft_camera_viewfinder/) | Live camera feed on TFT display | Intermediate |
| [Camera + SD Capture](../../examples/05-camera/03_camera_sd_capture/) | Capture photos and save to SD card | Advanced |

---

## Troubleshooting

| Problem | Likely Cause | Solution |
|---------|--------------|----------|
| "Camera init failed" | Ribbon cable inserted upside-down | Re-insert with contacts facing the board |
| "Camera init failed" | PSRAM not enabled | Set Tools → PSRAM → OPI PSRAM |
| "Camera init failed" | Wrong camera model | Verify `CAMERA_MODEL_OV2640` (or correct model) in sketch |
| "Out of memory" | Frame size too large | Reduce resolution to `FRAMESIZE_QVGA` (320×240) |
| Garbled image | Loose ribbon cable | Reseat the FPC connector firmly |
| No image, but init succeeds | Wrong pin mapping | Use the pin mapping from this guide |
| FPS too low | High resolution + WiFi | Drop to QVGA or UXGA for streaming; disable JPEG compression |
| WiFi + Camera crash | Insufficient power | Use battery or 3A USB power supply |

---

## Performance Tips

| Setting | QVGA (320×240) | VGA (640×480) | SVGA (800×600) | UXGA (1600×1200) |
|---------|:--------------:|:-------------:|:--------------:|:-----------------:|
| **FPS (JPEG)** | 25-30 | 15-20 | 10-15 | 3-5 |
| **FPS (Grayscale)** | 30+ | 20-25 | 15-20 | 5-8 |
| **WiFi Streaming** | ✅ Smooth | ✅ Good | ⚠️ Choppy | ❌ Not recommended |

- Use **JPEG** output for streaming (smaller frame size)
- Use **Grayscale** for higher FPS
- Disable **WiFi** for maximum camera FPS
- Use **QVGA or VGA** for most projects