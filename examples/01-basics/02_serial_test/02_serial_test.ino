/**
 * @file    02_serial_test.ino
 * @author  OceanLabz
 * @date    2024
 * @brief   Second example for the OceanLabz AI Explorer S3 Board.
 *
 *          Dumps complete system information to the Serial console
 *          and provides an interactive command prompt for exploring
 *          the board's capabilities.
 *
 *          Commands (type into Serial Monitor):
 *            help    — show this help
 *            info    — print full system info again
 *            psram   — test PSRAM read/write
 *            flash   — show flash chip info
 *            wifi    — print WiFi MAC address
 *            led on  — turn RGB LED green
 *            led off — turn RGB LED off
 *            blink   — blink LED 5 times
 *            heap    — show current free heap
 *            restart — soft-reset the board
 *            test    — run quick peripheral sanity check
 */

 // ============================================================
 // Hardware Configuration
 // ============================================================
#define LED_PIN         14        // Onboard RGB LED (WS2812B)
#define NUM_LEDS         1

// ============================================================
// Includes
// ============================================================
#include <Adafruit_NeoPixel.h>
#include <esp_chip_info.h>
#include <esp_flash.h>
#include <esp_mac.h>
#include <esp_system.h>
#include <esp_psram.h>
#include <esp_spi_flash.h>
#include <soc/soc.h>
#include <soc/efuse_reg.h>
#include <rom/rtc.h>

// ============================================================
// Global Objects
// ============================================================
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// ============================================================
// Constants
// ============================================================
static const unsigned long STATUS_INTERVAL_MS = 10000;  // auto-status every 10 s
static const size_t        SERIAL_BUFFER_SIZE = 64;     // command input buffer
static const uint8_t       BRIGHTNESS = 50;     // safe desktop brightness

// ============================================================
// setup() — runs once
// ============================================================
void setup() {
    // --- Init Serial ---
    Serial.begin(115200);
    // Allow CDC-ACM to enumerate on some OSes
    delay(800);
    Serial.println();

    // --- Init NeoPixel ---
    strip.begin();
    strip.setBrightness(BRIGHTNESS);
    strip.show();                     // start OFF

    // --- Start-up Banner ---
    printBanner();
    printSystemInfo();

    Serial.println();
    Serial.println(F("Type 'help' and press Enter for available commands."));
    Serial.println();
    showPrompt();
}

// ============================================================
// loop() — runs forever
// ============================================================
void loop() {
    // --- Non-blocking auto-status every 10 seconds ---
    static unsigned long lastStatus = 0;
    if (millis() - lastStatus >= STATUS_INTERVAL_MS) {
        lastStatus = millis();
        printStatusLine();
        showPrompt();
    }

    // --- Process incoming serial commands ---
    processSerialInput();
}

// ============================================================
// Serial Command Processing
// ============================================================
static char cmdBuffer[SERIAL_BUFFER_SIZE];
static uint8_t cmdIndex = 0;

static void processSerialInput() {
    while (Serial.available() > 0) {
        char c = (char)Serial.read();

        if (c == '\n' || c == '\r') {
            // End of command
            if (cmdIndex > 0) {
                cmdBuffer[cmdIndex] = '\0';
                executeCommand(cmdBuffer);
                cmdIndex = 0;
                showPrompt();
            }
        }
        else if (c == '\b' || c == 0x7F) {
            // Backspace
            if (cmdIndex > 0) {
                cmdIndex--;
                Serial.print(F("\b \b"));
            }
        }
        else if (cmdIndex < SERIAL_BUFFER_SIZE - 1) {
            // Regular character
            // Convert to lowercase for case-insensitive matching
            if (c >= 'A' && c <= 'Z') c += 32;
            cmdBuffer[cmdIndex++] = c;
            Serial.print(c);        // local echo
        }
    }
}

// ============================================================
// Command Dispatcher
// ============================================================
static void executeCommand(const char* cmd) {
    Serial.println();

    if (strcmp(cmd, "help") == 0) {
        cmdHelp();
    }
    else if (strcmp(cmd, "info") == 0) {
        printSystemInfo();
    }
    else if (strcmp(cmd, "psram") == 0) {
        cmdPsram();
    }
    else if (strcmp(cmd, "flash") == 0) {
        cmdFlash();
    }
    else if (strcmp(cmd, "wifi") == 0) {
        cmdWifi();
    }
    else if (strcmp(cmd, "led on") == 0) {
        setLedColor(0, 255, 0);          // green
        Serial.println(F("LED: ON (green)"));
    }
    else if (strcmp(cmd, "led off") == 0) {
        setLedColor(0, 0, 0);            // off
        Serial.println(F("LED: OFF"));
    }
    else if (strcmp(cmd, "blink") == 0) {
        cmdBlink();
    }
    else if (strcmp(cmd, "heap") == 0) {
        Serial.print(F("Free heap:  "));
        Serial.print(ESP.getFreeHeap());
        Serial.println(F(" bytes"));

        Serial.print(F("Free PSRAM: "));
        Serial.print(ESP.getFreePsram());
        Serial.println(F(" bytes"));

        Serial.print(F("Min free    "));
        Serial.print(ESP.getMinFreeHeap());
        Serial.println(F(" bytes"));
    }
    else if (strcmp(cmd, "restart") == 0) {
        Serial.println(F("Restarting in 1 second..."));
        setLedColor(255, 0, 0);          // red = about to restart
        delay(1000);
        ESP.restart();
    }
    else if (strcmp(cmd, "test") == 0) {
        cmdTest();
    }
    else if (strlen(cmd) == 0) {
        // Empty line — do nothing
    }
    else {
        Serial.print(F("Unknown command: \""));
        Serial.print(cmd);
        Serial.println(F("\". Type 'help' for available commands."));
    }
}

// ============================================================
// Command Implementations
// ============================================================

/**
 * Print the help menu.
 */
static void cmdHelp() {
    Serial.println(F("╔══════════════════════════════════════════╗"));
    Serial.println(F("║        Available Commands                ║"));
    Serial.println(F("╠══════════════════════════════════════════╣"));
    Serial.println(F("║  help      — show this help              ║"));
    Serial.println(F("║  info      — print full system info      ║"));
    Serial.println(F("║  psram     — test PSRAM read/write       ║"));
    Serial.println(F("║  flash     — show flash chip details     ║"));
    Serial.println(F("║  wifi      — print WiFi MAC address      ║"));
    Serial.println(F("║  led on    — turn RGB LED green          ║"));
    Serial.println(F("║  led off   — turn RGB LED off            ║"));
    Serial.println(F("║  blink     — blink LED 5 times           ║"));
    Serial.println(F("║  heap      — show free heap / PSRAM      ║"));
    Serial.println(F("║  restart   — soft-reset the board        ║"));
    Serial.println(F("║  test      — run peripheral sanity check ║"));
    Serial.println(F("╚══════════════════════════════════════════╝"));
}

/**
 * PSRAM read/write test.
 */
static void cmdPsram() {
#if CONFIG_SPIRAM
    Serial.println(F("PSRAM:  detected"));
    Serial.print(F("  Total size: "));
    Serial.print(ESP.getPsramSize() / 1024);
    Serial.println(F(" KB"));

    // Allocate a test buffer in PSRAM
    size_t testSize = 1024 * 128;           // 128 KB
    uint8_t* buf = (uint8_t*)ps_calloc(testSize, 1);
    if (buf == NULL) {
        Serial.println(F("  Error: could not allocate test buffer!"));
        return;
    }

    // Write pattern
    uint32_t t0 = micros();
    for (size_t i = 0; i < testSize; i++) {
        buf[i] = (uint8_t)(i & 0xFF);
    }
    uint32_t t1 = micros();

    // Verify pattern
    bool ok = true;
    for (size_t i = 0; i < testSize; i++) {
        if (buf[i] != (uint8_t)(i & 0xFF)) {
            ok = false;
            break;
        }
    }
    uint32_t t2 = micros();

    if (ok) {
        float writeTime = (t1 - t0) / 1000.0f;
        float readTime = (t2 - t1) / 1000.0f;
        Serial.print(F("  Write: "));
        Serial.print(writeTime, 1);
        Serial.print(F(" ms ("));
        Serial.print((testSize / 1024.0f) / (writeTime / 1000.0f), 0);
        Serial.println(F(" KB/s)"));
        Serial.print(F("  Verify: "));
        Serial.print(readTime, 1);
        Serial.print(F(" ms ("));
        Serial.print((testSize / 1024.0f) / (readTime / 1000.0f), 0);
        Serial.println(F(" KB/s)"));
        Serial.println(F("  Result:  PASS"));
    }
    else {
        Serial.println(F("  Result:  FAIL — data mismatch!"));
    }

    free(buf);
    Serial.print(F("  Free:   "));
    Serial.print(ESP.getFreePsram() / 1024);
    Serial.println(F(" KB"));
#else
    Serial.println(F("PSRAM:  not enabled in this build config"));
    Serial.println(F("  Set Tools → PSRAM → OPI PSRAM and re-upload."));
#endif
}

/**
 * Flash chip information.
 */
static void cmdFlash() {
    uint32_t flashSize = 0;
    esp_flash_get_size(NULL, &flashSize);

    Serial.println(F("Flash Chip:"));
    Serial.print(F("  Size:  "));
    Serial.print(flashSize / (1024 * 1024));
    Serial.println(F(" MB"));

    // Flash chip model
    esp_flash_t* flash = NULL;
    if (esp_flash_init(&flash) == ESP_OK && flash != NULL) {
        const char* model = "Unknown";
        switch (flash->chip_id) {
        case 0x5E: model = "GD25Q128"; break;
        case 0x17: model = "W25Q128JV"; break;
        case 0x20: model = "XM25Q128"; break;
        case 0xC8: model = "EN25Q128"; break;
        default: model = "Generic"; break;
        }
        Serial.print(F("  Model: "));
        Serial.println(model);

        Serial.print(F("  Mode:  "));
        if (flash->chip_drv->io_mode == 2) Serial.print(F("QIO"));
        else if (flash->chip_drv->io_mode == 1) Serial.print(F("DIO"));
        else Serial.print(F("Standard SPI"));
        Serial.println();
    }

    // Partition info
    Serial.println(F("  Partitions:"));
    esp_partition_iterator_t it = esp_partition_find(ESP_PARTITION_TYPE_ANY,
        ESP_PARTITION_SUBTYPE_ANY,
        NULL);
    while (it) {
        const esp_partition_t* part = esp_partition_get(it);
        Serial.print(F("    0x"));
        Serial.print(part->address, HEX);
        Serial.print(F("  size="));
        Serial.print(part->size / 1024);
        Serial.print(F(" KB  \""));
        Serial.print(part->label);
        Serial.print(F("\""));
        if (part->type == ESP_PARTITION_TYPE_APP) {
            Serial.print(F(" [app"));
            if (part->subtype == ESP_PARTITION_SUBTYPE_APP_FACTORY) {
                Serial.print(F(", factory]"));
            }
            else if (part->subtype == ESP_PARTITION_SUBTYPE_APP_OTA_0) {
                Serial.print(F(", OTA0]"));
            }
        }
        else if (part->type == ESP_PARTITION_TYPE_DATA) {
            Serial.print(F(" [data]"));
        }
        Serial.println();
        it = esp_partition_next(it);
    }
    esp_partition_iterator_release(it);

    // Sketch size
    Serial.print(F("  Sketch size: "));
    Serial.print(ESP.getSketchSize() / 1024);
    Serial.print(F(" / "));
    Serial.print(ESP.getFreeSketchSpace() / 1024);
    Serial.println(F(" KB used"));
}

/**
 * Print WiFi MAC address.
 */
static void cmdWifi() {
    uint8_t mac[8] = { 0 };
    if (esp_read_mac(mac, ESP_MAC_WIFI_STA) == ESP_OK) {
        Serial.print(F("WiFi STA MAC: "));
        for (int i = 0; i < 6; i++) {
            if (mac[i] < 0x10) Serial.print(F("0"));
            Serial.print(mac[i], HEX);
            if (i < 5) Serial.print(F(":"));
        }
        Serial.println();

        if (esp_read_mac(mac, ESP_MAC_WIFI_SOFTAP) == ESP_OK) {
            Serial.print(F("WiFi AP  MAC: "));
            for (int i = 0; i < 6; i++) {
                if (mac[i] < 0x10) Serial.print(F("0"));
                Serial.print(mac[i], HEX);
                if (i < 5) Serial.print(F(":"));
            }
            Serial.println();
        }
    }
    else {
        Serial.println(F("WiFi:  could not read MAC address"));
    }
}

/**
 * Blink the RGB LED N times.
 */
static void cmdBlink() {
    const int N = 5;
    Serial.print(F("Blinking "));
    Serial.print(N);
    Serial.println(F(" times..."));

    for (int i = 0; i < N; i++) {
        setLedColor(0, 255, 0);          // green on
        delay(200);
        setLedColor(0, 0, 0);            // off
        delay(150);
    }
    Serial.println(F("Done."));
}

/**
 * Quick peripheral sanity check.
 */
static void cmdTest() {
    Serial.println(F("Running peripheral sanity check..."));
    Serial.println();

    // 1. RGB LED
    Serial.print(F("1. RGB LED ... "));
    setLedColor(0, 255, 0);
    delay(300);
    setLedColor(0, 0, 0);
    Serial.println(F("PASS"));

    // 2. PSRAM
    Serial.print(F("2. PSRAM  ... "));
#if CONFIG_SPIRAM
    void* p = ps_malloc(1024);
    if (p != NULL) {
        free(p);
        Serial.println(F("PASS"));
    }
    else {
        Serial.println(F("FAIL (allocation failed)"));
    }
#else
    Serial.println(F("SKIP (PSRAM not enabled)"));
#endif

    // 3. Internal ADC (read a pin to prove ADC works)
    Serial.print(F("3. ADC    ... "));
    int adcVal = analogRead(4);           // GPIO4 (camera SIOD) is ADC-capable
    if (adcVal >= 0 && adcVal <= 4095) {
        Serial.print(F("PASS (value="));
        Serial.print(adcVal);
        Serial.println(F(")"));
    }
    else {
        Serial.println(F("PASS (out of range)"));
    }

    // 4. Chip temperature
    Serial.print(F("4. Temp   ... "));
    float temp = temperatureRead();
    Serial.print(temp, 1);
    Serial.println(F(" °C (internal sensor)"));

    // 5. Free heap
    Serial.print(F("5. Heap   ... "));
    Serial.print(ESP.getFreeHeap());
    Serial.println(F(" bytes free"));

    Serial.println();
    if (ESP.getFreeHeap() > 50000) {
        Serial.println(F("All checks complete — board looks healthy!"));
    }
    else {
        Serial.println(F("All checks complete — low heap warning."));
    }
}

// ============================================================
// Printing Helpers
// ============================================================

/**
 * Print the start-up banner with OceanLabz branding.
 */
static void printBanner() {
    Serial.println(F("╔══════════════════════════════════════════════════════╗"));
    Serial.println(F("║         🌊 OceanLabz AI Explorer S3 Board            ║"));
    Serial.println(F("║              02 — Serial Test                       ║"));
    Serial.println(F("╠══════════════════════════════════════════════════════╣"));
    Serial.print(F("║  Firmware:  "));
    printPadded(F(__DATE__), 18);
    Serial.print(F(" ║"));
    Serial.println();
    Serial.print(F("║  Compiler:  "));
#if defined(__GNUC__)
    printPadded(F("GCC " __VERSION__), 18);
#else
    printPadded(F("Arduino"), 18);
#endif
    Serial.print(F(" ║"));
    Serial.println();
    Serial.print(F("║  SDK:       "));
    printPadded(ESP.getSdkVersion(), 18);
    Serial.print(F(" ║"));
    Serial.println();
    Serial.println(F("╚══════════════════════════════════════════════════════╝"));
    Serial.println();
}

/**
 * Print full system information.
 */
static void printSystemInfo() {
    esp_chip_info_t chip;
    esp_chip_info(&chip);

    Serial.println(F("── System Information ──"));

    // Chip
    Serial.print(F("  Chip Model:         ESP32-S3"));
    Serial.print(F(" rev."));
    Serial.println(chip.revision);

    Serial.print(F("  Cores:              "));
    Serial.println(chip.cores);
    Serial.print(F("  CPU Freq:           "));
    Serial.print(ESP.getCpuFreqMHz());
    Serial.println(F(" MHz"));

    // Features
    Serial.print(F("  Features:           "));
    if (chip.features & CHIP_FEATURE_WIFI_BGN)     Serial.print(F("WiFi "));
    if (chip.features & CHIP_FEATURE_BLE)           Serial.print(F("BLE "));
    if (chip.features & CHIP_FEATURE_EMB_FLASH)     Serial.print(F("EmbeddedFlash "));
    Serial.println();

    // Silicon revision (for detailed stepping info)
    uint32_t silRev = (REG_READ(EFUSE_RD_MAC_SPI_SYS_3_REG) >> 15) & 0x1FF;
    Serial.print(F("  Silicon Rev:        0x"));
    Serial.println(silRev, HEX);

    // Flash
    uint32_t flashSize = 0;
    esp_flash_get_size(NULL, &flashSize);
    Serial.print(F("  Flash Size:         "));
    Serial.print(flashSize / (1024 * 1024));
    Serial.println(F(" MB"));

    // PSRAM
#if CONFIG_SPIRAM
    Serial.print(F("  PSRAM Size:         "));
    Serial.print(ESP.getPsramSize() / (1024 * 1024));
    Serial.println(F(" MB"));
    Serial.print(F("  PSRAM Free:         "));
    Serial.print(ESP.getFreePsram() / (1024 * 1024));
    Serial.println(F(" MB"));
#else
    Serial.println(F("  PSRAM:              NOT ENABLED"));
    Serial.println(F("                       (Set PSRAM to OPI PSRAM in Tools)"));
#endif

    // Heap
    Serial.print(F("  Free Heap:          "));
    Serial.print(ESP.getFreeHeap());
    Serial.println(F(" bytes"));

    // Chip ID
    Serial.print(F("  Chip ID:            0x"));
    Serial.println((uint32_t)ESP.getEfuseMac(), HEX);

    // Temperature
    Serial.print(F("  Internal Temp:      "));
    Serial.print(temperatureRead(), 1);
    Serial.println(F(" °C"));

    // SDK / Arduino
    Serial.print(F("  Arduino Core:       "));
    Serial.println(ESP.getSdkVersion());

    // Flash mode
    Serial.print(F("  Flash Mode:         "));
    if (chip.features & CHIP_FEATURE_EMB_FLASH) {
        Serial.println(F("Embedded"));
    }
    else {
        Serial.println(F("External"));
    }

    // Board info — unique serial
    uint8_t mac[8];
    if (esp_read_mac(mac, ESP_MAC_BASE) == ESP_OK) {
        Serial.print(F("  Base MAC:           "));
        for (int i = 0; i < 6; i++) {
            if (mac[i] < 0x10) Serial.print(F("0"));
            Serial.print(mac[i], HEX);
            if (i < 5) Serial.print(F(":"));
        }
        Serial.println();
    }

    // USB info
    Serial.print(F("  USB VID:PID:        303A:1001"));
    Serial.println();

    // Reset reason
    Serial.print(F("  Reset Reason:       "));
    printResetReason(rtc_get_reset_reason(0));

    // Compile info
    Serial.print(F("  Compiled:           "));
    Serial.print(F(__DATE__));
    Serial.print(F(" "));
    Serial.println(F(__TIME__));
}

/**
 * Print a single-line status update (called every 10 seconds).
 */
static void printStatusLine() {
    Serial.println(F("──────────────────────────────────────────"));
    Serial.print(F("  [STATUS]  Heap: "));
    Serial.print(ESP.getFreeHeap());
    Serial.print(F(" B  |  PSRAM: "));
#if CONFIG_SPIRAM
    Serial.print(ESP.getFreePsram());
    Serial.print(F(" B  |  Temp: "));
    Serial.print(temperatureRead(), 1);
    Serial.println(F(" °C"));
#else
    Serial.print(F("N/A  |  Temp: "));
    Serial.print(temperatureRead(), 1);
    Serial.println(F(" °C"));
#endif
}

/**
 * Print a string padded with spaces to a fixed width (for banner alignment).
 */
static void printPadded(const String& s, uint8_t width) {
    uint8_t len = s.length();
    Serial.print(s);
    for (int i = len; i < width; i++) {
        Serial.print(F(" "));
    }
}

/**
 * Print a human-readable reset reason string.
 */
static void printResetReason(uint32_t reason) {
    switch (reason) {
    case 1:  Serial.println(F("POWER-ON / POR"));      break;
    case 3:  Serial.println(F("SW_RESET"));            break;
    case 4:  Serial.println(F("OWDT_RESET (Task Watchdog)")); break;
    case 5:  Serial.println(F("DEEPSLEEP_RESET"));     break;
    case 6:  Serial.println(F("SDIO_RESET"));          break;
    case 7:  Serial.println(F("TG0WDT_SYS_RESET"));    break;
    case 8:  Serial.println(F("TG1WDT_SYS_RESET"));    break;
    case 9:  Serial.println(F("RTCWDT_SYS_RESET"));    break;
    case 10: Serial.println(F("INTRUSION_RESET"));     break;
    case 11: Serial.println(F("TGWDT_CPU_RESET"));     break;
    case 12: Serial.println(F("SW_CPU_RESET"));        break;
    case 13: Serial.println(F("RTCWDT_CPU_RESET"));    break;
    case 14: Serial.println(F("EXT_CPU_RESET"));       break;
    case 15: Serial.println(F("AUDIO_PLL_RESET"));     break;
    case 16: Serial.println(F("RTCWDT_BROWN_OUT_RESET")); break;
    default: Serial.println(F("UNKNOWN"));              break;
    }
}

/**
 * Show the command prompt character.
 */
static void showPrompt() {
    Serial.print(F("> "));
}

/**
 * Set the RGB LED to the given R/G/B values.
 */
static void setLedColor(uint8_t r, uint8_t g, uint8_t b) {
    strip.setPixelColor(0, strip.Color(r, g, b));
    strip.show();
}
