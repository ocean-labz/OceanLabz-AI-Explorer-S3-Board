# 📜 Scripts

Platform-specific helper scripts for setting up the development environment and automating common tasks.

## Structure

```
scripts/
├── windows/      # Windows (.bat, .ps1)
├── macos/        # macOS (.sh)
└── linux/        # Linux (.sh)
```

## Available Scripts (Planned)

| Script | Platform | Description |
|--------|----------|-------------|
| `install_drivers` | All | Install required USB/serial drivers |
| `setup_arduino_cli` | All | Install Arduino CLI + ESP32 core |
| `flash_bootloader` | All | Re-flash ESP32-S3 bootloader |
| `erase_flash` | All | Erase entire ESP32 flash memory |
| `batch_upload` | All | Upload firmware to multiple boards |

## Usage

### Windows
```cmd
scripts\windows\install_drivers.bat
```

### macOS / Linux
```bash
chmod +x scripts/macos/install_drivers.sh
./scripts/macos/install_drivers.sh
```

## Contributing Scripts

If you create a useful helper script, please submit a PR following the [Contribution Guidelines](../docs/contributing/CONTRIBUTING.md). Scripts should:
- Be well-commented
- Handle errors gracefully
- Support both verbose and silent modes
- Be cross-platform where possible (use conditionals for OS detection)