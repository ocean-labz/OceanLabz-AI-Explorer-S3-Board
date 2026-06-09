/**
 * @file    pins_ai_explorer_s3.h
 * @author  OceanLabz
 * @date    2024-01-01
 * @brief   Pin definitions for OceanLabz AI Explorer S3 Board
 *
 * Include this header in any sketch targeting the AI Explorer S3:
 *   #include "pins_ai_explorer_s3.h"
 *
 * This file is maintained at:
 *   tools/pin_configuration/pins_ai_explorer_s3.h
 */

#ifndef PINS_AI_EXPLORER_S3_H
#define PINS_AI_EXPLORER_S3_H

 // ============================================================
 // Power / System
 // ============================================================
#define BOARD_LED          14    // RGB LED (NeoPixel / WS2812B)

// ============================================================
// Microphone — INMP441 (I²S)
// ============================================================
#define MIC_WS             39    // Word Select (LRCLK)
#define MIC_SCK            40    // Serial Clock (BCLK)
#define MIC_SD             41    // Serial Data (DOUT)

// ============================================================
// Speaker Amplifier — MAX98357A (I²S)
// ============================================================
#define SPEAKER_LRCK       21    // Left-Right Clock
#define SPEAKER_BCLK       48    // Bit Clock
#define SPEAKER_DIN        47    // Data In

// ============================================================
// SD_MMC Card (1-bit mode)
// ============================================================
#define SD_CMD              3    // Command line
#define SD_CLK              2    // Clock line
#define SD_D0              46    // Data line 0

// ============================================================
// SPI TFT Display
// ============================================================
#define TFT_BL             42    // Backlight control
#define TFT_MOSI           44    // Master Out Slave In
#define TFT_CLK            43    // Serial Clock
#define TFT_DC             38    // Data / Command
#define TFT_RST             1    // Reset
#define TFT_CS             45    // Chip Select

// ============================================================
// I²C OLED Display
// NOTE: Shares GPIO43 (SCL) and GPIO44 (SDA) with SPI TFT
//       Do NOT connect both displays simultaneously.
// ============================================================
#define OLED_SDA           43    // I²C Data (shared with TFT_CLK)
#define OLED_SCL           44    // I²C Clock (shared with TFT_MOSI)

// ============================================================
// Camera Parallel Interface
// Pins are fixed by board layout. Do not change.
// ============================================================
#define CAM_PWDN           -1    // Power Down (not connected)
#define CAM_RESET          -1    // Reset (not connected)
#define CAM_XCLK           15    // External Clock
#define CAM_PCLK           13    // Pixel Clock
#define CAM_VSYNC           6    // Vertical Sync
#define CAM_HREF            7    // Horizontal Reference
#define CAM_SIOD            4    // SCCB Data (I²C-like)
#define CAM_SIOC            5    // SCCB Clock
#define CAM_D0             11    // Data bit 0  (Y2)
#define CAM_D1              9    // Data bit 1  (Y3)
#define CAM_D2              8    // Data bit 2  (Y4)
#define CAM_D3             10    // Data bit 3  (Y5)
#define CAM_D4             12    // Data bit 4  (Y6)
#define CAM_D5             18    // Data bit 5  (Y7)
#define CAM_D6             17    // Data bit 6  (Y8)
#define CAM_D7             16    // Data bit 7  (Y9)

// ============================================================
// I²S Bus Configuration
// ============================================================
#define I2S_MIC_PORT       0     // I²S port for INMP441
#define I2S_SPEAKER_PORT   1     // I²S port for MAX98357A

// ============================================================
// Camera Model Constants (for use with esp_camera)
// ============================================================
#define CAMERA_MODEL_AI_EXPLORER_OV2640  0
#define CAMERA_MODEL_AI_EXPLORER_OV3660  1
#define CAMERA_MODEL_AI_EXPLORER_OV5640  2

#endif // PINS_AI_EXPLORER_S3_H
