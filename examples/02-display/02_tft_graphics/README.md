# TFT Graphics

## Description
Draw shapes, text, and bitmap images on an SPI TFT display using the TFT_eSPI library. This example introduces the SPI display interface and graphics primitives.

## Required Hardware
- OceanLabz AI Explorer S3 Board
- SPI TFT display (ILI9341 240×320 recommended)

## Required Libraries
- **TFT_eSPI** by Bodmer (v2.5.0+)

## Wiring
| TFT Pin | GPIO |
|---------|:----:|
| BL | **42** |
| MOSI | **44** |
| CLK | **43** |
| DC | **38** |
| RST | **1** |
| CS | **45** |
| VCC | 3.3V |
| GND | GND |

### TFT_eSPI Configuration
Copy the provided `User_Setup.h` from this folder to:
```
Arduino/libraries/TFT_eSPI/User_Setup.h
```

## Steps
1. Install TFT_eSPI via Library Manager
2. Configure `User_Setup.h` with the correct driver (ILI9341) and pins
3. Connect TFT display
4. Upload `02_tft_graphics.ino`

## Expected Output
- TFT shows: colorful shapes (rectangles, circles, triangles), text, and a color gradient
- Display updates every 2 seconds to show different patterns

## Troubleshooting
| Problem | Solution |
|---------|----------|
| White screen | User_Setup.h missing; verify driver selection |
| Backlight off | Set GPIO42 HIGH; check BL pin |
| Wrong colors | Set `TFT_INVERSION_ON` or `TFT_INVERSION_OFF` in User_Setup.h |
| Flickering | Check CS=45 is correct; no other peripheral using same pins |