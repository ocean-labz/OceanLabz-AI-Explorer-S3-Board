/**
 * @file    01_camera_test.ino
 * @author  OceanLabz
 * @date    2026
 * @brief   Camera example for the OceanLabz AI Explorer S3 Board.
 *
 *          Initialises the camera module (OV2640 / OV3660 / OV5640),
 *          captures a single frame, and prints frame information to
 *          the Serial console.
 *
 *          Commands (type into Serial Monitor):
 *            help    — show this help
 *            capture — capture a new frame and print info
 *            info    — print camera configuration
 *            res     — switch resolution (QVGA / VGA / SVGA / XGA / SXGA / UXGA)
 *            fmt     — switch pixel format (JPEG / GRAYSCALE / RGB565)
 *            heap    — show free heap / PSRAM
 *            restart — soft-reset the board
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

// ============================================================
// Pin Definitions — OceanLabz AI Explorer S3
// (Matches CAMERA_MODEL_ESP32S3_EYE pinout in ESP32 camera driver)
// ============================================================
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
// Pixel Format Presets
// ============================================================
typedef struct {
    const char* name;
    pixformat_t val;
} fmt_entry_t;

static const fmt_entry_t FORMATS[] = {
    { "JPEG",      PIXFORMAT_JPEG     },
    { "GRAYSCALE", PIXFORMAT_GRAYSCALE},
    { "RGB565",    PIXFORMAT_RGB565   },
};
static const int NUM_FMTS = sizeof(FORMATS) / sizeof(FORMATS[0]);

// ============================================================
// State
// ============================================================
static int  g_resIdx = 0;    // current resolution index
static int  g_fmtIdx = 0;    // current format index
static bool g_cameraOk = false;

// ============================================================
// Helper — Initialise Camera
// (Follows the proven pattern from esp32s3-camera-webserver)
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
    config.pixel_format = FORMATS[g_fmtIdx].val;
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.jpeg_quality = 12;
    config.fb_count = 1;

    // If PSRAM is present, upgrade to double-buffered + latest grab mode
    if (psramFound()) {
        config.jpeg_quality = 10;
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

    // Sensor post-init adjustments (from working webserver code)
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
// Helper — Print Camera ID String
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
// Helper — Print Pixel Format Name
// ============================================================
static const char* pixFormatStr(pixformat_t fmt) {
    switch (fmt) {
    case PIXFORMAT_JPEG:      return "JPEG";
    case PIXFORMAT_GRAYSCALE: return "GRAYSCALE";
    case PIXFORMAT_RGB565:    return "RGB565";
    case PIXFORMAT_YUV422:    return "YUV422";
    case PIXFORMAT_YUV420:    return "YUV420";
    default:                  return "OTHER";
    }
}

// ============================================================
// Helper — Print Resolution Dimensions
// ============================================================
static void printResolution(framesize_t fs) {
    switch (fs) {
    case FRAMESIZE_QVGA:   Serial.print(F("320x240"));   break;
    case FRAMESIZE_VGA:    Serial.print(F("640x480"));   break;
    case FRAMESIZE_SVGA:   Serial.print(F("800x600"));   break;
    case FRAMESIZE_XGA:    Serial.print(F("1024x768"));  break;
    case FRAMESIZE_SXGA:   Serial.print(F("1280x1024")); break;
    case FRAMESIZE_UXGA:   Serial.print(F("1600x1200")); break;
#ifdef CAMERA_MODEL_OV5640
    case FRAMESIZE_QXGA:   Serial.print(F("2048x1536")); break;
#endif
    default:               Serial.print(F("unknown"));    break;
    }
}

// ============================================================
// Helper — PSRAM Check
// ============================================================
static void printPsramInfo() {
    Serial.print(F("  PSRAM: "));
    if (psramFound()) {
        Serial.print(F("found  — "));
        Serial.print(ESP.getPsramSize() / (1024 * 1024));
        Serial.println(F(" MB"));
    }
    else {
        Serial.println(F("NOT FOUND — enable OPI PSRAM in Tools menu"));
    }
}

// ============================================================
// Helper — Capture & Print Frame Info
// ============================================================
static void captureAndPrint() {
    if (!g_cameraOk) {
        Serial.println(F("Camera not initialised. Type 'help'."));
        return;
    }

    // Capture
    Serial.print(F("Capturing... "));
    camera_fb_t* fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println(F("FAILED"));
        return;
    }
    Serial.println(F("OK"));

    // Frame info
    Serial.println(F("  ┌─ Frame Information ─────────────────────"));
    Serial.print(F("  │ Resolution : "));
    printResolution(RESOLUTIONS[g_resIdx].val);
    Serial.println();
    Serial.print(F("  │ Format     : "));
    Serial.println(pixFormatStr(fb->format));
    Serial.print(F("  │ Width      : "));
    Serial.println(fb->width);
    Serial.print(F("  │ Height     : "));
    Serial.println(fb->height);
    Serial.print(F("  │ Size       : "));
    Serial.print(fb->len);
    Serial.println(F(" bytes"));

    // JPEG-specific info
    if (fb->format == PIXFORMAT_JPEG) {
        Serial.print(F("  │ Quality    : JPEG compressed "));
        Serial.print(fb->len);
        Serial.println(F(" bytes"));
    }

    Serial.println(F("  └──────────────────────────────────────────"));

    // Free the frame buffer
    esp_camera_fb_return(fb);
}

// ============================================================
// Helper — Print Camera Configuration
// ============================================================
static void printConfig() {
    Serial.println(F("  ┌─ Camera Configuration ──────────────────"));
    Serial.print(F("  │ Model      : "));
    Serial.println(cameraIdStr());
    Serial.print(F("  │ Resolution : "));
    Serial.print(RESOLUTIONS[g_resIdx].name);
    Serial.print(F(" ("));
    printResolution(RESOLUTIONS[g_resIdx].val);
    Serial.println(F(")"));
    Serial.print(F("  │ Format     : "));
    Serial.println(FORMATS[g_fmtIdx].name);
    printPsramInfo();

    sensor_t* s = esp_camera_sensor_get();
    if (s) {
        Serial.print(F("  │ Sensor PID : 0x"));
        Serial.println(s->id.PID, HEX);
        Serial.print(F("  │ Sensor VER : 0x"));
        Serial.println(s->id.VER, HEX);
    }
    Serial.println(F("  └──────────────────────────────────────────"));
}

// ============================================================
// Helper — Switch Resolution
// ============================================================
static void setResolution(int idx) {
    if (idx < 0 || idx >= NUM_RES) {
        Serial.print(F("Invalid index. Use 0.."));
        Serial.println(NUM_RES - 1);
        return;
    }
    g_resIdx = idx;
    Serial.print(F("Switching to "));
    Serial.print(RESOLUTIONS[g_resIdx].name);
    Serial.println(F("..."));

    if (initCamera()) {
        Serial.println(F("OK"));
    }
}

// ============================================================
// Helper — Switch Format
// ============================================================
static void setFormat(int idx) {
    if (idx < 0 || idx >= NUM_FMTS) {
        Serial.print(F("Invalid index. Use 0.."));
        Serial.println(NUM_FMTS - 1);
        return;
    }
    g_fmtIdx = idx;
    Serial.print(F("Switching to "));
    Serial.print(FORMATS[g_fmtIdx].name);
    Serial.println(F("..."));

    if (initCamera()) {
        Serial.println(F("OK"));
    }
}

// ============================================================
// Helper — Print Banner
// ============================================================
static void printBanner() {
    Serial.println(F("========================================"));
    Serial.println(F("  OceanLabz AI Explorer S3 Board"));
    Serial.println(F("  Example 01 — Camera Test"));
    Serial.println(F("========================================"));
    Serial.println();
}

// ============================================================
// Helper — Show Help
// ============================================================
static void showHelp() {
    Serial.println(F("Available commands:"));
    Serial.println(F("  help              — show this help"));
    Serial.println(F("  capture           — capture a frame and print info"));
    Serial.println(F("  info              — print camera config & PSRAM status"));
    Serial.println(F("  res               — list available resolutions"));
    Serial.println(F("  res <n>           — switch to resolution index <n>"));
    Serial.println(F("  fmt               — list available pixel formats"));
    Serial.println(F("  fmt <n>           — switch to format index <n>"));
    Serial.println(F("  heap              — show free heap / PSRAM"));
    Serial.println(F("  restart           — soft-reset the board"));
    Serial.println();
}

// ============================================================
// Helper — List Resolutions
// ============================================================
static void listResolutions() {
    Serial.println(F("Available resolutions:"));
    for (int i = 0; i < NUM_RES; i++) {
        Serial.print(F("  ["));
        Serial.print(i);
        Serial.print(F("] "));
        Serial.print(RESOLUTIONS[i].name);
        Serial.print(F(" ("));
        printResolution(RESOLUTIONS[i].val);
        Serial.println(F(")"));
    }
    Serial.print(F("Current: ["));
    Serial.print(g_resIdx);
    Serial.print(F("] "));
    Serial.println(RESOLUTIONS[g_resIdx].name);
    Serial.println();
}

// ============================================================
// Helper — List Formats
// ============================================================
static void listFormats() {
    Serial.println(F("Available pixel formats:"));
    for (int i = 0; i < NUM_FMTS; i++) {
        Serial.print(F("  ["));
        Serial.print(i);
        Serial.print(F("] "));
        Serial.println(FORMATS[i].name);
    }
    Serial.print(F("Current: ["));
    Serial.print(g_fmtIdx);
    Serial.print(F("] "));
    Serial.println(FORMATS[g_fmtIdx].name);
    Serial.println();
}

// ============================================================
// Helper — Show Heap
// ============================================================
static void showHeap() {
    Serial.print(F("Free heap : "));
    Serial.print(ESP.getFreeHeap());
    Serial.println(F(" bytes"));

    Serial.print(F("Free PSRAM: "));
    if (psramFound()) {
        Serial.print(ESP.getFreePsram());
        Serial.println(F(" bytes"));
    }
    else {
        Serial.println(F("N/A (not found)"));
    }
    Serial.println();
}

// ============================================================
// Helper — Process Serial Command
// ============================================================
static void processCommand(const String& cmd) {
    String c = cmd;
    c.trim();
    if (c.length() == 0) return;

    int sp = c.indexOf(' ');
    String verb = (sp == -1) ? c : c.substring(0, sp);
    String arg = (sp == -1) ? "" : c.substring(sp + 1);

    verb.toLowerCase();

    if (verb == "help") {
        showHelp();
    }
    else if (verb == "capture") {
        captureAndPrint();
    }
    else if (verb == "info") {
        printConfig();
    }
    else if (verb == "res") {
        if (arg.length() == 0) {
            listResolutions();
        }
        else {
            int idx = arg.toInt();
            setResolution(idx);
        }
    }
    else if (verb == "fmt") {
        if (arg.length() == 0) {
            listFormats();
        }
        else {
            int idx = arg.toInt();
            setFormat(idx);
        }
    }
    else if (verb == "heap") {
        showHeap();
    }
    else if (verb == "restart") {
        Serial.println(F("Restarting..."));
        delay(500);
        ESP.restart();
    }
    else {
        Serial.print(F("Unknown command: \""));
        Serial.print(verb);
        Serial.println(F("\". Type 'help'."));
    }
}

// ============================================================
// Helper — Show Prompt
// ============================================================
static void showPrompt() {
    Serial.print(F("> "));
}

// ============================================================
// setup() — runs once on power-up / reset
// ============================================================
void setup() {
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    delay(800);

    Serial.println();
    printBanner();

    // --- PSRAM check ---
    printPsramInfo();
    Serial.println();

    // --- Initialise camera ---
    Serial.print(F("Initialising camera ("));
    Serial.print(cameraIdStr());
    Serial.print(F(", "));
    Serial.print(RESOLUTIONS[g_resIdx].name);
    Serial.print(F(", "));
    Serial.print(FORMATS[g_fmtIdx].name);
    Serial.println(F(")..."));

    if (initCamera()) {
        Serial.println(F("Camera ready!"));
    }
    else {
        Serial.println(F("Camera init failed. Check PSRAM, ribbon cable, and model #define."));
    }

    Serial.println();
    showHelp();
    showPrompt();
}

// ============================================================
// loop() — runs forever
// ============================================================
void loop() {
    static String input;

    while (Serial.available()) {
        char ch = Serial.read();
        if (ch == '\r' || ch == '\n') {
            if (input.length() > 0) {
                processCommand(input);
                input = "";
                showPrompt();
            }
        }
        else {
            input += ch;
        }
    }
}
