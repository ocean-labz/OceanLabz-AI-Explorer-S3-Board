/**
 * @file    01_oled_hello.ino
 * @author  OceanLabz
 * @date    2026
 * @brief   Display example for the OceanLabz AI Explorer S3 Board.
 *
 *          Prints text on an I²C OLED display (SSD1306 / SH1106).
 *
 *          Concepts demonstrated:
 *            • Adafruit_SSD1306        — OLED driver library
 *            • Adafruit_GFX            — graphics primitives (text, draw)
 *            • Wire.begin()            — I²C bus initialisation
 *            • display.setTextSize()   — font scaling
 *            • display.setTextColor()  — foreground / background colour
 *            • display.setCursor()     — text position (x, y)
 *            • display.print()         — write text to the display buffer
 *            • display.display()       — flush the buffer to the OLED
 *            • display.clearDisplay()  — erase the buffer
 *            • millis() / 1000         — one-second counter
 */

 // ============================================================
 // Includes
 // ============================================================
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ============================================================
// Display Configuration
// ============================================================

// OLED dimensions (128 × 64 pixels)
#define SCREEN_WIDTH    128
#define SCREEN_HEIGHT    64

// I²C pins on the AI Explorer S3
#define OLED_SDA         43
#define OLED_SCL         44

// Most OLED modules use address 0x3C; try 0x3D if yours differs
#define OLED_ADDR        0x3C

// Reset pin: -1 means tied to the same I²C bus (no dedicated RST)
#define OLED_RESET        -1

// Create the display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ============================================================
// setup() — runs once on power-up
// ============================================================
void setup() {
    // --- Serial for debug output ---
    Serial.begin(115200);
    delay(500);
    Serial.println();
    Serial.println("OLED Hello — starting...");

    // --- Initialise I²C bus with our pins ---
    Wire.begin(OLED_SDA, OLED_SCL);

    // --- Initialise the OLED ---
    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
        Serial.println("OLED initialisation FAILED!");
        Serial.print("Check wiring (SDA=");
        Serial.print(OLED_SDA);
        Serial.print(", SCL=");
        Serial.print(OLED_SCL);
        Serial.println(") and address (0x3C / 0x3D).");
        while (true) {
            delay(100);     // halt here if no display found
        }
    }
    Serial.println("OLED initialised OK");

    // Clear the display buffer
    display.clearDisplay();

    // --- Draw a static title that never changes ---
    display.setTextSize(2);             // 2× scale font
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("OceanLabz");
    display.println("AI");
    display.setTextSize(1);
    display.println("Explorer S3");
    display.display();

    delay(1500);    // show the title for 1.5 seconds

    Serial.println("Ready.");
    Serial.println();
}

// ============================================================
// loop() — runs repeatedly
// ============================================================
void loop() {
    static unsigned long lastTick = 0;
    static unsigned int  counter = 0;

    unsigned long now = millis();

    // Update once per second
    if (now - lastTick >= 1000) {
        lastTick = now;
        counter++;

        // --- Update the display ---
        display.clearDisplay();

        // Line 1 — board name
        display.setTextSize(2);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 0);
        display.println("OceanLabz AI");

        // Line 2 — model
        display.setTextSize(1);
        display.setCursor(0, 22);
        display.println("Explorer S3");

        // Line 3 — live counter
        display.setCursor(0, 42);
        display.print("Uptime: ");
        display.print(counter);
        display.println("s");

        // Flush the buffer to the OLED
        display.display();

        // --- Also print to Serial Monitor ---
        Serial.print("Uptime: ");
        Serial.print(counter);
        Serial.println("s");
    }
}
