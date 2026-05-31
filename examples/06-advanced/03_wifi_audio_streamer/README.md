# WiFi Audio Streamer

## Description
Stream audio from the INMP441 microphone over WiFi to a listening client (e.g., a browser or audio player). This example combines audio capture with network streaming.

## Required Hardware
- OceanLabz AI Explorer S3 Board
- WiFi router (2.4 GHz)

## Required Libraries
- WiFi (built-in)
- ESP32-audioI2S by schreibfaul1

## Wiring
No external wiring needed. Microphone is onboard.

## Steps
1. Set WiFi credentials in sketch
2. Upload `03_wifi_audio_streamer.ino`
3. Open Serial Monitor at 115200 baud
4. Note the IP address
5. Connect to `http://<IP>/stream` from a browser/media player

## Expected Output
- Serial Monitor shows: `Streaming at http://192.168.x.x/stream`
- Browser receives raw audio data / browser-compatible stream

## Troubleshooting
| Problem | Solution |
|---------|----------|
| WiFi not connecting | Check SSID/password; 2.4 GHz only |
| No audio stream | Check firewall settings; use VLC to test stream |
| Audio choppy | Reduce sample rate or bit depth |
| Buffer underrun | Increase audio buffer size in sketch |