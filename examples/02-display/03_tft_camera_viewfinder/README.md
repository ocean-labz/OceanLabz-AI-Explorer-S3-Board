# TFT Camera Viewfinder

## Description
Display a live camera feed on the SPI TFT display. This example combines the camera module and TFT display to create a real-time viewfinder.

## Required Hardware
- OceanLabz AI Explorer S3 Board
- SPI TFT display (ILI9341 240×320)
- OV2640 camera module (or OV3660/OV5640)

## Required Libraries
- **TFT_eSPI** by Bodmer (v2.5.0+)
- ESP32 Camera (built-in)

## Wiring
Connect camera via ribbon cable (contacts facing board). Connect TFT as shown in [TFT Graphics README](../02_tft_graphics/README.md).

## Steps
1. Install TFT_eSPI and configure User_Setup.h
2. Connect camera and TFT display
3. Set **Tools → PSRAM → OPI PSRAM**
4. Open `03_tft_camera_viewfinder.ino`
5. Select correct camera model (`CAMERA_MODEL_OV2640` etc.)
6. Upload

## Expected Output
- TFT display shows live video feed from the camera
- Frame rate displayed in corner
- Press RESET to restart

## Troubleshooting
| Problem | Solution |
|---------|----------|
| Camera init fails | Enable PSRAM; check ribbon orientation |
| No TFT output | Verify User_Setup.h; check TFT wiring |
| Low FPS | Reduce camera resolution in sketch |
| Display flickers | Power supply insufficient — use battery or 3A supply |