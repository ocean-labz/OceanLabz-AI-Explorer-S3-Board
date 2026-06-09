do# AI Voice Assistant Demo

## Description

A full-featured AI voice assistant for the **OceanLabz AI Explorer S3 Board** that runs a complete voice pipeline entirely on-device (microphone to speaker) using cloud AI APIs:

| Stage | Service | Protocol |
|-------|---------|----------|
| Record | INMP441 mic - PSRAM (2 s, 16 kHz 16-bit PCM) | I2S (port 0) |
| Speech-to-Text | OpenAI Whisper (whisper-1) | HTTPS multipart upload |
| AI Response | OpenAI ChatGPT (gpt-4o-mini) | HTTPS JSON |
| Text-to-Speech | OpenAI TTS (tts-1, voice: alloy) | HTTPS stream - MP3 in PSRAM |
| Playback | MAX98357A speaker | I2S (port 1), AudioGeneratorMP3 |

**No SD card required.** All audio buffers are allocated in PSRAM (64 KB WAV + 256 KB MP3).

### Why this approach?

- **Fast** - Shared TLS keep-alive connection across all 3 API calls eliminates redundant handshakes; 2-second recording minimises capture time
- **Accurate** - Whisper STT with GPT-4o-mini understands any natural language, not just a fixed vocabulary
- **Simple** - Type `RECORD` in Serial Monitor, speak, and hear the AI reply through the speaker

## Flow

1. Type `RECORD` in the Serial Monitor (or connect a physical trigger button)
2. **Magenta** LED - recording 2 seconds of audio from the INMP441 mic
3. **Blue** LED - uploading to Whisper for speech-to-text
4. **Cyan** LED - sending transcribed text to ChatGPT
5. **Teal** LED - downloading TTS audio (MP3) from OpenAI
6. **Green** LED - playing the AI reply through the MAX98357A speaker

All 3 API calls reuse a **single persistent TLS connection** (`Connection: keep-alive`) - the connection is established once and kept open for the entire pipeline.

You can also type any text directly in the Serial Monitor to skip the mic - Whisper step and go straight to ChatGPT - TTS - speaker.

## Required Hardware

- OceanLabz AI Explorer S3 Board (with PSRAM - 8 MB PSRAM onboard)

## Board Pin Mapping

All pins are defined in `tools/pin_configuration/pins_ai_explorer_s3.h` and used exactly as declared:

| Function | GPIO | Peripheral |
|----------|------|------------|
| RGB LED | 14 | WS2812B NeoPixel |
| I2S Mic WS | 39 | INMP441 Word Select (LRCLK) |
| I2S Mic SCK | 40 | INMP441 Serial Clock (BCLK) |
| I2S Mic SD | 41 | INMP441 Serial Data (DOUT) |
| Speaker LRCK | 21 | MAX98357A Left-Right Clock |
| Speaker BCLK | 48 | MAX98357A Bit Clock |
| Speaker DIN | 47 | MAX98357A Data In |
| I2S Mic Port | 0 | I2S_NUM_0 |
| I2S Speaker Port | 1 | I2S_NUM_1 |

## Required Libraries

| Library | Version | Purpose |
|---------|---------|---------|
| ESP32 Arduino Core | >= 2.0.14 | Board support, driver/i2s_std.h (new I2S driver), WiFi.h |
| Adafruit NeoPixel | >= 1.12 | WS2812B RGB LED |
| ArduinoJson | >= 6.21 | JSON parsing for API responses |
| ESP8266Audio | >= 1.9.7 | AudioGeneratorMP3 for MP3 playback |

**Installation (Arduino IDE):**
1. **Tools - Board - Boards Manager** - install "ESP32 by Espressif Systems" (>= 2.0.14)
2. **Tools - Manage Libraries** - install:
   - "Adafruit NeoPixel" by Adafruit
   - "ArduinoJson" by Benoit Blanchon
   - "ESP8266Audio" by Earle F. Philhower

## Board Configuration (Tools menu)

| Setting | Value |
|---------|-------|
| Board | ESP32S3 Dev Module |
| Upload Speed | 921600 |
| CPU Frequency | 240 MHz (WiFi/BT) |
| Flash Mode | QIO 80 MHz |
| Flash Size | 16 MB (128 Mb) |
| Partition Scheme | Huge APP (3MB No OTA/1MB SPIFFS) |
| PSRAM | **OPI PSRAM** (must be enabled for buffer allocation) |
| Arduino Runs On | Core 1 |
| USB CDC On Boot | Enabled |

**PSRAM must be enabled** - the sketch allocates ~320 KB of PSRAM for audio buffers.

## WiFi and API Configuration

The sketch connects to your WiFi and uses the OpenAI API. Edit these in the sketch before uploading:

```cpp
const char* ssid     = "oceanlabz";
const char* password = "Online@1234";
const char* apiKey   = "sk-...";  // OpenAI API key
```

You need:
- An **OpenAI API key** with access to Whisper, GPT-4o-mini, and TTS models

## Upload and Run

1. Install libraries and configure board settings (see above)
2. Set your WiFi credentials and OpenAI API key in the sketch
3. Upload the sketch
4. Open **Serial Monitor** at **115200 baud**
5. Wait for WiFi connection (`WiFi OK  IP: 192.168.x.x`)
6. Type `RECORD` and press Enter
7. Speak clearly into the microphone when you see the magenta LED
8. Wait ~5-12 s for the pipeline to complete and the AI to speak back

### Serial Commands

| Command | Action |
|---------|--------|
| RECORD | Full pipeline: Record - Whisper - ChatGPT - TTS - Speaker |
| any text | Direct ChatGPT - TTS - Speaker (no mic) |
| HELP | Show this menu |

## LED Status Reference

| LED Colour | Meaning |
|------------|---------|
| Purple | Booting / initialising |
| Red (solid) | Error / failure |
| Dim green | Ready, waiting for command |
| Magenta | Recording audio |
| Blue | Uploading to Whisper (STT) |
| Cyan | Querying ChatGPT |
| Teal | Downloading TTS audio |
| Green (flashing) | Playing audio through speaker |

## Optimisation and Tuning

### Recording quality

The I2S microphone reads 24-bit data in 32-bit frames via the **new I2S driver API** (`i2s_std.h`), removes DC offset per batch, applies a noise gate (`abs(val) < 30 -> 0`), and right-shifts by **16 bits** to fit into 16-bit PCM (INMP441 left-aligns 24-bit data in the 32-bit frame). If the recording is too quiet or too loud:

- **Quiet** - reduce the shift: change `>> 16` to `>> 13`
- **Loud** - increase the shift: change `>> 16` to `>> 18`

### VAD / noise gate

The noise gate zeros samples below `abs(30)`. If you're in a noisy environment, increase this threshold. In a quiet environment, decrease it.

### Recording duration

Change `RECORDING_SECONDS` (currently 2 s) - PSRAM buffer supports up to 10 s at 16 kHz.

### TTS speed

The TTS request includes `"speed":1.0` in the JSON payload - range 0.25 (slow) to 4.0 (fast).

### Speaker volume

Adjust `audioOut->SetGain(0.85)` - range 0.0 (mute) to 1.0 (max).

### TLS keep-alive optimisation

All 3 API calls (Whisper STT - ChatGPT - TTS) share a **single persistent TLS connection** to `api.openai.com:443`. This eliminates 2 redundant TLS handshakes per voice cycle, saving approximately 2-3 seconds. The shared connection is managed by `ensureApiConnection()` and uses `Connection: keep-alive` on all requests.

## How Audio Flows Without SD Card

```
INMP441 (I2S port 0)
    |
    v
ps_malloc'd int16_t[32000]  <- 64 KB PSRAM (recordBufWav)
    |
    v  (WAV header prepended in multipart upload)
OpenAI Whisper STT API -> text
    |
    v
OpenAI ChatGPT API -> response text
    |
    v
OpenAI TTS API -> MP3 bytes -> ps_malloc'd uint8_t[262144] (mp3Buf, 256 KB PSRAM)
    |
    v
AudioFileSourceRAM (custom class)  <- feeds MP3 to decoder
    |
    v
AudioGeneratorMP3 -> AudioOutputI2S (I2S port 1)
    |
    v
MAX98357A speaker
```

All three OpenAI API calls share a single TCP+TLS connection (keep-alive).

## Troubleshooting

| Problem | Likely Cause | Solution |
|---------|-------------|----------|
| Board crashes/reboots on boot | Flash not erased after upload OR PSRAM not enabled | Use **Tools - Erase Flash** before re-upload; enable **OPI PSRAM** |
| PSRAM allocation fails | PSRAM not enabled in Tools | Enable **OPI PSRAM** in Tools - PSRAM |
| WiFi doesn't connect | Wrong SSID/password | Check SSID/password in sketch |
| STT returns empty | API key invalid or quota exhausted | Verify apiKey and billing status |
| GPT returns error | API quota / model access | Check you have access to gpt-4o-mini |
| TTS returns no data | API key lacks TTS access | Verify API key scopes |
| Recording has 0 samples | Mic not connected or dead | Check I2S pins, test with 01_mic_recorder |
| Playback distorted | Shift mismatch | Adjust >> 16 in recordAudioToRAM() |

## See Also

- `01_mic_recorder` - standalone mic test
- `02_speaker_test` - standalone speaker test
