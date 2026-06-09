/**
 * @file    02_sdmmc_logger.ino
 * @author  OceanLabz
 * @date    2026
 * @brief   Storage example for the OceanLabz AI Explorer S3 Board.
 *
 *          Logs sensor data (free heap, uptime, CPU temperature) to a CSV
 *          file on the microSD card at a fixed interval.  Demonstrates
 *          periodic data logging with file append operations.
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
#include "driver/temperature_sensor.h"

// ============================================================
// Pin Definitions — OceanLabz AI Explorer S3
// ============================================================
#define SD_CMD          3
#define SD_CLK          2
#define SD_D0          46

// ============================================================
// Logging Parameters
// ============================================================
static const char* CSV_FILE = "/data.csv";         ///< CSV path on SD
static const char* CSV_HEADER = "Timestamp,FreeHeap,Uptime_s,CpuTemp_C";

static const uint32_t LOG_INTERVAL_MS = 5000;          ///< 5 s between samples
static const uint32_t LOG_DURATION_MS = 30000;         ///< log for 30 s total

// ============================================================
// Globals
// ============================================================
static temperature_sensor_handle_t temp_sensor = NULL;

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
    switch (SD_MMC.cardType()) {
    case CARD_MMC:  Serial.println(F("MMC"));   break;
    case CARD_SD:   Serial.println(F("SDSC"));  break;
    case CARD_SDHC: Serial.println(F("SDHC"));  break;
    default:        Serial.println(F("UNKNOWN"));
    }

    Serial.print(F("  Total Space:     "));
    Serial.print(totalBytes);
    Serial.println(F(" MB"));

    Serial.print(F("  Used Space:      "));
    Serial.print(usedBytes);
    Serial.println(F(" MB"));

    Serial.print(F("  Filesystem:      "));
    if (SD_MMC.cardType() == CARD_SDHC || SD_MMC.cardType() == CARD_SD) {
        uint8_t spc = SD_MMC.totalBytes() / SD_MMC.cardSize();
        Serial.println((spc <= 64) ? F("FAT32") : F("exFAT / FAT32"));
    }
    else {
        Serial.println(F("FAT32 (assumed)"));
    }
    Serial.println();
}

// ============================================================
// Helper — Initialise the internal temperature sensor
// ============================================================
static bool initTempSensor() {
    temperature_sensor_config_t config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(
        -10,   // min range °C
        80     // max range °C
    );
    esp_err_t err = temperature_sensor_install(&config, &temp_sensor);
    if (err != ESP_OK) {
        Serial.print(F("  Temperature sensor install FAILED: 0x"));
        Serial.println(err, HEX);
        return false;
    }

    err = temperature_sensor_enable(temp_sensor);
    if (err != ESP_OK) {
        Serial.print(F("  Temperature sensor enable FAILED: 0x"));
        Serial.println(err, HEX);
        temperature_sensor_uninstall(temp_sensor);
        temp_sensor = NULL;
        return false;
    }

    return true;
}

// ============================================================
// Helper — Read CPU temperature (°C)
// ============================================================
static float readCpuTemp() {
    if (temp_sensor == NULL) return 0.0f;

    float temp = 0.0f;
    esp_err_t err = temperature_sensor_get_celsius(temp_sensor, &temp);
    if (err != ESP_OK) {
        Serial.print(F("  Temperature read FAILED: 0x"));
        Serial.println(err, HEX);
        return 0.0f;
    }
    return temp;
}

// ============================================================
// Helper — Append one CSV row to the log file
// ============================================================
static bool appendLogRow(uint32_t timestamp_ms, uint32_t free_heap,
    uint32_t uptime_s, float cpu_temp) {
    File file = SD_MMC.open(CSV_FILE, FILE_APPEND);
    if (!file) {
        Serial.println(F("  ERROR: Failed to open file for append"));
        return false;
    }

    file.printf("%lu,%lu,%lu,%.1f\n",
        (unsigned long)timestamp_ms,
        (unsigned long)free_heap,
        (unsigned long)uptime_s,
        cpu_temp);
    file.close();
    return true;
}

// ============================================================
// setup() — runs once on power-up / reset
// ============================================================
void setup() {
    Serial.begin(115200);
    delay(500);                             // allow serial to settle

    Serial.println();
    Serial.println(F("========================================"));
    Serial.println(F("  OceanLabz AI Explorer S3 Board"));
    Serial.println(F("  Example 02 — SDMMC Data Logger"));
    Serial.println(F("========================================"));
    Serial.println();

    // --------------------------------------------------------
    // 1. Mount SD card (1-bit mode)
    // --------------------------------------------------------
    Serial.print(F("Initialising SD_MMC (1-bit mode)... "));

    // CLK=GPIO2, CMD=GPIO3, D0=GPIO46 (1-bit mode)
    SD_MMC.setPins(SD_CLK, SD_CMD, SD_D0);

    if (!SD_MMC.begin("/sdcard", true)) {   // true = 1-bit mode
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
    // 3. Initialise temperature sensor
    // --------------------------------------------------------
    Serial.print(F("Initialising internal temperature sensor... "));
    if (initTempSensor()) {
        Serial.println(F("OK"));
    }
    else {
        Serial.println(F("SKIPPED — temperature will read 0.0"));
    }
    Serial.println();

    // --------------------------------------------------------
    // 4. Write CSV header (overwrite any previous file)
    // --------------------------------------------------------
    Serial.print(F("Writing CSV header to "));
    Serial.print(CSV_FILE);
    Serial.print(F("... "));

    {
        File file = SD_MMC.open(CSV_FILE, FILE_WRITE);
        if (!file) {
            Serial.println(F("FAILED"));
            Serial.println(F("Halting — cannot create log file."));
            while (true) {
                delay(100);
            }
        }
        file.println(CSV_HEADER);
        file.close();
    }

    Serial.println(F("OK"));
    Serial.println();

    // --------------------------------------------------------
    // 5. Start logging loop
    // --------------------------------------------------------
    Serial.print(F("Logging data to "));
    Serial.print(CSV_FILE);
    Serial.print(F(" every "));
    Serial.print(LOG_INTERVAL_MS / 1000);
    Serial.print(F("s for "));
    Serial.print(LOG_DURATION_MS / 1000);
    Serial.println(F("s..."));
    Serial.println();
}

// ============================================================
// loop() — runs repeatedly
// ============================================================
void loop() {
    static uint32_t lastLogMs = 0;
    static uint32_t startMs = millis();   // capture once on first call

    uint32_t nowMs = millis();

    // --------------------------------------------------------
    // 1. Check if the logging period has expired
    // --------------------------------------------------------
    if ((nowMs - startMs) >= LOG_DURATION_MS) {
        Serial.println();
        Serial.println(F("Logging period complete."));
        Serial.print(F("Remove the SD card and check "));
        Serial.print(CSV_FILE);
        Serial.println(F(" on your computer."));

        // Cleanup temperature sensor
        if (temp_sensor != NULL) {
            temperature_sensor_disable(temp_sensor);
            temperature_sensor_uninstall(temp_sensor);
            temp_sensor = NULL;
        }

        // Halt
        while (true) {
            delay(1000);
        }
    }

    // --------------------------------------------------------
    // 2. Check if it's time for the next sample
    // --------------------------------------------------------
    if ((nowMs - lastLogMs) >= LOG_INTERVAL_MS) {
        lastLogMs = nowMs;

        uint32_t elapsedMs = nowMs - startMs;
        uint32_t freeHeap = (uint32_t)esp_get_free_heap_size();
        uint32_t uptimeS = elapsedMs / 1000;
        float    cpuTemp = readCpuTemp();

        Serial.printf("  [%4lu s] FreeHeap=%lu  CpuTemp=%.1f°C  -> ",
            (unsigned long)uptimeS,
            (unsigned long)freeHeap,
            cpuTemp);

        if (appendLogRow(elapsedMs, freeHeap, uptimeS, cpuTemp)) {
            Serial.println(F("logged"));
        }
        else {
            Serial.println(F("FAILED"));
        }
    }
}
