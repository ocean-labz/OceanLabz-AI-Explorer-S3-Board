/**
 * @file    02_tft_graphics.ino
 * @author  OceanLabz
 * @date    2026
 * @brief   TFT graphics example for the OceanLabz AI Explorer S3 Board.
 *
 *          Draws shapes, text, and colour fills on a 1.8" SPI TFT display
 *          (ST7735, 128×160) using the Adafruit_ST7735 + Adafruit_GFX libraries.
 *
 *          The sketch cycles through 5 demo screens every 3 seconds to
 *          demonstrate the most common graphics primitives needed for any
 *          display-based project.
 *
 *          Concepts demonstrated:
 *            • Adafruit_ST7735 initialisation (initR / INITR_BLACKTAB)
 *            • tft.fillScreen()        — solid background fill
 *            • tft.fillRect()          — filled rectangle
 *            • tft.drawRect()          — outlined rectangle
 *            • tft.fillCircle()        — filled circle
 *            • tft.drawTriangle()      — outlined triangle
 *            • tft.fillRoundRect()     — rounded rectangle
 *            • tft.drawLine()          — straight line
 *            • tft.setTextSize()       — font scaling
 *            • tft.setTextColor()      — foreground (and optional bg) colour
 *            • tft.setCursor()         — text position
 *            • tft.print() / println() — write text
 *            • tft.width() / tft.height() — screen dimensions
 *            • tft.setRotation()       — screen orientation
 *            • Adafruit_GFX::color565() — 16-bit RGB colour helper
 *            • tft.getTextBounds()     — measure string pixel width for centring
 */

 // ============================================================
 // Includes
 // ============================================================
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

// ============================================================
// Pin Definitions  (AI Explorer S3  ←→  TFT display)
// ============================================================
#define TFT_CS     45
#define TFT_DC     38
#define TFT_RST     1            // GPIO1 / U0TXD — works with Adafruit_ST7735
#define TFT_MOSI   43
#define TFT_SCLK   44
#define TFT_MISO   -1            // not connected on this display
#define TFT_BL     42

// ============================================================
// SPI Bus & Display Object
// ============================================================

// Use the default FSPI (SPI2) peripheral — this works correctly
// on ESP32-S3 with Adafruit_ST7735.
SPIClass tftSPI(FSPI);

// Create the display object, passing the SPI bus and control pins.
// The RST pin is passed explicitly so initR() can toggle it.
Adafruit_ST7735 tft = Adafruit_ST7735(&tftSPI, TFT_CS, TFT_DC, TFT_RST);

// ============================================================
// Helper — draw centred text (Adafruit_GFX has no built-in)
// ============================================================
void drawCentred(const char* text, int y, uint16_t color, uint8_t size) {
    int16_t x1, y1;
    uint16_t w, h;
    tft.setTextSize(size);
    tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
    tft.setTextColor(color);
    tft.setCursor((tft.width() - w) / 2, y);
    tft.print(text);
}

// ============================================================
// setup() — runs once on power-up
// ============================================================
void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println();
    Serial.println("TFT Graphics — starting...");

    // --- Power the display (backlight on) ---
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
    delay(100);                 // settling time for display controller

    // --- Initialise the SPI bus ---
    // Order: SCK, MISO, MOSI, SS (CS)
    tftSPI.begin(TFT_SCLK, TFT_MISO, TFT_MOSI, TFT_CS);

    // --- Initialise the ST7735 controller ---
    // INITR_BLACKTAB is the correct init sequence for this board's
    // 1.8" blue-colour display.
    tft.initR(INITR_BLACKTAB);

    // Set screen rotation:
    //   0 = portrait (USB at top),   1 = landscape (USB left),
    //   2 = portrait (USB at bottom), 3 = landscape (USB right)
    tft.setRotation(1);

    tft.fillScreen(ST77XX_BLACK);

    Serial.print("Screen size: ");
    Serial.print(tft.width());
    Serial.print(" x ");
    Serial.println(tft.height());

    // --- Splash screen for 2 seconds ---
    drawCentred("OceanLabz AI", 30, ST77XX_CYAN, 2);
    drawCentred("Explorer S3", 60, ST77XX_CYAN, 2);
    drawCentred("TFT Graphics Demo", 90, ST77XX_WHITE, 1);
    delay(2000);

    Serial.println("Ready. Cycling through graphics...");
    Serial.println();
}

// ============================================================
// loop() — runs repeatedly
// ============================================================
void loop() {
    static int step = 0;

    tft.fillScreen(ST77XX_BLACK);

    switch (step) {
    case 0:  drawColourBars();          break;
    case 1:  drawShapes();              break;
    case 2:  drawTextDemo();            break;
    case 3:  drawLinesAndGradient();    break;
    case 4:  drawCheckerboard();        break;
    default: step = -1;                  break;
    }

    // Show step label at bottom
    tft.setTextSize(1);
    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(4, tft.height() - 12);
    tft.print("Demo ");
    tft.print(step + 1);
    tft.print(" / 5");

    step++;
    delay(3000);    // show each screen for 3 seconds
}

// ============================================================
// Demo 1 — Colour bars (like a test pattern)
// ============================================================
void drawColourBars() {
    int w = tft.width() / 8;
    int h = tft.height();

    uint16_t colours[] = {
        ST77XX_RED, ST77XX_GREEN, ST77XX_BLUE, ST77XX_YELLOW,
        ST77XX_CYAN, ST77XX_MAGENTA, ST77XX_ORANGE, ST77XX_WHITE
    };

    for (int i = 0; i < 8; i++) {
        tft.fillRect(i * w, 0, w, h, colours[i]);
    }

    // Overlay text
    drawCentred("Colour Bars", h / 2 - 10, ST77XX_BLACK, 2);
}

// ============================================================
// Demo 2 — Geometric shapes
// ============================================================
void drawShapes() {
    int cx = tft.width() / 2;
    int cy = tft.height() / 2;

    // Filled rounded rectangle (border)
    tft.fillRoundRect(10, 10, tft.width() - 20, 50, 8, ST77XX_BLUE);
    tft.drawRoundRect(10, 10, tft.width() - 20, 50, 8, ST77XX_WHITE);

    // Filled rectangle
    tft.fillRect(10, 70, 80, 60, ST77XX_RED);

    // Outlined rectangle
    tft.drawRect(100, 70, 50, 60, ST77XX_GREEN);

    // Filled circle
    tft.fillCircle(cx, cy + 40, 30, ST77XX_YELLOW);

    // Triangle
    tft.drawTriangle(
        10, tft.height() - 10,
        cx, tft.height() - 60,
        tft.width() - 10, tft.height() - 10,
        ST77XX_CYAN
    );

    // Rounded rect in centre
    tft.fillRoundRect(cx - 40, cy - 15, 80, 30, 6, ST77XX_MAGENTA);
    tft.setTextSize(1);
    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(cx - 30, cy - 8);
    tft.print("Shapes");
}

// ============================================================
// Demo 3 — Text sizes and colours
// ============================================================
void drawTextDemo() {
    int y = 8;

    // Size 1
    tft.setTextSize(1);
    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(4, y);
    tft.println("Size 1: ABC abc 123");
    y += 14;

    // Size 2
    tft.setTextSize(2);
    tft.setTextColor(ST77XX_CYAN);
    tft.setCursor(4, y);
    tft.println("Size 2");
    y += 28;

    // Size 3
    tft.setTextSize(3);
    tft.setTextColor(ST77XX_YELLOW);
    tft.setCursor(4, y);
    tft.println("S3");
    y += 38;

    // Centred text using getTextBounds helper
    y += 4;
    drawCentred("OceanLabz AI", y, ST77XX_GREEN, 1);
    y += 14;

    // Multi-colour text — red on yellow bg
    tft.setTextSize(1);
    tft.setTextColor(ST77XX_RED, ST77XX_YELLOW);
    tft.setCursor(4, y);
    tft.print(" Red on Yellow ");
    y += 14;

    // Inverse colour (white text on blue bg)
    tft.setTextColor(ST77XX_WHITE, ST77XX_BLUE);
    tft.setCursor(4, y);
    tft.print(" White on Blue ");
}

// ============================================================
// Demo 4 — Lines and a gradient effect
// ============================================================
void drawLinesAndGradient() {
    int w = tft.width();
    int h = tft.height();

    // Horizontal lines from top to bottom forming a colour gradient
    for (int y = 0; y < h; y += 6) {
        uint16_t col = tft.color565(y * 4, 0, h * 4 - y * 4);
        tft.drawLine(0, y, w, y, col);
    }

    // Cross-hair
    tft.drawLine(w / 2, 0, w / 2, h, ST77XX_WHITE);
    tft.drawLine(0, h / 2, w, h / 2, ST77XX_WHITE);

    // Diagonal lines
    tft.drawLine(0, 0, w, h, ST77XX_YELLOW);
    tft.drawLine(w, 0, 0, h, ST77XX_YELLOW);

    // Label
    drawCentred("Lines & Gradient", h / 2 - 4, ST77XX_WHITE, 1);
}

// ============================================================
// Demo 5 — Checkerboard (nested fills / loops)
// ============================================================
void drawCheckerboard() {
    int cols = 8;
    int rows = 6;
    int cw = tft.width() / cols;
    int rh = tft.height() / rows;

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            bool isEven = ((r + c) % 2 == 0);
            tft.fillRect(c * cw, r * rh, cw, rh,
                isEven ? ST77XX_BLUE : ST77XX_YELLOW);
        }
    }

    // Border line
    tft.drawRect(0, 0, tft.width(), tft.height(), ST77XX_RED);
}
