# Voice Recorder

## Description
Record audio from the INMP441 microphone and save it as a WAV file on the microSD card. Then play it back through the MAX98357A speaker. This is a complete audio capture and playback system.

## Required Hardware
- OceanLabz AI Explorer S3 Board
- microSD card (FAT32 formatted, Class 10 recommended)
- Speaker (4Ω–8Ω, up to 3W)

## Required Libraries
- ESP32-audioI2S by schreibfaul1 (for playback, optional)
- SD_MMC (built-in)

## Wiring
Insert SD card. Connect speaker to SPK+/SPK-.

## Steps
1. Insert FAT32-formatted SD card
2. Upload `02_voice_recorder.ino`
3. Open Serial Monitor at 115200 baud
4. Press **BOOT** to start recording (5 seconds)
5. Press **BOOT** again to play back

## Expected Output
```
Ready. Press BOOT to record...
Recording... (5 seconds)
Recording complete! /recording.wav (42800 bytes)
Press BOOT to play back...
Playing...
Playback complete.
```

## Troubleshooting
| Problem | Solution |
|---------|----------|
| WAV file corrupted | SD card too slow — use Class 10 or better |
| Playback too fast/slow | Check sample rate (16000 Hz recommended) |
| No recording | Check microphone onboard; speak clearly |
| "SD full" | Free space on SD card; use larger card |