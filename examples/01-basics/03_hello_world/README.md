# 03 — Hello World

## Description

The **simplest possible "Hello World"** for the OceanLabz AI Explorer S3 Board. No external libraries, no external hardware, no GPIO pins — just the board, a USB cable, and the Arduino IDE.

This example teaches the **5 fundamental Arduino concepts** that every embedded developer needs:

| Concept | What it does |
|---------|-------------|
| `Serial.begin()` | Initialise serial communication at a given baud rate |
| `Serial.print()` / `Serial.println()` | Send text to the Serial Monitor |
| `Serial.available()` | Check if the user has typed something |
| `Serial.readStringUntil()` | Read a full line of text from the serial port |
| `millis()` | Get the number of milliseconds since the board was reset |

## Required Hardware

- OceanLabz AI Explorer S3 Board
- USB Type-C cable (data-capable)

## Required Libraries

- **None** — uses only built-in Arduino functions.

## Wiring

No external wiring required.

## Steps

1. Open [`03_hello_world.ino`](03_hello_world.ino) in Arduino IDE
2. Select **Board**: `ESP32S3 Dev Module`
3. Select **Port**: your board's COM port
4. Click **Upload**
5. Open **Serial Monitor** (**Tools → Serial Monitor**) at **115200 baud**

## Commands

Type any of these commands into the Serial Monitor and press **Enter**:

| Command   | Response |
|-----------|----------|
| `hello`   | The board replies "Hello back!" |
| `uptime`  | Prints how long the board has been running (h/m/s) |
| *(anything else)* | The board echoes back whatever you typed |

## Expected Output

After uploading, the Serial Monitor shows:

```
========================================
  OceanLabz AI Explorer S3 Board
  Example 03 — Hello World
========================================

Hello, World!
Compiled on: Jan 31 2024 at 14:30:00

Type something and press Enter —
the board will echo it back.
Type 'uptime' to see how long
the board has been running.

>
```

### Example interactions

```
> hello
  Hello back! 👋
> uptime
  Uptime: 42s
> How are you?
  You said: how are you?
> good morning board
  You said: good morning board
```

## Troubleshooting

| Problem | Solution |
|---------|----------|
| Serial Monitor shows nothing | Set baud rate to **115200** |
| Garbled text | Baud rate mismatch — both must be 115200 |
| Upload fails | Hold **BOOT** → press **RESET** → release **BOOT** (download mode) |
| No port appears | Try a different USB cable; install CP210x drivers if needed |

## Code Structure

```
03_hello_world.ino
├── setup()              → Init serial, print welcome banner
└── loop()               → Read serial input, dispatch actions
    ├── "hello"          → Print a friendly reply
    ├── "uptime"         → Calculate and print elapsed time
    └── (other text)     → Echo the input back to the user
```

## Learning Path

After this sketch works, move on to:
- [`01_oled_hello`](../../02-display/01_oled_hello/README.md) — display text on the I²C OLED screen
- [`01_mic_recorder`](../../03-audio/01_mic_recorder/README.md) — capture audio from the INMP441 microphone
