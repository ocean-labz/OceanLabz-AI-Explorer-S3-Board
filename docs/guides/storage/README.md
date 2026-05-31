# 💾 SD Card Guide — SD_MMC Storage

This guide covers using the microSD card slot via the SD_MMC (4-bit) interface.

---

## Overview

The OceanLabz AI Explorer S3 Board uses the **SD_MMC** interface in **1-bit mode** (default) or **4-bit mode** for faster access. The SD_MMC interface offers significantly faster read/write speeds than SPI-based SD card access.

### Pin Mapping
| SD_MMC Pin | GPIO | Notes |
|------------|:----:|-------|
| CMD | **3** | Command line |
| CLK | **2** | Clock line (up to 40MHz) |
| D0 | **46** | Data line 0 |

> **4-bit mode** would require D1, D2, D3 — these are not available on this board due to pin conflicts with other peripherals. Use **1-bit mode**.

---

## SD Card Specifications

| Requirement | Value |
|-------------|-------|
| Card Type | microSD (SDSC, SDHC, SDXC) |
| Format | FAT32 (recommended), exFAT |
| Max Capacity | 32GB (FAT32), 64GB+ (exFAT) |
| Speed Class | Class 10 or UHS-I recommended |

> ⚠️ Use a **genuine** branded SD card. Counterfeit cards often fail unpredictably.

---

## Formatting

### Recommended: FAT32
For cards up to 32GB, use **FAT32** with default allocation size.

**Windows:**
1. Insert card → Right-click drive → **Format**
2. File system: **FAT32**
3. Allocation unit size: **Default**
4. Quick format: ✅

**macOS:**
1. Open **Disk Utility**
2. Select the card → **Erase**
3. Format: **MS-DOS (FAT32)**
4. Scheme: **Master Boot Record**

**Linux:**
```bash
sudo mkfs.vfat -F 32 /dev/sdX1
```

### For Cards Larger Than 32GB
Use **exFAT**. The ESP32 Arduino Core supports exFAT from v2.0.14+.

---

## Required Library

The SD_MMC library is built into the ESP32 Arduino Core — no additional installation required.

```cpp
#include <SD_MMC.h>
```

---

## Basic Usage

### Initialization (1-bit mode)
```cpp
#include <SD_MMC.h>

void setup() {
    Serial.begin(115200);
    
    if (!SD_MMC.begin()) {
        Serial.println("Card Mount Failed");
        return;
    }
    
    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %llu MB\n", cardSize);
}
```

### File Operations
```cpp
// Write a file
File file = SD_MMC.open("/data.txt", FILE_WRITE);
file.println("Hello, OceanLabz!");
file.close();

// Read a file
file = SD_MMC.open("/data.txt", FILE_READ);
while (file.available()) {
    Serial.write(file.read());
}
file.close();
```

---

## Examples

| Example | Description | Level |
|---------|-------------|:-----:|
| [SD Card Test](../../examples/04-storage/01_sdmmc_card_test/) | Initialize, read card info, basic file I/O | Beginner |
| [SD Card Logger](../../examples/04-storage/02_sdmmc_logger/) | Log sensor data to CSV file | Intermediate |

---

## Troubleshooting

| Problem | Likely Cause | Solution |
|---------|--------------|----------|
| "Card Mount Failed" | Card not formatted as FAT32 | Re-format as FAT32 |
| "Card Mount Failed" | Poor connection | Remove and re-insert card; check for debris |
| "Card Mount Failed" | Wrong SD_MMC mode | Use `SD_MMC.begin("/sdcard", true)` for 1-bit mode |
| Read/write fails | Card is counterfeit | Try a genuine SanDisk or Samsung card |
| "File not found" | Wrong path | Paths are case-sensitive: `/data.txt` not `/Data.txt` |
| Slow write speed | Card is slow class | Use Class 10 or better |
| "FlashFatFS Error" | Card not unmounted before removal | Always unmount: `SD_MMC.end()` before removing |

---

## Performance

| Operation | 1-bit Mode (typical) |
|-----------|:--------------------:|
| Read (sequential) | ~8-10 MB/s |
| Write (sequential) | ~3-5 MB/s |
| Write (4KB random) | ~0.5 MB/s |

Performance depends on card quality. For audio recording, a Class 10 card is strongly recommended.