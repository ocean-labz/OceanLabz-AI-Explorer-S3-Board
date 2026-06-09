/**
 * @file    02_camera_streaming.ino
 * @author  OceanLabz (adapted from Espressif CameraWebServer example)
 * @date    2026
 * @brief   WiFi camera streaming example for the OceanLabz AI Explorer S3 Board.
 *
 *          Creates a WiFi Access Point and serves a web interface for:
 *          - Live MJPEG streaming (/stream)
 *          - Single-frame JPEG capture (/capture)
 *          - BMP capture (/bmp)
 *          - Camera control (resolution, quality, brightness, etc.)
 *
 *          Default AP credentials:
 *            SSID     : OceanLabz-CAM
 *            Password : 12345678
 *
 *          After uploading, open a browser and navigate to:
 *            http://192.168.4.1
 *
 * @note  PSRAM MUST be enabled in the Arduino IDE:
 *        Tools → PSRAM → "OPI PSRAM"
 *
 * @note  This example includes files from the Espressif CameraWebServer
 *        example, adapted for the AI Explorer S3 pin mapping.
 */

 // ============================================================
 // Camera Model Selection
 // ============================================================
 // Uncomment exactly ONE of the following lines:
#define CAMERA_MODEL_OV2640
// #define CAMERA_MODEL_OV3660
// #define CAMERA_MODEL_OV5640

#include "camera_pins.h"
#include <esp_camera.h>
#include <WiFi.h>

// ============================================================
// WiFi Access Point Configuration
// ============================================================
const char* ssid = "OceanLabz-CAM";
const char* password = "12345678";

// Forward declaration — defined in app_httpd.cpp
void startCameraServer();

// ============================================================
// setup() — runs once on power-up / reset
// ============================================================
void setup() {
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println();

    // ── Camera Configuration ──────────────────────────────────
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
    config.frame_size = FRAMESIZE_UXGA;
    config.pixel_format = PIXFORMAT_JPEG;  // for streaming
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
        return;
    }

    // Sensor post-init adjustments
    sensor_t* s = esp_camera_sensor_get();
    if (s) {
        // Initial sensors are flipped vertically and colors are a bit saturated
        s->set_vflip(s, 1);         // flip it back
        s->set_brightness(s, 1);     // up the brightness just a bit
        s->set_saturation(s, 0);     // lower the saturation
    }

    // ── WiFi Access Point ────────────────────────────────────
    WiFi.softAP(ssid, password);
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);

    // ── Start Camera Server ──────────────────────────────────
    startCameraServer();

    Serial.print("Camera Ready! Use 'http://");
    Serial.print(IP);
    Serial.println("' to connect");
}

// ============================================================
// loop() — runs forever
// ============================================================
void loop() {
    // Do nothing. Everything is done in another task by the web server
    delay(10000);
}
