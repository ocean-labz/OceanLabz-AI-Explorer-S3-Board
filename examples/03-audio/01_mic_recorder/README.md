# Microphone Recorder

## Description
Record audio from the INMP441 digital microphone and view the waveform in the Arduino Serial Plotter. This example introduces I²S audio input.

## Required Hardware
- OceanLabz AI Explorer S3 Board

## Required Libraries
- None (uses built-in `esp32-hal-i2s.h`)

## Wiring
No external wiring needed. INMP441 is onboard (WS=39, SCK=40, SD=41).

## Steps
1. Upload `01_mic_recorder.ino`
2. Open **Serial Plotter** (**Tools** → **Serial Plotter**) at 115200 baud
3. Speak into the microphone

## Expected Output
- Serial Plotter shows a waveform of the captured audio
- Silence shows a flat line; speaking shows amplitude peaks

## Troubleshooting
| Problem | Solution |
|---------|----------|
| Flat line | Check microphone is not obstructed; speak closer |
| Noisy signal | Power supply noise — use battery |
| Garbled plot | Wrong baud rate; set to 115200 |
| I²S init fails | Verify WS=39, SCK=40, SD=41 pins |