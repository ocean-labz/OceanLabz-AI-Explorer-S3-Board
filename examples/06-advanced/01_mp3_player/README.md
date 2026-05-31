# MP3 Player

## Description
Play MP3 audio files stored on the microSD card through the MAX98357A speaker amplifier. This example demonstrates audio file decoding and I²S output using the ESP32-audioI2S library.

## Required Hardware
- OceanLabz AI Explorer S3 Board
- microSD card (FAT32 formatted)
- Speaker (4Ω–8Ω, up to 3W)
- MP3 audio files

## Required Libraries
- **ESP32-audioI2S** by schreibfaul1 (install via Library Manager)

## Wiring
Connect speaker to SPK+/SPK- terminals. Insert SD card with MP3 files.

## Steps
1. Install ESP32-audioI2S via Library Manager
2. Copy MP3 files to root of SD card (e.g., `/song1.mp3`, `/song2.mp3`)
3. Upload `01_mp3_player.ino`
4. Open Serial Monitor at 115200 baud

## Expected Output
- Speaker plays the first MP3 file from the SD card
- Serial Monitor shows: `Playing: /song1.mp3`
- Automatically advances to next file when current ends

## Troubleshooting
| Problem | Solution |
|---------|----------|
| No sound | Check speaker connection; verify MP3 files on SD card |
| "File not found" | MP3 must be in root directory (not subfolder) |
| Garbled audio | MP3 bitrate too high — use 128kbps or lower |
| "SD init failed" | Format SD as FAT32; check insertion |