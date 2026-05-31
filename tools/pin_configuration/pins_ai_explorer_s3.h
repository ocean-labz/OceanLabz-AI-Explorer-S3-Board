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
#define CAM_RESET          -1    // Reset (not connected — shared with TFT RST)
#define CAM_XCLK           15    // External Clock
#define CAM_SIOD            4    // SCCB Data (I²C-like)
#define CAM_SIOC            5    // SCCB Clock
#define CAM_Y9              6    // Pixel data bit 9
#define CAM_Y8              7    // Pixel data bit 8
#define CAM_Y7              8    // Pixel data bit 7
#define CAM_Y6              9    // Pixel data bit 6
#define CAM_Y5             10    // Pixel data bit 5
#define CAM_Y4             11    // Pixel data bit 4
#define CAM_Y3             12    // Pixel data bit 3
#define CAM_Y2             13    // Pixel data bit 2
#define CAM_VSYNC          14    // Vertical Sync (shared with RGB LED)
#define CAM_HREF           16    // Horizontal Reference
#define CAM_PCLK           17    // Pixel Clock

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