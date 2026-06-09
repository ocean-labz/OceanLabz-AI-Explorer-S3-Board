/**
 * @file    03_camera_sd_capture.ino
 * @author  OceanLabz
 * @date    2026
 * @brief   Camera + SD capture example for the OceanLabz AI Explorer S3 Board.
 *
 *          Captures JPEG photos from the camera module (OV2640 / OV3660 /
 *          OV5640) and saves them directly to the microSD card.  Each capture
 *          creates a sequentially-numbered file:
 *            /photo_0001.jpg, /photo_0002.jpg, …
 *
 *          Press the **BOOT** button (GPIO0) to take a photo.
 *          Type commands into the Serial Monitor for additional control:
 *            capture — take a photo manually
 *            info    — show camera & SD card status
 *            res     — switch resolution (QVGA / VGA / SVGA / XGA / SXGA / UXGA)
 *            quality — change JPEG quality (1–63, lower = better)
 *            list    — list saved photos on SD card
 *            help    — show command list
 *
 *          Pin Mapping (camera parallel interface — fixed by board layout):
 *            PWDN    → N/C
 *            RESET   → N/C
 *            XCLK    → GPIO15
 *            PCLK    → GPIO13
 *            VSYNC   → GPIO6
 *            HREF    → GPIO7
 *            SIOD    → GPIO4   (SCCB Data / I²C SDA)
 *            SIOC    → GPIO5   (SCCB Clock / I²C SCL)
 *            D0/Y2   → GPIO11
 *            D1/Y3   → GPIO9
 *            D2/Y4   → GPIO8
 *            D3/Y5   → GPIO10
 *            D4/Y6   → GPIO12
 *            D5/Y7   → GPIO18
 *            D6/Y8   → GPIO17
 *            D7/Y9   → GPIO16
 *
 *          Pin Mapping (SD_MMC 1-bit mode):
 *            CMD     → GPIO3
 *            CLK     → GPIO2
 *            D0      → GPIO46
 *
 * @note  PSRAM MUST be enabled in the Arduino IDE:
 *        Tools → PSRAM → "OPI PSRAM"
 */

 // ============================================================
 // Camera Model Selection
 // ============================================================
 // Uncomment exactly ONE of the following lines:
#define CAMERA_MODEL_OV2640
// #define CAMERA_MODEL_OV3660
// #define CAMERA_MODEL_OV5640

// ============================================================
 // Includes
 // ============================================================
#include <esp_camera.h>
#include <SD_MMC.h>
#include <FS.h>

// ============================================================
 // Pin Definitions — OceanLabz AI Explorer S3
 // ============================================================
// Camera pins (matches CAMERA_MODEL_ESP32S3_EYE pinout)
#define PWDN_GPIO_NUM    -1
#define RESET_GPIO_NUM   -1
#define XCLK_GPIO_NUM    15
#define SIOD_GPIO_NUM     4
#define SIOC_GPIO_NUM     5

#define Y2_GPIO_NUM      11
#define Y3_GPIO_NUM       9
#define Y4_GPIO_NUM       8
#define Y5_GPIO_NUM      10
#define Y6_GPIO_NUM      12
#define Y7_GPIO_NUM      18
#define Y8_GPIO_NUM      17
#define Y9_GPIO_NUM      16

#define VSYNC_GPIO_NUM    6
#define HREF_GPIO_NUM     7
#define PCLK_GPIO_NUM    13

// SD_MMC pins (1-bit mode)
#define SD_CMD             3
#define SD_CLK             2
#define SD_D0             46

// BOOT button (ESP32-S3)
#define BOOT_BUTTON_PIN    0

// ============================================================
 // Resolution Presets
 // ============================================================
typedef struct {
    const char* name;
    framesize_t val;
} res_entry_t;

static const res_entry_t RESOLUTIONS[] = {
    { "QVGA",   FRAMESIZE_QVGA   },   // 320 x 240
    { "VGA",    FRAMESIZE_VGA    },   // 640 x 480
    { "SVGA",   FRAMESIZE_SVGA   },   // 800 x 600
    { "XGA",    FRAMESIZE_XGA    },   // 1024 x 768
    { "SXGA",   FRAMESIZE_SXGA   },   // 1280 x 1024
    { "UXGA",   FRAMESIZE_UXGA   },   // 1600 x 1200
#ifdef CAMERA_MODEL_OV5640
    { "QXGA",   FRAMESIZE_QXGA   },   // 2048 x 1536 (OV5640 only)
#endif
};
static const int NUM_RES = sizeof(RESOLUTIONS) / sizeof(RESOLUTIONS[0]);

// ============================================================
 // State
 // ============================================================
static int  g_resIdx = 5;     // default: UXGA (index 5)
static int  g_jpegQuality = 10;    // default JPEG quality (1–63, lower = better)
static bool g_cameraOk = false;
static bool g_sdOk = false;
static int  g_photoNum = 0;     // sequential photo counter

// Debounce for BOOT button
static unsigned long g_lastPress = 0;
static const unsigned long DEBOUNCE_MS = 500;

// ============================================================
 // Helper — Camera ID String
 // ============================================================
static const char* cameraIdStr() {
#if defined(CAMERA_MODEL_OV2640)
    return "OV2640";
#elif defined(CAMERA_MODEL_OV3660)
    return "OV3660";
#elif defined(CAMERA_MODEL_OV5640)
    return "OV5640";
#else
    return "UNKNOWN";
#endif
}

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
 // Helper — Initialise Camera
 // ============================================================
static bool initCamera() {
    // De-init if already running
    if (g_cameraOk) {
        esp_camera_deinit();
        g_cameraOk = false;
        delay(100);
    }

    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.frame_size = RESOLUTIONS[g_resIdx].val;
    config.pixel_format = PIXFORMAT_JPEG;
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.jpeg_quality = g_jpegQuality;
    config.fb_count = 1;

    // If PSRAM is present, upgrade to double-buffered + latest grab mode
    if (psramFound()) {
        config.jpeg_quality = g_jpegQuality;
        config.fb_count = 2;
        config.grab_mode = CAMERA_GRAB_LATEST;
    }
    else {
        // No PSRAM — limit resolution and use DRAM
        config.frame_size = FRAMESIZE_SVGA;
        config.fb_location = CAMERA_FB_IN_DRAM;
    }

    // Camera init
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed with error 0x%x\n", err);
        return false;
    }

    // Sensor post-init adjustments
    sensor_t* s = esp_camera_sensor_get();
    if (s) {
        s->set_vflip(s, 1);         // flip vertically
        s->set_brightness(s, 1);     // slight brightness boost
        s->set_saturation(s, 0);     // lower saturation
    }

    g_cameraOk = true;
    return true;
}

// ============================================================
 // Helper — Initialise SD Card
 // ============================================================
static bool initSDCard() {
    // Set the SD_MMC pins explicitly for this board
    // CLK=GPIO2, CMD=GPIO3, D0=GPIO46  (1-bit mode)
    SD_MMC.setPins(SD_CLK, SD_CMD, SD_D0);

    if (!SD_MMC.begin("/sdcard", true)) {   // true = 1-bit mode
        Serial.println("SD Card Mount Failed!");
        return false;
    }

    // Check card type
    uint8_t cardType = SD_MMC.cardType();
    if (cardType == CARD_NONE) {
        Serial.println("No SD card found!");
        return false;
    }

    g_sdOk = true;
    return true;
}

// ============================================================
 // Helper — Find next available photo number
 // ============================================================
static int findNextPhotoNum() {
    // Start from 1 and find the first unused number
    for (int i = 1; i <= 9999; i++) {
        char path[32];
        snprintf(path, sizeof(path), "/photo_%04d.jpg", i);
        if (!SD_MMC.exists(path)) {
            return i;
        }
    }
    return -1;  // all 9999 slots used
}

// ============================================================
 // Helper — Capture and Save Photo
 // ============================================================
static bool capturePhoto() {
    if (!g_cameraOk) {
        Serial.println("Camera not initialised — cannot capture.");
        return false;
    }
    if (!g_sdOk) {
        Serial.println("SD card not initialised — cannot save.");
        return false;
    }

    // Find next available filename
    int num = findNextPhotoNum();
    if (num < 0) {
        Serial.println("SD card full — all 9999 photo slots used!");
        return false;
    }

    char path[32];
    snprintf(path, sizeof(path), "/photo_%04d.jpg", num);

    Serial.printf("Capturing... ");

    // Acquire a frame
    camera_fb_t* fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("FAILED — camera frame buffer NULL");
        return false;
    }

    // Verify we got JPEG data
    if (fb->format != PIXFORMAT_JPEG) {
        Serial.println("FAILED — frame is not JPEG (change pixel format to JPEG)");
        esp_camera_fb_return(fb);
        return false;
    }

    Serial.println("OK!");

    // Save frame length before writing (so we can verify after)
    size_t fbLen = fb->len;

    // Write to SD card
    File file = SD_MMC.open(path, FILE_WRITE);
    if (!file) {
        Serial.println("Save FAILED — could not open file for writing");
        esp_camera_fb_return(fb);
        return false;
    }

    size_t written = file.write(fb->buf, fb->len);
    file.close();

    // Return frame buffer to free memory
    esp_camera_fb_return(fb);

    // Verify write
    if (written == 0) {
        Serial.println("Save FAILED — 0 bytes written (card full or write-protected?)");
        return false;
    }
    if (written != fbLen) {
        Serial.printf("WARNING — wrote %d of %d bytes\n", written, fbLen);
    }

    Serial.printf("Saved to %s\n", path);
    Serial.printf("File size: %d bytes\n", written);
    Serial.println();

    g_photoNum = num;
    return true;
}

// ============================================================
 // Helper — List Saved Photos
 // ============================================================
static void listPhotos() {
    if (!g_sdOk) {
        Serial.println("SD card not initialised.");
        return;
    }

    Serial.println("Photos on SD card:");
    int count = 0;
    File root = SD_MMC.open("/");
    if (!root || !root.isDirectory()) {
        Serial.println("  Failed to open root directory");
        return;
    }

    File file = root.openNextFile();
    while (file) {
        const char* name = file.name();
        // Only list .jpg files
        if (strstr(name, ".jpg") || strstr(name, ".JPG")) {
            Serial.printf("  %s  (%d bytes)\n", name, file.size());
            count++;
        }
        file = root.openNextFile();
    }
    root.close();

    if (count == 0) {
        Serial.println("  (no photos found)");
    }
    Serial.printf("Total: %d photos\n\n", count);
}

// ============================================================
 // Helper — Print System Info
 // ============================================================
static void printInfo() {
    Serial.println(F("──────────────────────────────────────"));
    Serial.println(F("  System Info"));
    Serial.println(F("──────────────────────────────────────"));

    // Camera info
    Serial.printf("  Camera:      %s\n", g_cameraOk ? cameraIdStr() : "NOT INIT");
    Serial.printf("  Resolution:  %s\n", RESOLUTIONS[g_resIdx].name);
    Serial.printf("  JPEG Quality: %d\n", g_jpegQuality);

    // SD card info
    if (g_sdOk) {
        Serial.printf("  SD Card:     %s\n", cardTypeStr(SD_MMC.cardType()));
        Serial.printf("  Card Size:   %llu MB\n", SD_MMC.cardSize() / (1024 * 1024));
        Serial.printf("  Total Space: %llu MB\n", SD_MMC.totalBytes() / (1024 * 1024));
        Serial.printf("  Used Space:  %llu MB\n", SD_MMC.usedBytes() / (1024 * 1024));
    }
    else {
        Serial.println(F("  SD Card:     NOT INIT"));
    }

    // Memory info
    Serial.printf("  Free Heap:   %d bytes\n", ESP.getFreeHeap());
    if (psramFound()) {
        Serial.printf("  Free PSRAM:  %d bytes\n", ESP.getFreePsram());
    }
    else {
        Serial.println(F("  PSRAM:       Not found"));
    }

    Serial.printf("  Photos taken: %d\n", g_photoNum);
    Serial.println(F("──────────────────────────────────────"));
    Serial.println();
}

// ============================================================
 // Helper — Print Help
 // ============================================================
static void printHelp() {
    Serial.println(F("──────────────────────────────────────"));
    Serial.println(F("  Commands (type into Serial Monitor)"));
    Serial.println(F("──────────────────────────────────────"));
    Serial.println(F("  capture — take a photo and save to SD"));
    Serial.println(F("  info    — show camera & SD card status"));
    Serial.println(F("  res     — cycle through resolutions"));
    Serial.println(F("  quality — change JPEG quality (1-63)"));
    Serial.println(F("    usage: quality <number>"));
    Serial.println(F("    e.g.:  quality 10"));
    Serial.println(F("  list    — list saved photos on SD card"));
    Serial.println(F("  heap    — show free heap / PSRAM"));
    Serial.println(F("  help    — show this help"));
    Serial.println(F("  restart — soft-reset the board"));
    Serial.println(F("──────────────────────────────────────"));
    Serial.println(F("  BOOT button (GPIO0) = capture photo"));
    Serial.println(F("──────────────────────────────────────"));
    Serial.println();
}

// ============================================================
 // Helper — Process Serial Commands
 // ============================================================
static void processCommand(const String& cmd) {
    String trimmed = cmd;
    trimmed.trim();

    if (trimmed == "capture") {
        capturePhoto();
    }
    else if (trimmed == "info") {
        printInfo();
    }
    else if (trimmed == "res") {
        g_resIdx = (g_resIdx + 1) % NUM_RES;
        Serial.printf("Switching resolution to: %s\n", RESOLUTIONS[g_resIdx].name);
        if (initCamera()) {
            Serial.println("Camera re-initialised OK.");
        }
        else {
            Serial.println("Camera re-init FAILED!");
        }
    }
    else if (trimmed.startsWith("quality")) {
        // Parse: "quality 10"
        int idx = trimmed.indexOf(' ');
        if (idx > 0) {
            int q = trimmed.substring(idx + 1).toInt();
            if (q >= 1 && q <= 63) {
                g_jpegQuality = q;
                Serial.printf("JPEG quality set to: %d\n", g_jpegQuality);
                // Update the sensor quality
                sensor_t* s = esp_camera_sensor_get();
                if (s) {
                    s->set_quality(s, g_jpegQuality);
                    Serial.println("Sensor quality updated.");
                }
            }
            else {
                Serial.println("Quality must be 1–63 (lower = better).");
            }
        }
        else {
            Serial.printf("Current JPEG quality: %d\n", g_jpegQuality);
            Serial.println("Usage: quality <1-63>  (lower = better)");
        }
    }
    else if (trimmed == "list") {
        listPhotos();
    }
    else if (trimmed == "heap") {
        Serial.printf("Free Heap:  %d bytes\n", ESP.getFreeHeap());
        if (psramFound()) {
            Serial.printf("Free PSRAM: %d bytes\n", ESP.getFreePsram());
        }
        else {
            Serial.println("PSRAM: Not found");
        }
        Serial.println();
    }
    else if (trimmed == "help") {
        printHelp();
    }
    else if (trimmed == "restart") {
        Serial.println("Restarting...");
        ESP.restart();
    }
    else if (trimmed.length() > 0) {
        Serial.printf("Unknown command: '%s'\n", trimmed.c_str());
        Serial.println("Type 'help' for available commands.");
    }
}

// ============================================================
 // setup() — runs once on power-up / reset
 // ============================================================
void setup() {
    Serial.begin(115200);
    delay(500);   // allow serial to settle

    Serial.println();
    Serial.println(F("========================================"));
    Serial.println(F("  OceanLabz AI Explorer S3 Board"));
    Serial.println(F("  Example 03 — Camera + SD Capture"));
    Serial.println(F("========================================"));
    Serial.println();

    // ── 1. Initialise Camera ──────────────────────────────────
    Serial.printf("Initialising camera (%s) ... ", cameraIdStr());

    if (!initCamera()) {
        Serial.println("FAILED!");
        Serial.println();
        Serial.println(F("Troubleshooting:"));
        Serial.println(F("  - Enable PSRAM: Tools → PSRAM → OPI PSRAM"));
        Serial.println(F("  - Check camera module connection (FPC cable)"));
        Serial.println(F("  - Try a lower resolution (type 'res' to cycle)"));
        return;
    }
    Serial.printf("OK!  Resolution: %s, Quality: %d\n",
        RESOLUTIONS[g_resIdx].name, g_jpegQuality);

    // ── 2. Initialise SD Card ─────────────────────────────────
    Serial.print(F("Initialising SD_MMC (1-bit mode)... "));

    if (!initSDCard()) {
        Serial.println("FAILED!");
        Serial.println();
        Serial.println(F("Troubleshooting:"));
        Serial.println(F("  - Ensure a FAT32-formatted microSD is inserted"));
        Serial.println(F("  - Check SD card is not write-protected"));
        Serial.println(F("  - Try reformatting the card (FAT32, default allocation size)"));
        // Camera still works — just can't save photos
        Serial.println();
        Serial.println(F("Camera is operational but photos cannot be saved."));
    }
    else {
        Serial.printf("OK!  Card: %s, Size: %llu MB\n",
            cardTypeStr(SD_MMC.cardType()),
            SD_MMC.cardSize() / (1024 * 1024));
    }

    // ── 3. Configure BOOT Button ──────────────────────────────
    pinMode(BOOT_BUTTON_PIN, INPUT_PULLUP);

    Serial.println();
    Serial.println(F("Ready!  Press BOOT button or type 'capture' to take a photo."));
    Serial.println(F("        Type 'help' for available commands."));
    Serial.println();
}

// ============================================================
 // loop() — runs forever
 // ============================================================
void loop() {
    // ── Check BOOT button (with debounce) ──────────────────────
    if (digitalRead(BOOT_BUTTON_PIN) == LOW) {
        unsigned long now = millis();
        if (now - g_lastPress >= DEBOUNCE_MS) {
            g_lastPress = now;
            Serial.println(F("[BOOT] Button pressed — capturing photo..."));
            capturePhoto();
        }
    }

    // ── Check Serial commands ──────────────────────────────────
    if (Serial.available()) {
        String cmd = Serial.readStringUntil('\n');
        processCommand(cmd);
    }

    delay(10);   // small delay to avoid busy-looping
}
