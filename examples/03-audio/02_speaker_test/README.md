# Speaker Test

## Description
Play a test tone and audio samples through the MAX98357A speaker amplifier. This example introduces I²S audio output.

## Required Hardware
- OceanLabz AI Explorer S3 Board
- Speaker (4Ω–8Ω, up to 3W)

## Required Libraries
- None (uses built-in `esp32-hal-i2s.h` and `analogWrite`)

## Wiring
| Speaker Terminal | MAX98357A Terminal |
|------------------|:------------------:|
| Speaker (+) | SPK+ |
| Speaker (-) | SPK- |

## Steps
1. Connect speaker to SPK+/SPK- terminals
2. Upload `02_speaker_test.ino`
3. A tone will play through the speaker

## Expected Output
- Speaker plays a 1 kHz tone for 2 seconds
- Then a rising frequency sweep (200 Hz → 2 kHz)
- Repeats every 10 seconds
- Serial Monitor shows frequency information

## Troubleshooting
| Problem | Solution |
|---------|----------|
| No sound | Check speaker connected to SPK+/SPK- |
| Distorted audio | Reduce volume in code; check speaker impedance (4-8Ω) |
| Low volume | MAX98357A has fixed gain — use higher efficiency speaker |
| Hissing noise | Add 100µF capacitor near amplifier power pins |