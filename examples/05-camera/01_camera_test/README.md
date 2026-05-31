# Camera Test

## Description
Initialize the camera module (OV2640/OV3660/OV5640), capture a single frame, and output image information over Serial. This is the first step in getting the camera working.

## Required Hardware
- OceanLabz AI Explorer S3 Board
- OV2640 camera module (recommended) or OV3660/OV5640
- USB Type-C cable

## Required Libraries
- ESP32 Camera (built-in with ESP32 Arduino Core v2.0.14+)

## Wiring
Connect camera via 24-pin FPC ribbon cable. **Metal contacts must face the board.**

## Steps
1. Set **Tools → PSRAM → OPI PSRAM** (REQUIRED)
2. Select correct camera model in sketch (`CAMERA_MODEL_OV2640`)
3. Upload `01_camera_test.ino`
4. Open Serial Monitor at 115200 baud

## Expected Output
```
Camera Test — OceanLabz AI Explorer S3
Camera ID: OV2640
Resolution: 320x240 (QVGA)
Frame size: 15400 bytes
Pixel format: JPEG
```

## Troubleshooting
| Problem | Solution |
|---------|----------|
| Init fails | Enable OPI PSRAM in Tools menu |
| Init fails | Check ribbon orientation (contacts face board) |
| Init fails | Verify camera model constant in sketch |
| Wrong camera ID | Check ribbon connection; try different camera |