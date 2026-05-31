# SD Card Logger

## Description
Log sensor data (free heap, uptime, CPU temperature) to a CSV file on the microSD card. This example demonstrates periodic data logging with file append operations.

## Required Hardware
- OceanLabz AI Explorer S3 Board
- microSD card (FAT32 formatted)

## Required Libraries
- None (built-in SD_MMC)

## Wiring
Insert microSD card into onboard slot.

## Steps
1. Insert FAT32-formatted microSD card
2. Upload `02_sdmmc_logger.ino`
3. Open Serial Monitor at 115200 baud

## Expected Output
```
Logging data to /data.csv every 5 seconds...
```
After 30 seconds, remove SD card and check `data.csv` on your computer:
```csv
Timestamp,FreeHeap,Uptime_s,CpuTemp_C
0,285432,5,42.5
5000,285120,10,43.1
...
```

## Troubleshooting
| Problem | Solution |
|---------|----------|
| File not created | Check card insertion; format as FAT32 |
| Data not appended | Ensure `FILE_APPEND` mode is used |
| CSV corrupted | Check newlines — use `file.println()` |