# Board Test Tools

Integration and factory-test sketches for validating AI Explorer S3 hardware.

## Available Tools

| Tool | Description |
|------|-------------|
| `test_all_peripherals.ino` | Comprehensive test of every onboard peripheral |
| `test_tft_colors.ino` | TFT colour bars + touch test (if applicable) |
| `test_sd_speed.ino` | SD_MMC read/write speed benchmark |
| `test_camera_focus.ino` | Camera capture and focus test |

## Usage

1. Open the `.ino` test sketch in Arduino IDE
2. Select **Board:** `ESP32S3 Dev Module` with PSRAM enabled
3. Upload and open the Serial Monitor (115200 baud)
4. Follow on-screen prompts to test each peripheral

> **Note:** These tools are intended for **manufacturing / QA testing**. Most users should refer to the [`examples/`](../../examples/) folder for learning and prototyping.
