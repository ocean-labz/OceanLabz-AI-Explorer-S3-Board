# OLED Hello World

## Description
Display "Hello, OceanLabz!" and other text on the I²C OLED display. This example teaches I²C communication and basic display usage.

## Required Hardware
- OceanLabz AI Explorer S3 Board
- I²C OLED display (SSD1306 or SH1106, 128×64)

## Required Libraries
- **Adafruit SSD1306** by Adafruit (v2.5.7+)
- **Adafruit GFX** by Adafruit (v1.11.5+)

## Wiring
| OLED Pin | GPIO |
|----------|:----:|
| SDA | **43** |
| SCL | **44** |
| VCC | 3.3V |
| GND | GND |

> Most OLED modules have a default I²C address of 0x3C. If yours uses 0x3D, change `display.begin(SSD1306_SWITCHCAPVCC, 0x3D)` in the sketch.

## Steps
1. Install required libraries via Library Manager
2. Connect OLED to board as per wiring table
3. Open `01_oled_hello.ino` and upload
4. Observe text on the OLED

## Expected Output
- OLED shows: "OceanLabz AI" on line 1, "Explorer S3" on line 2
- Counter increments every second on line 3
- Serial Monitor prints debug info at 115200 baud

## Troubleshooting
| Problem | Solution |
|---------|----------|
| No display | Run I²C scanner to find address; try 0x3C and 0x3D |
| Garbled text | Update Adafruit SSD1306 and GFX libraries |
| Dim display | Increase contrast in code |
| Wrong pins | Verify SDA=43, SCL=44 |