# Flashing Helpers

Scripts and utilities that simplify flashing the AI Explorer S3 board.

## Available Helpers

| File | Description |
|------|-------------|
| `flash_combined.bat` | Windows batch — flash combined firmware (bootloader + partitions + app) |
| `flash_combined.sh` | Linux/macOS shell — flash combined firmware |
| `erase_flash.bat` | Windows batch — full chip erase |
| `erase_flash.sh` | Linux/macOS shell — full chip erase |

## Dependencies

- [esptool](https://github.com/espressif/esptool) v4.0+ (`pip install esptool`)
- Python 3.8+

## Usage

```bash
# Full erase
esptool.py --chip esp32s3 --port COM3 erase_flash

# Flash combined firmware
esptool.py --chip esp32s3 --port COM3 --baud 921600 write_flash \
  0x0 factory_firmware.bin
```

> These helpers are companions to the [`releases/`](../../releases/) artifacts. Factory firmware binaries are stored under `releases/tools/factory_firmware/`.
