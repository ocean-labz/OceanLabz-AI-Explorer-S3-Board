# 🎤 Audio Guide — Microphone & Speaker

This guide covers using the onboard INMP441 digital microphone and MAX98357A speaker amplifier.

---

## Audio System Overview

```
┌─────────────┐     I²S      ┌──────────────┐
│  INMP441     │◄───────────►│  ESP32-S3    │
│  (Microphone)│  WS=39      │              │
│              │  SCK=40     │              │
│              │  SD=41      │              │
└─────────────┘              │              │
                             │              │
┌─────────────┐              │              │
│  MAX98357A  │◄───────────►│              │
│  (Speaker)  │  LRCK=21    │              │
│             │  BCLK=48    │              │
│  3W Output  │  DIN=47     └──────────────┘
└─────────────┘
```

Both devices use the I²S protocol but are on **separate I²S buses** — they can operate simultaneously.

---

## Microphone — INMP441

### Specifications
| Parameter | Value |
|-----------|-------|
| Type | Digital I²S MEMS Microphone |
| Sensitivity | -26 dBFS (±1 dB) |
| SNR | 61 dBA |
| Frequency Response | 60 Hz – 15 kHz |
| Voltage | 1.8V – 3.3V |

### INMP441 Pin Mapping
| INMP441 Pin | GPIO | Notes |
|-------------|:----:|-------|
| WS (Word Select) | **39** | I²S LRCLK |
| SCK (Serial Clock) | **40** | I²S BCLK |
| SD (Serial Data) | **41** | I²S DOUT |

### Required Library
- No additional library required — use `esp32-hal-i2s.h` (built into ESP32 Arduino Core)

### Example
➡️ [Microphone Recorder](../../examples/03-audio/01_mic_recorder/)

---

## Speaker Amplifier — MAX98357A

### Specifications
| Parameter | Value |
|-----------|-------|
| Type | I²S Class D Amplifier |
| Output Power | 3.2W @ 4Ω, 3.7V |
| Efficiency | 92% |
| SNR | 95 dBA |
| Load Impedance | 4Ω – 8Ω |

### MAX98357A Pin Mapping
| MAX98357A Pin | GPIO | Notes |
|---------------|:----:|-------|
| LRCK | **21** | Left-Right Clock (I²S) |
| BCLK | **48** | Bit Clock (I²S) |
| DIN | **47** | Data In (I²S) |

> The MAX98357A uses **I²S standard format** (not left-justified). Ensure your audio library is configured for I²S standard.

### Required Library
Install via Library Manager:
- **ESP32-audioI2S** by schreibfaul1 (for MP3/streaming playback)
- Or use built-in `esp32-hal-i2s.h` for raw audio

### Example
➡️ [Speaker Test](../../examples/03-audio/02_speaker_test/)

---

## Speaker Connection

| Terminal | Connection |
|----------|------------|
| SPK+ | Speaker positive (red) |
| SPK- | Speaker negative (black) |

> **Recommended speaker:** 3W – 5W, 4Ω – 8Ω impedance. A small 3W 4Ω speaker works well for voice and music.

---

## Audio Examples

| Example | Description | Level |
|---------|-------------|:-----:|
| [Mic Recorder](../../examples/03-audio/01_mic_recorder/) | Record audio from INMP441, view waveform in Serial Plotter | Beginner |
| [Speaker Test](../../examples/03-audio/02_speaker_test/) | Play a test tone through MAX98357A | Beginner |
| [Voice Assistant Demo](../../examples/03-audio/03_voice_assistant_demo/) | Basic voice command detection and response | Advanced |
| [Voice Recorder](../../examples/06-advanced/02_voice_recorder/) | Record to SD card, play back through speaker | Advanced |
| [MP3 Player](../../examples/06-advanced/01_mp3_player/) | Play MP3 files from SD card | Advanced |

---

## Troubleshooting

| Problem | Likely Cause | Solution |
|---------|--------------|----------|
| No audio from mic | Wrong I²S port | Ensure I²S port 0 is used for microphone (not port 1) |
| Distorted audio from speaker | Volume too high | Reduce volume in code; MAX98357A gain is fixed |
| Speaker hissing/noise | Power supply noise | Use battery power; add 100µF capacitor near amplifier |
| Mic records silence | WS/SCK swapped | Verify WS=39 (LRCLK) and SCK=40 (BCLK) |
| Both mic and speaker fail | I²S clock conflict | Check both I²S buses are configured independently |