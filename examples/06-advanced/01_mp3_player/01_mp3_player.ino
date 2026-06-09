/**
 * @file    01_mp3_player.ino
 * @author  OceanLabz
 * @date    2026-06-09
 * @brief   Play MP3 and WAV audio files stored on the microSD card through
 *          the onboard MAX98357A I²S speaker amplifier.
 *
 *          MP3 decoding uses the minimp3 library (single-header, no external
 *          dependency).  WAV files are streamed directly as raw PCM to the
 *          I²S driver.  No third-party Arduino library is required.
 *
 *          The player automatically scans the SD card root directory for
 *          .mp3 and .wav files, builds a playlist sorted alphabetically,
 *          and plays them sequentially — looping back to the first track
 *          after the last one finishes.
 *
 *          IMPORTANT: The minimp3.h library has been patched so that the
 *          ~20 KB mp3dec_scratch_t struct inside mp3dec_decode_frame() is
 *          declared 'static' (lives in global/BSS memory, not the stack).
 *          Playback runs in a dedicated FreeRTOS task with an 8 KB stack,
 *          which is sufficient now that the scratch buffer is off-stack.
 *
 *          Hardware: OceanLabz AI Explorer S3 Board
 *          Module:   MAX98357A I²S speaker amplifier (onboard)
 *                    microSD card slot (SD_MMC 1-bit mode)
 *
 *          Pins (Speaker):
 *            LRCK = GPIO21  (Left-Right Clock / Word Select)
 *            BCLK = GPIO48  (Bit Clock / Serial Clock)
 *            DIN  = GPIO47  (Serial Data In)
 *            I²S port = 1
 *
 *          Pins (SD_MMC 1-bit mode):
 *            CMD  = GPIO3
 *            CLK  = GPIO2
 *            D0   = GPIO46
 *
 *          Instructions:
 *            1. Copy MP3 and/or WAV files to the root of a FAT32-formatted
 *               microSD card (e.g. /song1.mp3, /track2.wav).
 *               WAV: 16-bit or 8-bit PCM, mono or stereo, 8–48 kHz
 *               MP3: any bitrate (128 kbps recommended for quality)
 *            2. Connect an external speaker (4Ω–8Ω, up to 3W) to the
 *               SPK+ / SPK- terminal block on the board.
 *            3. Upload this sketch.
 *            4. Open Tools → Serial Monitor (115200 baud).
 */

 // =============================================================
 // Includes
 // =============================================================
#include <driver/i2s.h>
#include <SD_MMC.h>
#include <FS.h>
#include <vector>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

 // minimp3 — single-header MP3 decoder (public domain)
 // https://github.com/lieff/minimp3
//
// MINIMP3_NO_SIMD  — disable x86 SIMD paths (irrelevant on ESP32,
//                     eliminates detection code & vector locals)
// MINIMP3_ONLY_MP3 — disable Layer 1 / Layer 2 decoding (we only
//                     play .mp3 files, saves ~2 KB of code)
//
// NOTE: minimp3.h has been patched — the mp3dec_scratch_t inside
//       mp3dec_decode_frame() is declared 'static' so the ~20 KB
//       struct lives in global/BSS memory instead of the stack.
//       This is safe because we only call decode from one task.
#define MINIMP3_NO_SIMD
#define MINIMP3_ONLY_MP3
#define MINIMP3_IMPLEMENTATION
#include "minimp3.h"

 // =============================================================
 // Pin Definitions — OceanLabz AI Explorer S3
 // =============================================================
#define SPEAKER_LRCK   21
#define SPEAKER_BCLK   48
#define SPEAKER_DIN    47

#define SD_CMD          3
#define SD_CLK          2
#define SD_D0          46

 // =============================================================
 // I²S Configuration
 // =============================================================
const i2s_port_t  I2S_PORT = I2S_NUM_1;
const uint8_t     DMA_BUF_COUNT = 8;
const uint16_t    DMA_BUF_LEN = 256;

// =============================================================
// Buffer sizes
// =============================================================
const uint16_t    WAV_READ_BUF_SIZE = 1024;
const uint16_t    MP3_BUF_SIZE = 4096;

// =============================================================
// FreeRTOS task — playback runs here with an 8 KB stack.
// The minimp3 scratch buffer (~20 KB) was moved to static/global
// memory by patching minimp3.h, so the task stack only needs to
// cover normal function-call overhead (~2 KB deepest chain).
// =============================================================
const uint32_t    PLAYBACK_STACK_SIZE = 8192;    // 8 KB
const uint8_t     PLAYBACK_TASK_PRIORITY = 2;     // above idle, below WiFi
TaskHandle_t      playbackTaskHandle = NULL;

// =============================================================
// Supported audio file extensions (lowercase)
// =============================================================
const char* AUDIO_EXTENSIONS[] = { ".mp3", ".wav" };
const uint8_t AUDIO_EXT_COUNT = sizeof(AUDIO_EXTENSIONS) / sizeof(AUDIO_EXTENSIONS[0]);

// =============================================================
// WAV header structure (standard 44-byte header)
// =============================================================
struct WavHeader {
    char     riffTag[4];
    uint32_t fileSize;
    char     waveTag[4];
    char     fmtTag[4];
    uint32_t fmtChunkSize;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    char     dataTag[4];
    uint32_t dataSize;
};

// =============================================================
// Global state
// =============================================================
std::vector<String> playlist;
uint8_t  currentTrack = 0;
bool     sdReady = false;
bool     i2sInstalled = false;
uint32_t currentSampleRate = 0;

// =============================================================
// MP3 decoder & buffers — global scope (not on any task stack)
// =============================================================
mp3dec_t mp3dec;
static uint8_t  mp3Buf[MP3_BUF_SIZE];
static int16_t  pcmBuf[MINIMP3_MAX_SAMPLES_PER_FRAME];
static int16_t  monoBuf[1152];

// =============================================================
// Helper — check if a filename is a supported audio file
// =============================================================
bool isAudioFile(const String& filename) {
    String lowerName = filename;
    lowerName.toLowerCase();
    for (uint8_t e = 0; e < AUDIO_EXT_COUNT; e++) {
        if (lowerName.endsWith(AUDIO_EXTENSIONS[e])) {
            return true;
        }
    }
    return false;
}

// =============================================================
// Helper — check if a filename is an MP3 file
// =============================================================
bool isMp3File(const String& filename) {
    String lowerName = filename;
    lowerName.toLowerCase();
    return lowerName.endsWith(".mp3");
}

// =============================================================
// Helper — scan the SD card root directory for audio files
// =============================================================
uint16_t buildPlaylist() {
    playlist.clear();

    File root = SD_MMC.open("/");
    if (!root || !root.isDirectory()) {
        Serial.println(F("  ERROR: Cannot open SD root directory"));
        return 0;
    }

    Serial.println(F("  Scanning SD card for audio files..."));

    File file = root.openNextFile();
    while (file) {
        if (!file.isDirectory()) {
            String name = String(file.name());
            if (!name.startsWith("/")) {
                name = "/" + name;
            }
            if (isAudioFile(name)) {
                playlist.push_back(name);
                Serial.print(F("    Found: "));
                Serial.println(name);
            }
        }
        file = root.openNextFile();
    }

    std::sort(playlist.begin(), playlist.end());

    Serial.print(F("  Total audio files: "));
    Serial.println(playlist.size());
    Serial.println();

    return playlist.size();
}

// =============================================================
// Helper — configure and install the I²S driver
// =============================================================
bool configureI2S(uint32_t sampleRate) {
    if (i2sInstalled && currentSampleRate == sampleRate) {
        return true;
    }

    if (i2sInstalled) {
        i2s_driver_uninstall(I2S_PORT);
        i2sInstalled = false;
    }

    i2s_config_t i2sConfig = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = sampleRate,
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
        Serial.printf("  I²S INSTALL ERROR: %d\n", err);
        return false;
    }

    i2s_pin_config_t pinConfig = {
        .bck_io_num = SPEAKER_BCLK,
        .ws_io_num = SPEAKER_LRCK,
        .data_out_num = SPEAKER_DIN,
        .data_in_num = I2S_PIN_NO_CHANGE
    };

    err = i2s_set_pin(I2S_PORT, &pinConfig);
    if (err != ESP_OK) {
        Serial.printf("  I²S PIN ERROR: %d\n", err);
        return false;
    }

    i2sInstalled = true;
    currentSampleRate = sampleRate;
    return true;
}

// =============================================================
// Helper — write 16-bit mono PCM samples to I²S
// =============================================================
void writeI2S(const int16_t* samples, uint16_t count) {
    size_t bytesWritten = 0;
    i2s_write(I2S_PORT, samples, count * sizeof(int16_t),
        &bytesWritten, portMAX_DELAY);
}

// =============================================================
// Play an MP3 file (runs in playback task context)
// =============================================================
bool playMp3File(const char* filepath) {
    Serial.print(F("  Opening: "));
    Serial.println(filepath);

    File file = SD_MMC.open(filepath, FILE_READ);
    if (!file) {
        Serial.println(F("  ERROR: Cannot open file"));
        return false;
    }

    Serial.print(F("  File Size:    "));
    Serial.print(file.size());
    Serial.println(F(" bytes"));

    mp3dec_init(&mp3dec);

    int bufLen = 0;
    int bufPos = 0;
    bool firstFrame = true;
    uint32_t totalFileBytes = file.size();
    uint32_t fileBytesRead = 0;

    Serial.println(F("  Playing..."));

    while (true) {
        if (bufLen - bufPos < 1024) {
            int remaining = bufLen - bufPos;
            if (remaining > 0 && bufPos > 0) {
                memmove(mp3Buf, mp3Buf + bufPos, remaining);
            }
            bufPos = 0;
            bufLen = remaining;

            int space = MP3_BUF_SIZE - bufLen;
            if (space > 0 && fileBytesRead < totalFileBytes) {
                int toRead = (space > (int)(totalFileBytes - fileBytesRead))
                    ? (int)(totalFileBytes - fileBytesRead) : space;
                int bytesRead = file.read(mp3Buf + bufLen, toRead);
                if (bytesRead > 0) {
                    bufLen += bytesRead;
                    fileBytesRead += bytesRead;
                }
            }
        }

        if (bufLen - bufPos <= 0) break;

        mp3dec_frame_info_t info;
        int samples = mp3dec_decode_frame(&mp3dec,
            mp3Buf + bufPos,
            bufLen - bufPos,
            pcmBuf, &info);

        bufPos += info.frame_bytes;

        if (samples == 0) continue;

        if (firstFrame) {
            Serial.print(F("  Sample Rate:  "));
            Serial.print(info.hz);
            Serial.println(F(" Hz"));
            Serial.print(F("  Channels:     "));
            Serial.println(info.channels);
            Serial.print(F("  Bitrate:      "));
            Serial.print(info.bitrate_kbps);
            Serial.println(F(" kbps"));
            Serial.print(F("  Layer:        "));
            Serial.println(info.layer);

            if (!configureI2S(info.hz)) {
                file.close();
                return false;
            }
            firstFrame = false;
        }

        if (info.channels == 1) {
            writeI2S(pcmBuf, samples);
        }
        else {
            for (int i = 0; i < samples; i++) {
                monoBuf[i] = pcmBuf[i * 2];
            }
            writeI2S(monoBuf, samples);
        }
    }

    Serial.println(F("  Track finished."));
    Serial.println();

    file.close();
    return true;
}

// =============================================================
// Helper — parse WAV header
// =============================================================
bool parseWavHeader(File& file, WavHeader& hdr) {
    if (file.size() < 44) {
        Serial.println(F("  ERROR: File too small for WAV header"));
        return false;
    }

    file.seek(0);

    if (file.read((uint8_t*)hdr.riffTag, 4) != 4) return false;
    if (memcmp(hdr.riffTag, "RIFF", 4) != 0) {
        Serial.println(F("  ERROR: Not a RIFF file"));
        return false;
    }

    file.read((uint8_t*)&hdr.fileSize, 4);

    if (file.read((uint8_t*)hdr.waveTag, 4) != 4) return false;
    if (memcmp(hdr.waveTag, "WAVE", 4) != 0) {
        Serial.println(F("  ERROR: Not a WAVE file"));
        return false;
    }

    if (file.read((uint8_t*)hdr.fmtTag, 4) != 4) return false;
    if (memcmp(hdr.fmtTag, "fmt ", 4) != 0) {
        Serial.println(F("  ERROR: fmt chunk not found"));
        return false;
    }

    file.read((uint8_t*)&hdr.fmtChunkSize, 4);
    file.read((uint8_t*)&hdr.audioFormat, 2);
    file.read((uint8_t*)&hdr.numChannels, 2);
    file.read((uint8_t*)&hdr.sampleRate, 4);
    file.read((uint8_t*)&hdr.byteRate, 4);
    file.read((uint8_t*)&hdr.blockAlign, 2);
    file.read((uint8_t*)&hdr.bitsPerSample, 2);

    if (hdr.fmtChunkSize > 16) {
        file.seek(file.position() + (hdr.fmtChunkSize - 16));
    }

    if (hdr.audioFormat != 1) {
        Serial.printf("  ERROR: Not PCM (format=%u). Only PCM WAV supported.\n",
            hdr.audioFormat);
        return false;
    }

    if (hdr.sampleRate < 8000 || hdr.sampleRate > 48000) {
        Serial.printf("  ERROR: Unsupported sample rate: %u Hz\n",
            hdr.sampleRate);
        return false;
    }

    if (hdr.bitsPerSample != 8 && hdr.bitsPerSample != 16) {
        Serial.printf("  ERROR: Unsupported bit depth: %u\n",
            hdr.bitsPerSample);
        return false;
    }

    char chunkTag[4];
    uint32_t chunkSize;
    bool dataFound = false;

    while (file.available() >= 8) {
        file.read((uint8_t*)chunkTag, 4);
        file.read((uint8_t*)&chunkSize, 4);

        if (memcmp(chunkTag, "data", 4) == 0) {
            memcpy(hdr.dataTag, chunkTag, 4);
            hdr.dataSize = chunkSize;
            dataFound = true;
            break;
        }

        if (chunkSize > 0) {
            file.seek(file.position() + chunkSize);
        }
    }

    if (!dataFound) {
        Serial.println(F("  ERROR: data chunk not found in WAV file"));
        return false;
    }

    return true;
}

// =============================================================
// Play a WAV file (runs in playback task context)
// =============================================================
bool playWavFile(const char* filepath) {
    Serial.print(F("  Opening: "));
    Serial.println(filepath);

    File file = SD_MMC.open(filepath, FILE_READ);
    if (!file) {
        Serial.println(F("  ERROR: Cannot open file"));
        return false;
    }

    WavHeader hdr;
    if (!parseWavHeader(file, hdr)) {
        file.close();
        return false;
    }

    Serial.print(F("  Sample Rate:  "));
    Serial.print(hdr.sampleRate);
    Serial.println(F(" Hz"));
    Serial.print(F("  Channels:     "));
    Serial.println(hdr.numChannels);
    Serial.print(F("  Bits/Sample:  "));
    Serial.println(hdr.bitsPerSample);
    Serial.print(F("  Data Size:    "));
    Serial.print(hdr.dataSize);
    Serial.println(F(" bytes"));

    if (!configureI2S(hdr.sampleRate)) {
        file.close();
        return false;
    }

    uint8_t* readBuf = (uint8_t*)malloc(WAV_READ_BUF_SIZE);
    int16_t* outBuf = (int16_t*)malloc(WAV_READ_BUF_SIZE);
    if (!readBuf || !outBuf) {
        Serial.println(F("  ERROR: Out of memory"));
        free(readBuf); free(outBuf);
        file.close();
        return false;
    }

    uint32_t dataRemaining = hdr.dataSize;

    Serial.println(F("  Playing..."));

    while (dataRemaining > 0) {
        uint16_t bytesToRead = (dataRemaining > WAV_READ_BUF_SIZE)
            ? WAV_READ_BUF_SIZE
            : (uint16_t)dataRemaining;

        uint16_t bytesRead = file.read(readBuf, bytesToRead);
        if (bytesRead == 0) break;

        dataRemaining -= bytesRead;

        uint16_t outSamples = 0;

        if (hdr.bitsPerSample == 16 && hdr.numChannels == 1) {
            uint16_t numFrames = bytesRead / 2;
            memcpy(outBuf, readBuf, numFrames * 2);
            outSamples = numFrames;

        }
        else if (hdr.bitsPerSample == 16 && hdr.numChannels == 2) {
            uint16_t numFrames = bytesRead / 4;
            for (uint16_t i = 0; i < numFrames; i++) {
                outBuf[i] = ((int16_t*)readBuf)[i * 2];
            }
            outSamples = numFrames;

        }
        else if (hdr.bitsPerSample == 8 && hdr.numChannels == 1) {
            uint16_t numFrames = bytesRead;
            for (uint16_t i = 0; i < numFrames; i++) {
                outBuf[i] = (int16_t)((readBuf[i] - 128) * 256);
            }
            outSamples = numFrames;

        }
        else if (hdr.bitsPerSample == 8 && hdr.numChannels == 2) {
            uint16_t numFrames = bytesRead / 2;
            for (uint16_t i = 0; i < numFrames; i++) {
                outBuf[i] = (int16_t)((readBuf[i * 2] - 128) * 256);
            }
            outSamples = numFrames;
        }

        writeI2S(outBuf, outSamples);
    }

    Serial.println(F("  Track finished."));
    Serial.println();

    free(readBuf);
    free(outBuf);
    file.close();
    return true;
}

// =============================================================
// Play a track — dispatches to MP3 or WAV player
// =============================================================
bool playTrack(const char* filepath) {
    String path = String(filepath);
    if (isMp3File(path)) {
        return playMp3File(filepath);
    }
    else {
        return playWavFile(filepath);
    }
}

// =============================================================
// Playback task — runs in a FreeRTOS task with an 8 KB stack.
// The minimp3 scratch buffer was moved to static/global memory
// by patching minimp3.h, so only normal call-chain overhead
// remains on the stack (~2 KB at deepest point).
// =============================================================
void playbackTask(void* parameter) {
    Serial.println(F("Playback task started."));

    // Play first track
    currentTrack = 0;
    playTrack(playlist[currentTrack].c_str());

    // Auto-advance through playlist
    while (true) {
        currentTrack++;
        if (currentTrack >= playlist.size()) {
            currentTrack = 0;
            Serial.println(F("--- Playlist restart ---"));
            Serial.println();
        }

        playTrack(playlist[currentTrack].c_str());

        // Small pause between tracks
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

// =============================================================
// setup() — mount SD card, scan for audio, create playback task
// =============================================================
void setup() {
    Serial.begin(115200);
    while (!Serial) { ; }

    Serial.println();
    Serial.println(F("========================================"));
    Serial.println(F("  OceanLabz AI Explorer S3 Board"));
    Serial.println(F("  Example 01 — MP3 / WAV Player"));
    Serial.println(F("========================================"));
    Serial.println();

    // ---------------------------------------------------------
    // 1. Mount SD card (1-bit mode)
    // ---------------------------------------------------------
    Serial.print(F("Initialising SD_MMC (1-bit mode)... "));
    SD_MMC.setPins(SD_CLK, SD_CMD, SD_D0);

    if (!SD_MMC.begin("/sdcard", true)) {
        Serial.println(F("FAILED"));
        Serial.println();
        Serial.println(F("Troubleshooting:"));
        Serial.println(F("  - Ensure a FAT32-formatted microSD is inserted"));
        Serial.println(F("  - Check that the card is seated properly"));
        Serial.println(F("  - Try re-formatting the card as FAT32"));
        sdReady = false;
        return;
    }
    Serial.println(F("OK"));
    sdReady = true;

    uint64_t cardSizeMB = SD_MMC.cardSize() / (1024 * 1024);
    Serial.print(F("  Card Size: "));
    Serial.print(cardSizeMB);
    Serial.println(F(" MB"));
    Serial.println();

    // ---------------------------------------------------------
    // 2. Scan SD card and build playlist
    // ---------------------------------------------------------
    uint16_t trackCount = buildPlaylist();
    if (trackCount == 0) {
        Serial.println(F("ERROR: No audio files found on SD card"));
        Serial.println(F("  Copy .mp3 or .wav files to the root directory"));
        return;
    }

    // ---------------------------------------------------------
    // 3. Create playback task with 8 KB stack
    // ---------------------------------------------------------
    Serial.print(F("Creating playback task (8 KB stack)... "));

    BaseType_t result = xTaskCreatePinnedToCore(
        playbackTask,              // task function
        "PlayerTask",              // task name
        PLAYBACK_STACK_SIZE,       // stack size in bytes (8 KB)
        NULL,                      // task parameter
        PLAYBACK_TASK_PRIORITY,    // task priority
        &playbackTaskHandle,       // task handle
        1                          // run on Core 1 (app core is on Core 0)
    );

    if (result == pdPASS) {
        Serial.println(F("OK"));
    }
    else {
        Serial.println(F("FAILED"));
        Serial.printf("  xTaskCreatePinnedToCore error: %d\n", result);
    }
}

// =============================================================
// loop() — idle; playback runs in the dedicated FreeRTOS task
// =============================================================
void loop() {
    // Nothing to do here — playback runs in its own FreeRTOS task.
    // We just sleep to keep the loop task alive.
    vTaskDelay(pdMS_TO_TICKS(1000));
}
