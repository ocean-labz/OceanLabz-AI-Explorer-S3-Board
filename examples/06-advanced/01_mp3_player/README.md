# MP3 / WAV Player

## Description
Play MP3 and WAV audio files stored on the microSD card through the MAX98357A speaker amplifier. This example uses the raw ESP32 I²S driver for audio output and the **minimp3** library (single-header, public domain) for MP3 decoding — **no third-party Arduino library is required**, avoiding version compatibility issues.

The player automatically scans the SD card root directory for `.mp3` and `.wav` files, builds a playlist sorted alphabetically, and plays them sequentially. After the last track finishes, playback loops back to the first track.

## Required Hardware
- OceanLabz AI Explorer S3 Board
- microSD card (FAT32 formatted)
- Speaker (4Ω–8Ω, up to 3W)
- MP3 or WAV audio files

## Required Libraries
- **None** — uses the built-in ESP32 I²S driver (`driver/i2s.h`), `SD_MMC`, and the bundled `minimp3.h` (public domain, single-header MP3 decoder)

## Supported Audio Formats

### MP3
| Property | Notes |
|----------|-------|
| Bitrates | Any (128 kbps recommended for quality) |
| Channels | Mono and stereo (stereo downmixed to mono) |
| Sample rates | 8000 – 48000 Hz |
| Layers | MPEG-1 Layer III, MPEG-2 Layer III |

### WAV
| Format | Notes |
|--------|-------|
| 16-bit PCM mono | Direct playback (recommended) |
| 16-bit PCM stereo | Left channel extracted automatically |
| 8-bit PCM mono | Auto-converted to 16-bit signed |
| 8-bit PCM stereo | Left channel extracted, converted to 16-bit |
| Sample rates | 8000 – 48000 Hz |

## Wiring
Connect speaker to SPK+/SPK- terminals. Insert SD card with audio files.

## Steps
1. Copy MP3/WAV files to root of SD card (e.g., `/song1.mp3`, `/track2.wav`)
2. Upload `01_mp3_player.ino`
3. Open Serial Monitor at 115200 baud

## Expected Output
- Serial Monitor lists all audio files found on the SD card
- For each track, shows format-specific info (sample rate, channels, bitrate, etc.)
- Speaker plays each file sequentially
- Automatically advances to the next file when the current one ends
- Loops back to the first track after the last one finishes

## Troubleshooting
| Problem | Solution |
|---------|----------|
| No sound | Check speaker connection; verify audio files on SD card |
| "No audio files found" | Copy .mp3 or .wav files to the root directory (not subfolders) |
| "Not PCM" error | Only PCM WAV files are supported — re-export as 16-bit PCM |
| Garbled MP3 audio | Use 128 kbps bitrate; avoid VBR with very high peaks |
| Garbled WAV audio | Use 16-bit mono WAV at 44100 Hz for best results |
| "SD init failed" | Format SD as FAT32; check insertion |
| I²S install error | Another sketch may be using I²S port 1 — restart the board |
| "Cannot open file" | Ensure files are in root directory, not in subfolders |
| Stack overflow crash | The bundled `minimp3.h` has been patched — the `mp3dec_scratch_t` inside `mp3dec_decode_frame()` is declared `static` so the ~20 KB struct lives in global/BSS memory instead of the task stack. If you replace `minimp3.h` with a fresh copy, re-apply this one-line patch or the crash will recur. |

## minimp3.h Patch (ESP32 Stack Fix)
The upstream `minimp3.h` places a ~20 KB `mp3dec_scratch_t` struct on the call stack inside `mp3dec_decode_frame()`. This exceeds the ESP32's task stack budget and causes a "Stack canary watchpoint triggered" crash.

**Patch applied** (line 1718 of `minimp3.h`):
```c
// Before (upstream):
    mp3dec_scratch_t scratch;

// After (ESP32 patch):
    static mp3dec_scratch_t scratch;  /* ESP32 fix: moved off stack to global/BSS (~20 KB) */
```

This is safe because `mp3dec_decode_frame()` is only called from the single playback task. If you update `minimp3.h` to a newer upstream version, you must re-apply this patch.

## Credits
- **minimp3** — https://github.com/lieff/minimp3 (public domain, CC0 1.0)
