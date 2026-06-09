/****************************************************
 *  Project   : ESP32 AI Voice Assistant
 *  Board     : OceanLabz AI Explorer S3
 *  Author    : OceanLabz
 *  Version   : 1.0.0
 *  Date      : 2025-02-10
 *
 *  Description:
 *  --------------------------------------------------------
 *  - Records voice using INMP441 I2S microphone
 *  - Converts speech to text using OpenAI Whisper
 *  - Sends text to ChatGPT for AI response
 *  - Converts AI response to speech using ElevenLabs / OpenAI
 *  - Plays MP3 audio via MAX98357A I2S speaker
 *  - Uses the NEW I2S driver (driver/i2s_std.h) for both mic and speaker
 *    to avoid the legacy-vs-new I2S driver conflict in ESP-IDF 5 / Arduino 3.x
 *
 *  YouTube   : https://youtube.com/@OceanLabz
 ****************************************************/

 // ==================== INCLUDES ====================
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <FS.h>
#include <SD_MMC.h>
#include <ArduinoJson.h>
#include <driver/i2s_std.h>
#include <ArduinoHttpClient.h>
#include <Adafruit_NeoPixel.h>

// Audio libraries for MP3 decoding
#include "AudioFileSource.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"


// ==================== PIN DEFINITIONS ====================
// OceanLabz AI Explorer S3 Board
// See tools/pin_configuration/pins_ai_explorer_s3.h

// ----- SD Card (SD_MMC 1-bit mode) -----
#define SD_CMD       3
#define SD_CLK       2
#define SD_D0       46

// ----- MAX98357A Speaker (I2S Output, port 1) -----
#define I2S_BCLK    48
#define I2S_LRC     21
#define I2S_DOUT    47

// ----- INMP441 Microphone (I2S Input, port 0) -----
#define I2S_MIC_SERIAL_CLOCK     40
#define I2S_MIC_LEFT_RIGHT_CLOCK 39
#define I2S_MIC_SERIAL_DATA      41

// ----- WS2812B RGB LED -----
#define PIN_LED     14

// Recording settings
#define RECORDING_DURATION 6             // seconds
#define SAMPLE_RATE 16000               // Hz
#define BUFFER_SIZE 1024

// ==================== CONFIGURATION ====================
const char* ssid = "OL internet";
const char* password = "Online@1234";

const char* openaiApiKey = "YOUR_API_KEY";
// const char* elevenLabsApiKey = "sk_137a28120cd1594b49abe3e33d5a7997a0eff248cb7d4ded";
const char* elevenLabsApiKey = "YOUR_API_KEY";

// API URLs
const char* openaiChatUrl = "https://api.openai.com/v1/chat/completions";
const char* openaiTtsUrl = "https://api.openai.com/v1/audio/speech";
const char* openaiSttUrl = "https://api.openai.com/v1/audio/transcriptions";
const char* elevenLabsTtsUrl = "https://api.elevenlabs.io/v1/text-to-speech/";

const char* voiceId = "21m00Tcm4TlvDq8ikWAM";


// =============================================================
// Custom AudioFileSource for SD_MMC (replaces AudioFileSourceSD
// which is hardcoded to use SD.h / SPI SD)
// =============================================================
class AudioFileSourceSDMMC : public AudioFileSource {
public:
    AudioFileSourceSDMMC() {}
    AudioFileSourceSDMMC(const char* filename) {
        open(filename);
    }
    virtual ~AudioFileSourceSDMMC() override {
        if (f) {
            f.close();
        }
    }

    virtual bool open(const char* filename) override {
        f = SD_MMC.open(filename, FILE_READ);
        return f ? true : false;
    }

    virtual uint32_t read(void* data, uint32_t len) override {
        if (!f) return 0;
        return f.read(reinterpret_cast<uint8_t*>(data), len);
    }

    virtual bool seek(int32_t pos, int dir) override {
        if (!f) return false;
        if (dir == SEEK_SET) {
            return f.seek(pos, SeekSet);
        }
        else if (dir == SEEK_CUR) {
            return f.seek(pos, SeekCur);
        }
        else if (dir == SEEK_END) {
            return f.seek(pos, SeekEnd);
        }
        return false;
    }

    virtual bool close() override {
        if (f) {
            f.close();
        }
        return true;
    }

    virtual bool isOpen() override {
        return f ? true : false;
    }

    virtual uint32_t getSize() override {
        if (!f) return 0;
        return f.size();
    }

    virtual uint32_t getPos() override {
        if (!f) return 0;
        return f.position();
    }

private:
    File f;
};


// ==================== GLOBALS ====================
AudioGeneratorMP3* mp3 = nullptr;
AudioFileSourceSDMMC* file = nullptr;
AudioOutputI2S* out = nullptr;

// Microphone uses the NEW I2S driver (i2s_std) on port I2S_NUM_0
// Speaker uses AudioOutputI2S which internally uses the NEW I2S driver on I2S_NUM_1
// Both on same driver family -> no conflict
i2s_chan_handle_t mic_handle = nullptr;

Adafruit_NeoPixel* led = nullptr;

bool gettingResponse = false;
bool recordingMode = false;


// =============================================================
// LED helper
// =============================================================
void ledSet(uint8_t r, uint8_t g, uint8_t b) {
    if (!led) return;
    led->setPixelColor(0, led->Color(r, g, b));
    led->show();
}


// =============================================================
// JSON string escaping utility
// =============================================================
String escapeJsonString(String input) {
    input.replace("\\", "\\\\");
    input.replace("\"", "\\\"");
    input.replace("\n", "\\n");
    input.replace("\r", "\\r");
    input.replace("\t", "\\t");
    return input;
}


// =============================================================
// Initializes and mounts the SD card via SD_MMC
// AI Explorer S3 uses SD_MMC (native SD/MMC controller)
// Pins: CMD=3, CLK=2, D0=46 (1-bit mode)
// =============================================================
bool setupSDCard() {
    // Set SD_MMC pins for 1-bit mode (CMD, CLK, D0)
    SD_MMC.setPins(SD_CLK, SD_CMD, SD_D0);

    if (!SD_MMC.begin("/sdcard", true)) {
        Serial.println("SD_MMC card mount failed!");
        return false;
    }

    uint8_t cardType = SD_MMC.cardType();
    if (cardType == CARD_NONE) {
        Serial.println("No SD_MMC card attached");
        return false;
    }

    Serial.print("SD_MMC Card Type: ");
    if (cardType == CARD_MMC) Serial.println("MMC");
    else if (cardType == CARD_SD) Serial.println("SDSC");
    else if (cardType == CARD_SDHC) Serial.println("SDHC");
    else Serial.println("UNKNOWN");

    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
    Serial.printf("SD_MMC Card Size: %lluMB\n", cardSize);

    return true;
}


// =============================================================
// Connects ESP32 to a WiFi network
// =============================================================
void connectToWiFi() {
    Serial.print("Connecting to WiFi");
    WiFi.begin(ssid, password);
    ledSet(8, 4, 0);  // orange = connecting

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 40) {
        delay(500);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi connected!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        ledSet(0, 8, 0);  // green = connected
    }
    else {
        Serial.println("\nWiFi connection failed!");
        ledSet(8, 0, 0);  // red = error
    }
}


// =============================================================
// Initialises the microphone I2S channel using the NEW I2S driver
// (driver/i2s_std.h). Allocates and configures the channel on I2S_NUM_0.
// Call this once in setup; installI2SMic() is called at each capture.
// =============================================================
void setupI2SMicrophone() {
    // --- Channel config (port 0, master RX) ---
    // NOTE: designated initializers must match struct declaration order (C++ rule)
    // i2s_chan_config_t order: id, role, dma_desc_num, dma_frame_num,
    //                          auto_clear (union alias for auto_clear_after_cb),
    //                          auto_clear_before_cb, allow_pd, intr_priority
    i2s_chan_config_t chan_cfg = {
        .id = I2S_NUM_0,
        .role = I2S_ROLE_MASTER,
        .dma_desc_num = 8,
        .dma_frame_num = 1024,
        .auto_clear = true,             // union alias for auto_clear_after_cb
        .auto_clear_before_cb = false,
        .allow_pd = false,
        .intr_priority = 0,
    };

    esp_err_t err = i2s_new_channel(&chan_cfg, NULL, &mic_handle);
    if (err != ESP_OK) {
        Serial.printf("i2s_new_channel failed: %d\n", err);
        mic_handle = nullptr;
        return;
    }

    // --- std mode config ---
    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(
            I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = (gpio_num_t)I2S_MIC_SERIAL_CLOCK,
            .ws = (gpio_num_t)I2S_MIC_LEFT_RIGHT_CLOCK,
            .dout = I2S_GPIO_UNUSED,
            .din = (gpio_num_t)I2S_MIC_SERIAL_DATA,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };

    err = i2s_channel_init_std_mode(mic_handle, &std_cfg);
    if (err != ESP_OK) {
        Serial.printf("i2s_channel_init_std_mode failed: %d\n", err);
        i2s_del_channel(mic_handle);
        mic_handle = nullptr;
        return;
    }

    Serial.println("I2S Microphone configured (new driver, I2S_NUM_0)");
}


// =============================================================
// Enables (installs) the microphone channel ready for capture.
// Must be called before each recording/test session.
// =============================================================
bool installI2SMic() {
    if (!mic_handle) {
        Serial.println("Mic handle not initialised");
        return false;
    }
    if (i2s_channel_enable(mic_handle) != ESP_OK) {
        Serial.println("i2s_channel_enable failed");
        return false;
    }
    return true;
}


// =============================================================
// Disables the microphone channel to free resources.
// Call after each recording/test session.
// =============================================================
void uninstallI2SMic() {
    if (mic_handle) {
        i2s_channel_disable(mic_handle);
    }
}


// =============================================================
// Performs a basic microphone functionality test
// =============================================================
void testMicrophone() {
    Serial.println("\n=== Testing Microphone ===");

    if (!installI2SMic()) return;
    delay(100);

    Serial.println("Listening for audio input... Speak into microphone!");

    int32_t sample;
    size_t bytes_read;
    unsigned long start = millis();
    int non_zero_count = 0;

    while (millis() - start < 3000) {
        if (i2s_channel_read(mic_handle, (char*)&sample, sizeof(sample),
            &bytes_read, 100) == ESP_OK) {
            if (bytes_read == sizeof(sample)) {
                // INMP441: 24-bit data left-aligned in 32-bit frame
                // Shift right by 16 for proper 16-bit PCM
                int16_t sample_16bit = (int16_t)(sample >> 16);
                if (abs(sample_16bit) > 100) {
                    non_zero_count++;
                }
            }
        }
        delay(10);
    }

    if (non_zero_count > 0) {
        Serial.printf("Microphone test PASSED: %d audio events detected\n", non_zero_count);
    }
    else {
        Serial.println("Microphone test FAILED: No audio detected");
    }

    uninstallI2SMic();
}


// =============================================================
// Performs an advanced microphone diagnostic test
// =============================================================
void testMicrophoneDetailed() {
    Serial.println("\n=== Detailed Microphone Test ===");

    if (!installI2SMic()) return;
    delay(500);

    Serial.println("I2S driver installed successfully");
    Serial.println("Reading raw I2S data for 3 seconds...");

    int32_t samples[100];
    size_t bytes_read;
    unsigned long start = millis();
    int total_samples = 0;
    int non_zero_samples = 0;
    int max_amplitude = 0;

    while (millis() - start < 3000) {
        if (i2s_channel_read(mic_handle, (char*)samples, sizeof(samples),
            &bytes_read, 100) == ESP_OK) {
            int samples_read = bytes_read / sizeof(int32_t);
            total_samples += samples_read;

            for (int i = 0; i < samples_read; i++) {
                if (samples[i] != 0) {
                    non_zero_samples++;
                    int amplitude = abs(samples[i]);
                    if (amplitude > max_amplitude) max_amplitude = amplitude;
                }
            }
        }
    }

    Serial.println("\n=== Test Results ===");
    Serial.printf("Total samples read: %d\n", total_samples);
    Serial.printf("Non-zero samples: %d\n", non_zero_samples);
    Serial.printf("Max amplitude: %d\n", max_amplitude);
    Serial.printf("Zero sample percentage: %.1f%%\n",
        (total_samples - non_zero_samples) * 100.0 / total_samples);

    if (total_samples == 0) {
        Serial.println("CRITICAL: No samples read from I2S");
    }
    else if (non_zero_samples == 0) {
        Serial.println("PROBLEM: All samples are zero");
    }
    else if (max_amplitude < 1000) {
        Serial.println("WARNING: Very low amplitude detected");
    }
    else {
        Serial.println("SUCCESS: Audio data detected!");
    }

    uninstallI2SMic();
}


// =============================================================
// Creates a WAV file header
// =============================================================
void createWavHeader(uint8_t* header, uint32_t sampleRate, uint16_t bitDepth,
    uint16_t channels, uint32_t numSamples) {
    // RIFF header
    header[0] = 'R'; header[1] = 'I'; header[2] = 'F'; header[3] = 'F';
    uint32_t fileSize = numSamples * (bitDepth / 8) * channels + 36;
    header[4] = (fileSize) & 0xFF;
    header[5] = (fileSize >> 8) & 0xFF;
    header[6] = (fileSize >> 16) & 0xFF;
    header[7] = (fileSize >> 24) & 0xFF;
    header[8] = 'W'; header[9] = 'A'; header[10] = 'V'; header[11] = 'E';

    // fmt chunk
    header[12] = 'f'; header[13] = 'm'; header[14] = 't'; header[15] = ' ';
    header[16] = 16; header[17] = 0; header[18] = 0; header[19] = 0;
    header[20] = 1; header[21] = 0; // PCM format
    header[22] = channels & 0xFF; header[23] = (channels >> 8) & 0xFF;
    header[24] = sampleRate & 0xFF;
    header[25] = (sampleRate >> 8) & 0xFF;
    header[26] = (sampleRate >> 16) & 0xFF;
    header[27] = (sampleRate >> 24) & 0xFF;

    uint32_t byteRate = sampleRate * channels * (bitDepth / 8);
    header[28] = byteRate & 0xFF;
    header[29] = (byteRate >> 8) & 0xFF;
    header[30] = (byteRate >> 16) & 0xFF;
    header[31] = (byteRate >> 24) & 0xFF;

    header[32] = channels * (bitDepth / 8);
    header[33] = 0;
    header[34] = bitDepth; header[35] = 0;

    // data chunk
    header[36] = 'd'; header[37] = 'a'; header[38] = 't'; header[39] = 'a';
    uint32_t dataSize = numSamples * channels * (bitDepth / 8);
    header[40] = dataSize & 0xFF;
    header[41] = (dataSize >> 8) & 0xFF;
    header[42] = (dataSize >> 16) & 0xFF;
    header[43] = (dataSize >> 24) & 0xFF;
}


// =============================================================
// Updates WAV header after recording completes
// =============================================================
void updateWavHeader(uint8_t* header, uint32_t actual_samples) {
    uint32_t fileSize = actual_samples * 2 + 36;
    header[4] = fileSize & 0xFF;
    header[5] = (fileSize >> 8) & 0xFF;
    header[6] = (fileSize >> 16) & 0xFF;
    header[7] = (fileSize >> 24) & 0xFF;

    uint32_t dataSize = actual_samples * 2;
    header[40] = dataSize & 0xFF;
    header[41] = (dataSize >> 8) & 0xFF;
    header[42] = (dataSize >> 16) & 0xFF;
    header[43] = (dataSize >> 24) & 0xFF;
}


// =============================================================
// Records audio from the INMP441 microphone to SD card
// =============================================================
bool recordAudioToSD(const char* filename) {
    if (!installI2SMic()) return false;
    delay(100);

    if (SD_MMC.exists(filename)) {
        SD_MMC.remove(filename);
    }

    File wavFile = SD_MMC.open(filename, FILE_WRITE);
    if (!wavFile) {
        Serial.println("Failed to create WAV file");
        uninstallI2SMic();
        return false;
    }

    uint8_t wavHeader[44];
    uint32_t total_samples = SAMPLE_RATE * RECORDING_DURATION;
    createWavHeader(wavHeader, SAMPLE_RATE, 16, 1, total_samples);
    wavFile.write(wavHeader, 44);

    Serial.println("Recording... Speak now!");
    ledSet(8, 0, 8);  // magenta = recording

    const size_t buffer_size = 64;
    int32_t audio_buffer[buffer_size];

    uint32_t samples_written = 0;
    unsigned long start_time = millis();
    uint32_t expected_samples = total_samples;

    while (samples_written < expected_samples) {
        size_t bytes_read = 0;

        esp_err_t result = i2s_channel_read(mic_handle,
            (char*)audio_buffer,
            sizeof(audio_buffer),
            &bytes_read,
            100);

        if (result != ESP_OK && result != ESP_ERR_TIMEOUT) {
            Serial.println("I2S read error");
            break;
        }

        size_t samples_read = bytes_read / sizeof(int32_t);

        for (size_t i = 0; i < samples_read && samples_written < expected_samples; i++) {
            int32_t raw_sample = audio_buffer[i];
            // INMP441: 24-bit data left-aligned in 32-bit frame
            // Shift right by 16 to get proper 16-bit PCM
            int16_t sample_16bit = (int16_t)(raw_sample >> 16);

            // Noise gate — filter out silence/background noise
            if (abs(sample_16bit) < 30) {
                sample_16bit = 0;
            }

            wavFile.write((uint8_t*)&sample_16bit, sizeof(int16_t));
            samples_written++;
        }

        if (millis() - start_time > (RECORDING_DURATION * 1000 + 2000)) {
            break;
        }
    }

    wavFile.flush();

    if (samples_written > 0) {
        wavFile.seek(0);
        updateWavHeader(wavHeader, samples_written);
        wavFile.write(wavHeader, 44);
    }

    wavFile.close();
    uninstallI2SMic();

    Serial.printf("Recorded: %lu samples, File: %s\n", samples_written, filename);
    ledSet(0, 0, 8);  // blue = processing
    return samples_written > 0;
}


// =============================================================
// Converts recorded speech to text using Whisper API
// =============================================================
String speechToTextHttpClient(const char* filename) {
    String response = "";

    File audioFile = SD_MMC.open(filename, FILE_READ);
    if (!audioFile) {
        Serial.println("Failed to open audio file");
        return response;
    }

    WiFiClientSecure wifiClient;
    wifiClient.setInsecure();

    HttpClient client(wifiClient, "api.openai.com", 443);
    client.setHttpResponseTimeout(120000);

    String boundary = "----WebKitFormBoundary" + String(millis());
    String contentType = "multipart/form-data; boundary=" + boundary;

    size_t fileSize = audioFile.size();

    // Calculate content length
    String bodyStart = "--" + boundary + "\r\n";
    bodyStart += "Content-Disposition: form-data; name=\"file\"; filename=\"audio.wav\"\r\n";
    bodyStart += "Content-Type: audio/wav\r\n\r\n";

    String bodyEnd = "\r\n--" + boundary + "\r\n";
    bodyEnd += "Content-Disposition: form-data; name=\"model\"\r\n\r\n";
    bodyEnd += "whisper-1\r\n";
    bodyEnd += "--" + boundary + "--\r\n";

    size_t contentLength = bodyStart.length() + fileSize + bodyEnd.length();

    Serial.printf("Sending HTTPS request, size: %d bytes\n", contentLength);

    client.beginRequest();
    client.post("/v1/audio/transcriptions");
    client.sendHeader("Authorization", "Bearer " + String(openaiApiKey));
    client.sendHeader("Content-Type", contentType);
    client.sendHeader("Content-Length", contentLength);
    client.beginBody();

    client.print(bodyStart);

    const size_t chunkSize = 512;
    uint8_t buffer[chunkSize];
    size_t totalSent = 0;

    while (audioFile.available()) {
        size_t bytesRead = audioFile.read(buffer, chunkSize);
        if (bytesRead > 0) {
            client.write(buffer, bytesRead);
            totalSent += bytesRead;

            if (totalSent % 10240 == 0) {
                Serial.printf("Progress: %d/%d bytes (%.1f%%)\n",
                    totalSent, fileSize, (totalSent * 100.0) / fileSize);
            }
        }
        delay(2);
    }

    client.print(bodyEnd);
    client.endRequest();

    Serial.println("Request sent, waiting for response...");

    int httpCode = client.responseStatusCode();
    String httpResponse = client.responseBody();

    Serial.print("HTTP Code: ");
    Serial.println(httpCode);

    if (httpCode == 200) {
        DynamicJsonDocument doc(2048);
        DeserializationError error = deserializeJson(doc, httpResponse);

        if (!error && doc.containsKey("text")) {
            response = doc["text"].as<String>();
            Serial.println("Transcription: " + response);
        }
        else {
            Serial.println("JSON parsing failed");
        }
    }
    else {
        Serial.print("Error response: ");
        Serial.println(httpResponse);
    }

    audioFile.close();
    return response;
}


// =============================================================
// Builds a ChatGPT request JSON payload
// =============================================================
String buildChatGptPayload(String message) {
    DynamicJsonDocument doc(768);
    doc["model"] = "gpt-4o-mini";

    JsonArray messages = doc.createNestedArray("messages");

    JsonObject sysMsg = messages.createNestedObject();
    sysMsg["role"] = "system";
    sysMsg["content"] = "Please answer questions briefly, responses should not exceed 30 words.";

    JsonObject userMsg = messages.createNestedObject();
    userMsg["role"] = "user";
    userMsg["content"] = message;

    String output;
    serializeJson(doc, output);
    return output;
}


// =============================================================
// Extracts the assistant reply from ChatGPT JSON response
// =============================================================
String processChatGptResponse(String response) {
    DynamicJsonDocument jsonDoc(1024);
    DeserializationError error = deserializeJson(jsonDoc, response);

    if (!error) {
        String outputText = jsonDoc["choices"][0]["message"]["content"];
        // Remove newlines for cleaner output
        outputText.replace("\n", " ");
        return outputText;
    }
    return "";
}


// =============================================================
// Sends a message to the OpenAI Chat Completion API
// =============================================================
String sendMessage(String message) {
    HTTPClient http;
    http.begin(openaiChatUrl);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + String(openaiApiKey));

    String payload = buildChatGptPayload(message);
    int httpResponseCode = http.POST(payload);

    if (httpResponseCode == 200) {
        String response = http.getString();
        return processChatGptResponse(response);
    }

    Serial.printf("ChatGPT HTTP Error: %d\n", httpResponseCode);
    return "";
}


// =============================================================
// Sends user input to ChatGPT and plays the response
// =============================================================
void chatGptCall(String message) {
    Serial.println("Sending request to ChatGPT...");
    gettingResponse = true;

    String response = sendMessage(message);
    Serial.println(response);

    if (response != "") {
        Serial.print("ChatGPT: ");
        Serial.println(response);

        if (response.length() > 0) {
            Serial.println("Speaking recognized text...");
            generateAndPlayAudio(response);
        }
    }
    else {
        Serial.println("Failed to get ChatGPT response");
    }

    gettingResponse = false;
}


// =============================================================
// Generates MP3 audio using a cloud TTS API
// Uses ElevenLabs TTS (primary) with OpenAI TTS as alternative
// =============================================================
bool generateAudio(String text) {
    HTTPClient http;

    // NOTE: Currently using ElevenLabs. Switch to OpenAI TTS if preferred
    String url = String(elevenLabsTtsUrl) + voiceId;

    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("xi-api-key", elevenLabsApiKey);
    http.addHeader("Accept", "audio/mpeg");

    String jsonPayload = "{\"text\":\"" + escapeJsonString(text) +
        "\",\"model_id\":\"eleven_multilingual_v2\"," +
        "\"voice_settings\":{\"stability\":0.5,\"similarity_boost\":0.5}}";

    int httpCode = http.POST(jsonPayload);

    if (httpCode == HTTP_CODE_OK) {
        int audioSize = http.getSize();

        if (audioSize > 0) {
            // Create unique filename
            String filename = "/audio_response.mp3";

            // Open file for writing
            File audioFile = SD_MMC.open(filename.c_str(), FILE_WRITE);
            if (!audioFile) {
                Serial.println("Failed to open file for writing");
                http.end();
                return false;
            }

            // Read audio data and write to file
            WiFiClient* stream = http.getStreamPtr();
            size_t bytesRead = 0;
            uint8_t buffer[1024];

            while (http.connected() && bytesRead < (size_t)audioSize) {
                size_t available = stream->available();
                if (available) {
                    size_t toRead = min(available, sizeof(buffer));
                    size_t read = stream->readBytes(buffer, toRead);
                    if (read > 0) {
                        audioFile.write(buffer, read);
                        bytesRead += read;
                    }
                }
                delay(1);
            }

            audioFile.close();
            Serial.printf("Audio saved: %s (%d bytes)\n", filename.c_str(), bytesRead);

            http.end();
            return true;
        }
    }
    else {
        Serial.printf("HTTP Error: %d\n", httpCode);
        Serial.println(http.getString());

        // Fallback: try OpenAI TTS if ElevenLabs fails
        Serial.println("Trying OpenAI TTS as fallback...");
        http.end();

        // OpenAI TTS fallback
        HTTPClient http2;
        http2.begin(openaiTtsUrl);
        http2.addHeader("Content-Type", "application/json");
        http2.addHeader("Authorization", "Bearer " + String(openaiApiKey));

        String openaiPayload = "{\"model\":\"tts-1\",\"input\":\"" +
            escapeJsonString(text) +
            "\",\"voice\":\"alloy\",\"response_format\":\"mp3\",\"speed\":1.0}";

        int code2 = http2.POST(openaiPayload);
        if (code2 == HTTP_CODE_OK) {
            int size2 = http2.getSize();
            if (size2 > 0) {
                String filename2 = "/audio_response.mp3";
                File audioFile2 = SD_MMC.open(filename2.c_str(), FILE_WRITE);
                if (!audioFile2) {
                    Serial.println("Failed to open file for writing");
                    http2.end();
                    return false;
                }

                WiFiClient* stream2 = http2.getStreamPtr();
                size_t bytesRead2 = 0;
                uint8_t buf2[1024];

                while (http2.connected() && bytesRead2 < (size_t)size2) {
                    size_t avail2 = stream2->available();
                    if (avail2) {
                        size_t toRead2 = min(avail2, sizeof(buf2));
                        size_t read2 = stream2->readBytes(buf2, toRead2);
                        if (read2 > 0) {
                            audioFile2.write(buf2, read2);
                            bytesRead2 += read2;
                        }
                    }
                    delay(1);
                }

                audioFile2.close();
                Serial.printf("OpenAI TTS saved: %s (%d bytes)\n", filename2.c_str(), bytesRead2);
                http2.end();
                return true;
            }
        }
        else {
            Serial.printf("OpenAI TTS HTTP Error: %d\n", code2);
            Serial.println(http2.getString());
        }
        http2.end();
    }

    http.end();
    return false;
}


// =============================================================
// Plays an MP3 audio file stored on the SD card
// =============================================================
void playAudioFromSD() {
    String filename = "/audio_response.mp3";

    Serial.printf("Playing: %s\n", filename.c_str());
    ledSet(4, 8, 0);  // yellow-green = playing

    file = new AudioFileSourceSDMMC(filename.c_str());
    if (!file->isOpen()) {
        Serial.println("Failed to open MP3 file");
        cleanupAudio();
        return;
    }

    // Use I2S_NUM_1 for speaker (port 1) to avoid conflict with mic on port 0
    out = new AudioOutputI2S(1, 0, 8);
    out->SetPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    out->SetGain(0.9);

    mp3 = new AudioGeneratorMP3();
    if (!mp3->begin(file, out)) {
        Serial.println("MP3 decoder begin failed");
        cleanupAudio();
        return;
    }

    Serial.println("Playback started");
}


// =============================================================
// Frees all audio-related resources
// =============================================================
void cleanupAudio() {
    if (mp3) { delete mp3; mp3 = nullptr; }
    if (out) { delete out; out = nullptr; }
    if (file) { delete file; file = nullptr; }
    ledSet(0, 8, 0);  // green = ready
}


// =============================================================
// Generates speech audio from text and plays it
// =============================================================
void generateAndPlayAudio(String text) {
    Serial.println("Generating audio...");

    if (generateAudio(text)) {
        Serial.println("Audio generated successfully!");
        playAudioFromSD();
    }
    else {
        Serial.println("Failed to generate audio!");
    }
}


// =============================================================
// Records microphone audio and converts speech to text
// =============================================================
void startRecordingAndTranscription() {
    Serial.println("\n----- Starting Recording -----");
    Serial.println("Please speak... (recording for " + String(RECORDING_DURATION) + " seconds)");

    const char* filename = "/recording.wav";

    if (recordAudioToSD(filename)) {
        Serial.println("Recording completed");
        Serial.println("Converting speech to text...");

        String transcribedText = speechToTextHttpClient(filename);

        // Optional: Delete recording after processing
        // SD_MMC.remove(filename);

        if (transcribedText.length() > 0) {
            Serial.println("\nRecognition result: " + transcribedText);
            Serial.println("\nSending to ChatGPT...");
            chatGptCall(transcribedText);
        }
        else {
            Serial.println("Failed to recognize text or an error occurred.");
        }
    }
    else {
        Serial.println("Failed to record audio!");
    }
}


// =============================================================
// Handles user commands received via Serial
// =============================================================
void handleSerialCommands() {
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');
        command.trim();

        if (command.length() > 0) {
            if (command.equalsIgnoreCase("RECORD")) {
                startRecordingAndTranscription();
            }
            else if (command.startsWith("TTS:")) {
                String ttsText = command.substring(4);
                ttsText.trim();
                if (ttsText.length() > 0) {
                    generateAndPlayAudio(ttsText);
                }
                else {
                    Serial.println("TTS text is empty. Use 'TTS:your text' format");
                }
            }
            else if (command.equalsIgnoreCase("HELP")) {
                Serial.println("\n=== ESP32 Voice Assistant ===");
                Serial.println("Commands:");
                Serial.println("1. Type 'RECORD' to start voice recording and STT");
                Serial.println("2. Type 'TTS:your text' for direct text-to-speech");
                Serial.println("3. Type any text for AI response with speech");
                Serial.println("4. Type 'HELP' for this menu");
            }
            else if (command != "END") {  // Regular text as TTS input
                Serial.print("You: ");
                Serial.println(command);
                Serial.println("Sending to ChatGPT...");
                chatGptCall(command);
            }
        }
    }
}


// =============================================================
// Arduino setup function
// =============================================================
void setup() {
    delay(200);
    Serial.begin(115200);
    delay(200);
    Serial.println("\n=== OceanLabz AI Voice Assistant (AI Explorer S3) ===");

    // Stage 1: NeoPixel LED
    led = new Adafruit_NeoPixel(1, PIN_LED, NEO_GRB + NEO_KHZ800);
    led->begin();
    led->setBrightness(64);
    ledSet(4, 0, 4);  // purple = booting
    Serial.println("Stage 1: LED OK");

    // Stage 2: Initialize SD card (SD_MMC)
    if (!setupSDCard()) {
        Serial.println("SD_MMC Card initialization failed!");
        ledSet(8, 0, 0);  // red
    }
    else {
        Serial.println("Stage 2: SD_MMC Card OK");
    }

    // Stage 3: Connect to WiFi
    connectToWiFi();
    Serial.println("Stage 3: WiFi OK");

    // Stage 4: Initialize I2S microphone (new driver, port 0)
    setupI2SMicrophone();
    Serial.println("Stage 4: I2S Mic configured (new driver)");

    // Stage 5: Run diagnostics
    if (mic_handle) {
        testMicrophone();
        testMicrophoneDetailed();
    }
    else {
        Serial.println("Skipping mic tests — handle unavailable");
    }

    Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());

    Serial.println("\n=== ESP32 Voice Assistant ===");
    Serial.println("Commands:");
    Serial.println("1. Type 'RECORD' to start voice recording and STT");
    Serial.println("2. Type 'TTS:your text' for direct text-to-speech");
    Serial.println("3. Type any text for AI response with speech");
    Serial.println("4. Type 'HELP' for this menu");
    Serial.println("Ready.");
}


// =============================================================
// Main Arduino loop
// =============================================================
void loop() {
    // Handle audio playback
    if (mp3 && mp3->isRunning()) {
        if (!mp3->loop()) {
            mp3->stop();
            Serial.println("Playback finished");
            cleanupAudio();
        }
    }

    // Handle serial commands
    handleSerialCommands();

    delay(10);
}
