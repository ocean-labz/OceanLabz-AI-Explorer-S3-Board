# Camera + SD Capture

## Description
Capture JPEG photos from the camera and save them directly to the microSD card. Each capture creates a timestamped file on the SD card.

## Required Hardware
- OceanLabz AI Explorer S3 Board
- OV2640 (or OV3660/OV5640) camera module
- microSD card (FAT32 formatted)

## Required Libraries
- ESP32 Camera (built-in)
- SD_MMC (built-in)

## Wiring
Connect camera via FPC ribbon cable. Insert microSD card.

## Steps
1. Set **Tools → PSRAM → OPI PSRAM**
2. Upload `03_camera_sd_capture.ino`
3. Open Serial Monitor at 115200 baud
4. Press the **BOOT** button to capture a photo

## Expected Output
```
Capturing... OK!
Saved to /photo_0001.jpg
File size: 28420 bytes
```
Remove SD card and view photos on your computer.

## Troubleshooting
| Problem | Solution |
|---------|----------|
| Photo not saved | Check SD card insertion; format as FAT32 |
| Camera init fails | Enable OPI PSRAM |
| File size 0 | SD card write protected or full |
| BOOT button not working | Check button wiring; press firmly |