/**
 * @file    02_speaker_test.ino
 * @author  OceanLabz
 * @date    2026-06-02
 * @brief   Test the onboard MAX98357A I²S speaker amplifier by
 *          playing a 1 kHz sine wave followed by a frequency sweep.
 *
 * Hardware: OceanLabz AI Explorer S3 Board
 * Module:   MAX98357A I²S speaker amplifier (onboard)
 * Pins:     LRCK = GPIO21 (Left-Right Clock / Word Select)
 *           BCLK = GPIO48 (Bit Clock / Serial Clock)
 *           DIN  = GPIO47 (Serial Data In)
 *           I²S port = 1
 *
 * Instructions:
 *   1. Connect an external speaker (4Ω–8Ω, up to 3W) to the
 *      SPK+ / SPK- terminal block on the board.
 *   2. Upload this sketch.
 *   3. Open Tools → Serial Monitor (115200 baud).
 *   4. Observe the frequency info — you will hear:
 *      - A 1 kHz steady tone (2 seconds)
 *      - A rising sweep from 200 Hz → 2 kHz (3 seconds)
 *      - Silence for 5 seconds, then repeat.
 */

#include <driver/i2s.h>
#include <math.h>

 // =============================================================
 // Pin Definitions (onboard MAX98357A)
 // =============================================================
#define SPEAKER_LRCK  21
#define SPEAKER_BCLK  48
#define SPEAKER_DIN   47

// =============================================================
// I²S & Audio Configuration
// =============================================================
const i2s_port_t I2S_PORT = I2S_NUM_1;

const uint32_t SAMPLE_RATE = 44100;       // 44.1 kHz CD-quality
const uint16_t DMA_BUF_LEN = 256;         // samples per DMA buffer
const uint8_t  DMA_BUF_COUNT = 8;         // number of DMA buffers

const uint16_t AMPLITUDE = 28000;         // 16-bit amplitude (avoid clipping)
int16_t        sampleBuf[DMA_BUF_LEN];    // pre-computed sample buffer

// =============================================================
// Tone / Sweep Timing (milliseconds)
// =============================================================
const uint32_t TONE_DURATION_MS = 2000;  // steady tone
const uint32_t SWEEP_DURATION_MS = 3000;  // rising sweep
const uint32_t SILENCE_DURATION_MS = 5000;

// =============================================================
// Fill a 16-bit signed buffer with a sine wave at a given
// frequency.  Returns how many samples were actually written.
// =============================================================
uint16_t fillSine(int16_t* buf, uint16_t len, uint32_t sampleRate,
    uint32_t freq, uint16_t amplitude) {
    for (uint16_t i = 0; i < len; i++) {
        float phase = 2.0f * M_PI * freq / sampleRate;
        buf[i] = (int16_t)(amplitude * sinf(phase * i));
    }
    return len;
}

// =============================================================
// Write samples to the I²S bus (blocking helper)
// =============================================================
void playBuffer(int16_t* buf, uint16_t len) {
    size_t bytesWritten = 0;
    i2s_write(I2S_PORT, buf, len * sizeof(int16_t), &bytesWritten, portMAX_DELAY);
}

// =============================================================
// Play a steady tone for a given duration (ms).
// =============================================================
void playTone(uint32_t freq, uint32_t durationMs) {
    uint32_t totalSamples = (SAMPLE_RATE * durationMs) / 1000;
    uint32_t remaining = totalSamples;

    while (remaining > 0) {
        uint16_t chunk = (remaining > DMA_BUF_LEN) ? DMA_BUF_LEN : remaining;
        fillSine(sampleBuf, chunk, SAMPLE_RATE, freq, AMPLITUDE);
        playBuffer(sampleBuf, chunk);
        remaining -= chunk;
    }
}

// =============================================================
// Play a frequency sweep from startFreq to endFreq over
// a given duration (ms).
// =============================================================
void playSweep(uint32_t startFreq, uint32_t endFreq, uint32_t durationMs) {
    uint32_t totalSamples = (SAMPLE_RATE * durationMs) / 1000;

    for (uint32_t i = 0; i < totalSamples; i += DMA_BUF_LEN) {
        uint16_t chunk = (totalSamples - i > DMA_BUF_LEN) ? DMA_BUF_LEN : (totalSamples - i);

        // linearly interpolate frequency across the sweep
        float t = (float)i / totalSamples;
        uint32_t freq = startFreq + (uint32_t)((endFreq - startFreq) * t);

        fillSine(sampleBuf, chunk, SAMPLE_RATE, freq, AMPLITUDE);
        playBuffer(sampleBuf, chunk);

        // Print current sweep frequency once per DMA buffer
        Serial.print(">Sweep Freq: ");
        Serial.print(freq);
        Serial.println(" Hz");
    }
}

// =============================================================
// Setup
// =============================================================
void setup() {
    Serial.begin(115200);
    while (!Serial) { ; }
    Serial.println("\n=== MAX98357A Speaker Test ===");
    Serial.println("Connect a speaker to SPK+ / SPK-");
    Serial.print("Sample Rate: "); Serial.print(SAMPLE_RATE); Serial.println(" Hz");
    Serial.println();

    // ---------- I²S driver configuration ----------
    i2s_config_t i2sConfig = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = DMA_BUF_COUNT,
        .dma_buf_len = DMA_BUF_LEN,
        .use_apll = false,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0
    };

    esp_err_t err = i2s_driver_install(I2S_PORT, &i2sConfig, 0, NULL);
    if (err != ESP_OK) {
        Serial.printf("I²S INSTALL ERROR: %d\n", err);
        while (1) { ; }
    }

    // ---------- Pin mapping ----------
    i2s_pin_config_t pinConfig = {
        .bck_io_num = SPEAKER_BCLK,
        .ws_io_num = SPEAKER_LRCK,
        .data_out_num = SPEAKER_DIN,
        .data_in_num = I2S_PIN_NO_CHANGE
    };

    err = i2s_set_pin(I2S_PORT, &pinConfig);
    if (err != ESP_OK) {
        Serial.printf("I²S PIN ERROR: %d\n", err);
        while (1) { ; }
    }

    Serial.println("Speaker initialized!");
    Serial.println("Starting test sequence in 1 second...\n");
    delay(1000);
}

// =============================================================
// Loop — repeat tone + sweep + silence
// =============================================================
void loop() {
    // ---- 1. Steady 1 kHz tone ----
    Serial.println(">>> Playing 1 kHz tone (2 s)");
    playTone(1000, TONE_DURATION_MS);

    // ---- 2. Rising sweep: 200 Hz → 2 kHz ----
    Serial.println(">>> Frequency sweep: 200 Hz → 2 kHz (3 s)");
    playSweep(200, 2000, SWEEP_DURATION_MS);

    // ---- 3. Silence ----
    Serial.print(">>> Silence (");
    Serial.print(SILENCE_DURATION_MS / 1000);
    Serial.println(" s)");
    delay(SILENCE_DURATION_MS);

    Serial.println("--- Restarting ---\n");
}
