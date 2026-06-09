/**
 * @file    01_mic_recorder.ino
 * @author  OceanLabz
 * @date    2026-06-02
 * @brief   Record audio from the onboard INMP441 digital microphone
 *          and display the live waveform in the Arduino Serial Plotter.
 *
 * Hardware: OceanLabz AI Explorer S3 Board
 * Module:   INMP441 digital microphone (onboard, I²S)
 * Pins:     WS = GPIO39  (Word Select / LRCLK)
 *           SCK = GPIO40 (Serial Clock / BCLK)
 *           SD  = GPIO41 (Serial Data / DOUT)
 *           I²S port = 0
 *
 * Instructions:
 *   1. Upload this sketch to the board.
 *   2. Open Tools → Serial Plotter (115200 baud).
 *   3. Speak into the microphone (located near the board edge).
 *   4. Watch the live waveform — silence is a flat line, speech
 *      produces amplitude peaks.
 */

#include <driver/i2s.h>

 // =============================================================
 // Pin Definitions (onboard INMP441)
 // =============================================================
#define MIC_WS   39
#define MIC_SCK  40
#define MIC_SD   41

// =============================================================
// I²S Configuration
// =============================================================
const i2s_port_t I2S_PORT = I2S_NUM_0;

// The INMP441 is a 24-bit microphone. We read 32-bit samples
// (left-aligned 24-bit data in a 32-bit frame) to get the
// full range, then scale down to 16-bit for display.
const uint32_t SAMPLE_RATE = 16000;
const uint16_t BUFFER_SIZE = 64;        // smaller buffer = less latency
int32_t        raw32[BUFFER_SIZE];      // 32-bit raw samples
int16_t        displaySamples[32];      // decimated output for Serial Plotter

// =============================================================
// Setup
// =============================================================
void setup() {
    Serial.begin(115200);
    while (!Serial) { ; }
    Serial.println("\n=== INMP441 Mic Recorder ===");

    i2s_config_t i2sConfig = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
      .sample_rate = SAMPLE_RATE,
      .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,  // read full 24-bit data
      .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
      .communication_format = I2S_COMM_FORMAT_STAND_I2S,
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
      .dma_buf_count = 4,
      .dma_buf_len = BUFFER_SIZE,
      .use_apll = false,
      .tx_desc_auto_clear = false,
      .fixed_mclk = 0
    };

    esp_err_t err = i2s_driver_install(I2S_PORT, &i2sConfig, 0, NULL);
    if (err != ESP_OK) { Serial.printf("INSTALL ERR: %d\n", err); while (1) { ; } }

    i2s_pin_config_t pinConfig = {
      .bck_io_num = MIC_SCK,
      .ws_io_num = MIC_WS,
      .data_out_num = I2S_PIN_NO_CHANGE,
      .data_in_num = MIC_SD
    };

    err = i2s_set_pin(I2S_PORT, &pinConfig);
    if (err != ESP_OK) { Serial.printf("PIN ERR: %d\n", err); while (1) { ; } }

    Serial.println("Ready! Open Serial Plotter and speak into the mic.");
}

// =============================================================
// Loop
// =============================================================
void loop() {
    size_t bytesRead = 0;

    // Read 32-bit samples from the INMP441
    esp_err_t err = i2s_read(I2S_PORT, raw32, BUFFER_SIZE * sizeof(int32_t),
        &bytesRead, portMAX_DELAY);
    if (err != ESP_OK) return;

    uint16_t samples = bytesRead / sizeof(int32_t);

    // Calculate DC offset from this buffer
    int32_t sum = 0;
    for (uint16_t i = 0; i < samples; i++) {
        sum += raw32[i];
    }
    int32_t dc = sum / samples;

    // Decimate: only send ~32 samples to the plotter so it doesn't
    // get flooded. Also shift the 24-bit data down to 16-bit range:
    //   raw32: 0xFFFF??00  (24-bit left-aligned in 32-bit)
    //   >> 8  ->  0x00FFFFFF  (now fits in 16-bit after subtracting DC)
    uint16_t step = samples / 32;
    if (step < 1) step = 1;

    uint8_t count = 0;
    for (uint16_t i = 0; i < samples && count < 32; i += step) {
        int32_t val = (raw32[i] >> 8) - (dc >> 8);  // subtract DC, shift to 16-bit
        // Clamp to 16-bit range
        if (val > 32767) val = 32767;
        if (val < -32768) val = -32768;
        Serial.println((int16_t)val);
        count++;
    }

    // Pause so the plotter can render cleanly
    delay(30);
}
