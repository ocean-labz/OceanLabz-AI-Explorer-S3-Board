# SD Card Test

## Description
Initialize the microSD card via SD_MMC in 1-bit mode, read card information, and perform basic file read/write operations.

## Required Hardware
- OceanLabz AI Explorer S3 Board
- microSD card (FAT32 formatted, Class 10 recommended)

## Required Libraries
- None (`SD_MMC.h` is built into ESP32 Arduino Core)

## Wiring
Insert microSD card into the onboard slot. No external wiring needed.

## Steps
1. Insert a FAT32-formatted microSD card
2. Upload `01_sdmmc_card_test.ino`
3. Open Serial Monitor at 115200 baud

## Expected Output
```
SD Card Test — OceanLabz AI Explorer S3
Card Mount Successful!
Card Size: 15936 MB
Card Type: SDHC
Filesystem: FAT32
Writing file: /test.txt... OK
Reading file: /test.txt... Hello, OceanLabz!
Removing file: /test.txt... OK
```

## Troubleshooting
| Problem | Solution |
|---------|----------|
| "Card Mount Failed" | Format as FAT32; try `SD_MMC.begin("/sdcard", true)` for 1-bit |
| "Card Mount Failed" | Clean card contacts; try different card |
| Size shows 0 | Card is not supported — use genuine SDHC/SDXC card |
| Write fails | Card is write-protected or full |