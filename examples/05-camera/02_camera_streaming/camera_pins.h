/**
 * @file    camera_pins.h
 * @author  OceanLabz
 * @date    2026
 * @brief   Camera pin definitions for the OceanLabz AI Explorer S3 Board.
 *
 *          These pins match the CAMERA_MODEL_ESP32S3_EYE pinout used by
 *          the ESP32 camera driver. The parallel camera interface pins
 *          are fixed by the board layout and must not be changed.
 *
 *          Pin Mapping:
 *            PWDN    → N/C (not connected)
 *            RESET   → N/C (not connected)
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
 */

#ifndef CAMERA_PINS_H
#define CAMERA_PINS_H

 // ============================================================
 // Camera Model Selection
 // ============================================================
 // The camera model is defined in the main .ino sketch before
 // this header is included. Do NOT redefine it here.
 // Uncomment exactly ONE in 02_camera_streaming.ino:
 //   #define CAMERA_MODEL_OV2640
 //   #define CAMERA_MODEL_OV3660
 //   #define CAMERA_MODEL_OV5640

// ============================================================
// Pin Definitions — OceanLabz AI Explorer S3
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

#endif // CAMERA_PINS_H
