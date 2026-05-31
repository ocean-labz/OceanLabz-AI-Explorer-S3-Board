# Voice Assistant Demo

## Description
A basic voice command recognition demo using the INMP441 microphone. Detects simple voice commands (e.g., "LED on", "LED off") and triggers onboard actions. This example introduces voice activity detection and basic pattern matching.

## Required Hardware
- OceanLabz AI Explorer S3 Board

## Required Libraries
- None (uses built-in I²S and WiFi libraries)

## Wiring
No external wiring needed. Microphone is onboard.

## Steps
1. Upload `03_voice_assistant_demo.ino`
2. Open Serial Monitor at 115200 baud
3. Speak commands clearly into the microphone

## Expected Output
- Serial Monitor shows detected commands
- RGB LED responds to voice commands
- "LED ON" → LED lights up
- "LED OFF" → LED turns off

## Troubleshooting
| Problem | Solution |
|---------|----------|
| No commands detected | Speak closer to microphone; reduce background noise |
| False triggers | Adjust sensitivity threshold in sketch |
| Delay in response | Voice processing takes ~500ms — normal for ESP32-S3 |