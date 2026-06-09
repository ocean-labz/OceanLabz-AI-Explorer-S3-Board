/**
 * @file    01_sdmmc_card_test.ino
 * @author  OceanLabz
 * @date    2026
 * @brief   Storage example for the OceanLabz AI Explorer S3 Board.
 *
 *          Initialises the onboard microSD card slot via SD_MMC in 1-bit
 *          mode, prints card information (type, size, filesystem), and
 *          performs a basic file write / read / delete cycle to verify
 *          read-write functionality.
 *
 *          Wiring:
 *            Insert a FAT32-formatted microSD card into the onboard slot.
 *            No external wiring is required.
 *
 *          Pin Mapping (SD_MMC 1-bit mode):
 *            CMD   → GPIO3
 *            CLK   → GPIO2
 *            D0    → GPIO46
 */

 // ============================================================
 // Includes
 // ============================================================
#include <SD_MMC.h>
#include <FS.h>

// ============================================================
// Pin Definitions — OceanLabz AI Explorer S3
// ============================================================
#define SD_CMD          3
#define SD_CLK          2
#define SD_D0          46

// ============================================================
// Test Parameters
// ============================================================
static const char* TEST_FILE = "/test.txt";
static const char* TEST_CONTENT = "Hello, OceanLabz!";

// ============================================================
// Helper — Card Type String
// ============================================================
static const char* cardTypeStr(uint8_t type) {
    switch (type) {
    case CARD_MMC:     return "MMC";
    case CARD_SD:      return "SDSC";
    case CARD_SDHC:    return "SDHC";
    case CARD_UNKNOWN:
    default:           return "UNKNOWN";
    }
}

// ============================================================
// Helper — Print card information
// ============================================================
static void printCardInfo() {
    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);   // MB
    uint64_t totalBytes = SD_MMC.totalBytes() / (1024 * 1024);
    uint64_t usedBytes = SD_MMC.usedBytes() / (1024 * 1024);

    Serial.print(F("  Card Size:       "));
    Serial.print(cardSize);
    Serial.println(F(" MB"));

    Serial.print(F("  Card Type:       "));
    Serial.println(cardTypeStr(SD_MMC.cardType()));

    Serial.print(F("  Total Space:     "));
    Serial.print(totalBytes);
    Serial.println(F(" MB"));

    Serial.print(F("  Used Space:      "));
    Serial.print(usedBytes);
    Serial.println(F(" MB"));

    // Determine FAT32 vs exFAT via sector size heuristic
    uint8_t sectorsPerCluster;
    if (SD_MMC.cardType() == CARD_SDHC || SD_MMC.cardType() == CARD_SD) {
        sectorsPerCluster = SD_MMC.totalBytes() / SD_MMC.cardSize();
        if (sectorsPerCluster <= 64) {
            Serial.println(F("  Filesystem:      FAT32"));
        }
        else {
            Serial.println(F("  Filesystem:      exFAT / FAT32"));
        }
    }
    else {
        Serial.println(F("  Filesystem:      FAT32 (assumed)"));
    }

    Serial.println();
}

// ============================================================
// Helper — Write a test file
// ============================================================
static bool writeTestFile() {
    Serial.print(F("  Writing file: "));
    Serial.print(TEST_FILE);
    Serial.print(F("... "));

    File file = SD_MMC.open(TEST_FILE, FILE_WRITE);
    if (!file) {
        Serial.println(F("FAILED"));
        return false;
    }

    size_t written = file.print(TEST_CONTENT);
    file.close();

    if (written == 0) {
        Serial.println(F("FAILED (0 bytes written)"));
        return false;
    }

    Serial.println(F("OK"));
    return true;
}

// ============================================================
// Helper — Read back the test file
// ============================================================
static bool readTestFile() {
    Serial.print(F("  Reading file:  "));
    Serial.print(TEST_FILE);
    Serial.print(F("... "));

    File file = SD_MMC.open(TEST_FILE, FILE_READ);
    if (!file) {
        Serial.println(F("FAILED"));
        return false;
    }

    String content = file.readString();
    content.trim();
    file.close();

    Serial.println(content);

    if (content != TEST_CONTENT) {
        Serial.print(F("  WARNING: Content mismatch. Expected \""));
        Serial.print(TEST_CONTENT);
        Serial.println(F("\""));
        return false;
    }

    return true;
}

// ============================================================
// Helper — Delete the test file
// ============================================================
static bool deleteTestFile() {
    Serial.print(F("  Removing file:  "));
    Serial.print(TEST_FILE);
    Serial.print(F("... "));

    if (!SD_MMC.exists(TEST_FILE)) {
        Serial.println(F("NOT FOUND — skipping"));
        return false;
    }

    if (!SD_MMC.remove(TEST_FILE)) {
        Serial.println(F("FAILED"));
        return false;
    }

    Serial.println(F("OK"));
    return true;
}

// ============================================================
// setup() — runs once on power-up / reset
// ============================================================
void setup() {
    Serial.begin(115200);
    delay(500);                         // allow serial to settle

    Serial.println();
    Serial.println(F("========================================"));
    Serial.println(F("  OceanLabz AI Explorer S3 Board"));
    Serial.println(F("  Example 01 — SDMMC Card Test"));
    Serial.println(F("========================================"));
    Serial.println();

    // --------------------------------------------------------
    // 1. Mount SD card (1-bit mode)
    // --------------------------------------------------------
    Serial.print(F("Initialising SD_MMC (1-bit mode)... "));

    // Set the SD_MMC pins explicitly for this board
    // CLK=GPIO2, CMD=GPIO3, D0=GPIO46  (1-bit mode)
    SD_MMC.setPins(SD_CLK, SD_CMD, SD_D0);

    if (!SD_MMC.begin("/sdcard", true)) {           // true = 1-bit mode
        Serial.println(F("Card Mount Failed!"));
        Serial.println();
        Serial.println(F("Troubleshooting:"));
        Serial.println(F("  - Ensure a FAT32-formatted microSD is inserted"));
        Serial.println(F("  - Try cleaning the card contacts"));
        Serial.println(F("  - Try a different card (Class 10 recommended)"));
        Serial.println(F("  - Check solder joints on the SD card slot"));
        Serial.println();
        Serial.println(F("Halting — no SD card available."));
        while (true) {
            delay(100);
        }
    }

    Serial.println(F("Card Mount Successful!"));
    Serial.println();

    // --------------------------------------------------------
    // 2. Print card information
    // --------------------------------------------------------
    printCardInfo();

    // --------------------------------------------------------
    // 3. File I/O test cycle
    // --------------------------------------------------------
    bool allPassed = true;

    // Write
    if (!writeTestFile()) {
        allPassed = false;
    }

    // Read
    if (!readTestFile()) {
        allPassed = false;
    }

    // Delete
    if (!deleteTestFile()) {
        allPassed = false;
    }

    // --------------------------------------------------------
    // 4. Summary
    // --------------------------------------------------------
    Serial.println();
    Serial.print(F("SD Card Test "));
    Serial.println(allPassed ? F("PASSED ✓") : F("FAILED — see warnings above"));
    Serial.println();

    // Optionally list the root directory
    Serial.println(F("Contents of root directory:"));
    File root = SD_MMC.open("/");
    if (root) {
        File entry = root.openNextFile();
        while (entry) {
            Serial.print(F("  "));
            Serial.print(entry.name());
            if (entry.isDirectory()) {
                Serial.println(F("/"));
            }
            else {
                Serial.print(F("  ("));
                Serial.print(entry.size());
                Serial.println(F(" bytes)"));
            }
            entry = root.openNextFile();
        }
        root.close();
    }
    Serial.println();

    Serial.println(F("Done. Insert a new card or reset to re-run the test."));
}

// ============================================================
// loop() — runs repeatedly
// ============================================================
void loop() {
    // Nothing to do — test runs once in setup()
    delay(1000);
}
