/**
 * @file    01_blink.ino
 * @author  OceanLabz
 * @date    2024
 * @brief   First example for the OceanLabz AI Explorer S3 Board.
 *
 *          Blinks the onboard RGB LED (WS2812B NeoPixel on GPIO14) with
 *          multiple effects selectable via a single variable.
 *
 *          Effects:
 *            0 — Normal Blink   (1 s on / 1 s off)
 *            1 — Fast Blink     (200 ms on / 200 ms off)
 *            2 — Heartbeat      (double-pulse rhythm)
 *            3 — SOS Morse Code (... --- ...)
 *            4 — Fade           (smooth brightness / colour sweep)
 *            5 — Random Blink   (random colour & interval)
 *            6 — DEMO Mode      (auto-cycle through effects 0-5)
 */

#include <Adafruit_NeoPixel.h>

 // ============================================================
 // Hardware Configuration — DO NOT CHANGE
 // ============================================================
#define LED_PIN         14        // Onboard RGB LED (WS2812B)
#define NUM_LEDS         1        // Single NeoPixel

// Colour-order constant for this board's WS2812B variant
#define LED_TYPE  (NEO_GRB + NEO_KHZ800)

// NeoPixel object
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, LED_TYPE);

// ============================================================
// Effect Selection
// Change `currentEffect` below to try a different effect.
//
//   0 — Normal Blink    1 second on, 1 second off
//   1 — Fast Blink      200 ms on, 200 ms off
//   2 — Heartbeat       Double-thump rhythm
//   3 — SOS Morse       ... --- ...  (international distress)
//   4 — Fade            Smooth brightness & colour cycling
//   5 — Random Blink    Random colour + random interval
//   6 — DEMO            Cycles through effects 0-5 automatically
// ============================================================
static uint8_t currentEffect = 6;          // <<< CHANGE ME (0-6)

// ------------------------------------------------------------
// Named constants for readability
// ------------------------------------------------------------
static const uint8_t EFFECT_NORMAL = 0;
static const uint8_t EFFECT_FAST = 1;
static const uint8_t EFFECT_HEART = 2;
static const uint8_t EFFECT_SOS = 3;
static const uint8_t EFFECT_FADE = 4;
static const uint8_t EFFECT_RANDOM = 5;
static const uint8_t EFFECT_DEMO = 6;

// ------------------------------------------------------------
// Demo-mode timing
// ------------------------------------------------------------
static const unsigned long DEMO_DURATION_MS = 10000;  // 10 s per effect

// ============================================================
// Colour Helper
// ============================================================

/**
 * Return a 32-bit NeoPixel colour from separate RGB bytes.
 * Note: WS2812B on this board uses GRB order internally, so we
 * pass red/green/blue and let the library handle the mapping.
 */
static uint32_t rgb(uint8_t r, uint8_t g, uint8_t b) {
    return strip.Color(r, g, b);
}

// Pre-defined colours
static const uint32_t COL_RED = rgb(255, 0, 0);
static const uint32_t COL_GREEN = rgb(0, 255, 0);
static const uint32_t COL_BLUE = rgb(0, 0, 255);
static const uint32_t COL_WHITE = rgb(80, 80, 80);   // dimmer to protect eyes
static const uint32_t COL_YELLOW = rgb(255, 100, 0);
static const uint32_t COL_CYAN = rgb(0, 255, 100);
static const uint32_t COL_OFF = rgb(0, 0, 0);

// ============================================================
// setup() — runs once on power-up / reset
// ============================================================
void setup() {
    Serial.begin(115200);
    delay(500);                     // allow serial to settle

    Serial.println();
    Serial.println(F("========================================"));
    Serial.println(F("  OceanLabz AI Explorer S3 Board"));
    Serial.println(F("  Example 01 — Blink (RGB LED)"));
    Serial.println(F("========================================"));
    Serial.println();

    // Init NeoPixel
    strip.begin();
    strip.setBrightness(50);         // 0-255; 50 is safe for desktop use
    strip.show();                    // initialise pin to OFF

    // Announce the selected effect
    Serial.print(F("Selected effect: "));
    printEffectName(currentEffect);
    Serial.println();
    Serial.println();
}

// ============================================================
// loop() — runs forever
// ============================================================
void loop() {
    switch (currentEffect) {
    case EFFECT_NORMAL:  effectNormalBlink();  break;
    case EFFECT_FAST:    effectFastBlink();    break;
    case EFFECT_HEART:   effectHeartbeat();    break;
    case EFFECT_SOS:     effectSos();          break;
    case EFFECT_FADE:    effectFade();         break;
    case EFFECT_RANDOM:  effectRandom();       break;
    case EFFECT_DEMO:    effectDemo();         break;
    default:
        // Fallback: if someone sets an out-of-range value
        currentEffect = EFFECT_NORMAL;
        break;
    }
}

// ============================================================
// Effect Implementations
// ============================================================

// ----------------------------------------------------------
// 0 — Normal Blink: 1 second on, 1 second off
// ----------------------------------------------------------
static void effectNormalBlink() {
    static uint32_t prev = 0;
    static bool     on = false;

    if (millis() - prev >= 1000) {
        prev = millis();
        on = !on;
        setLed(on ? COL_GREEN : COL_OFF);
        Serial.println(on ? F("ON") : F("OFF"));
    }
}

// ----------------------------------------------------------
// 1 — Fast Blink: 200 ms on, 200 ms off
// ----------------------------------------------------------
static void effectFastBlink() {
    static uint32_t prev = 0;
    static bool     on = false;

    if (millis() - prev >= 200) {
        prev = millis();
        on = !on;
        setLed(on ? COL_CYAN : COL_OFF);
        if (on) Serial.println(F("fast blink"));
    }
}

// ----------------------------------------------------------
// 2 — Heartbeat: double-pulse, then pause
// ----------------------------------------------------------
static void effectHeartbeat() {
    // Two quick pulses: thump-THUMP … pause … thump-THUMP …
    const uint16_t PULSE_MS = 100;     // single pulse duration
    const uint16_t GAP_MS = 80;      // gap between the two pulses
    const uint16_t REST_MS = 600;     // rest between heartbeats

    // --- First thump (softer) ---
    fadeTo(COL_RED, PULSE_MS / 2, 30, 120);
    fadeTo(COL_OFF, PULSE_MS / 2, 120, 0);
    delay(GAP_MS);

    // --- Second thump (stronger) ---
    fadeTo(COL_RED, PULSE_MS / 2, 50, 255);
    fadeTo(COL_OFF, PULSE_MS / 2, 255, 0);
    delay(REST_MS);

    Serial.println(F("♥"));
}

// ----------------------------------------------------------
// 3 — SOS Morse Code: ... --- ...
// ----------------------------------------------------------
static void effectSos() {
    // Unit = 200 ms; dot = 1 unit, dash = 3 units, gap = 1 unit
    // Letter gap = 3 units, word gap = 7 units
    const uint16_t UNIT = 200;

    // S = dot dot dot
    dot(); dot(); dot();
    delay(UNIT * 3);               // letter gap

    // O = dash dash dash
    dash(); dash(); dash();
    delay(UNIT * 3);

    // S = dot dot dot
    dot(); dot(); dot();
    delay(UNIT * 7);               // word gap before repeat

    Serial.println(F("SOS"));
}

// ----------------------------------------------------------
// 4 — Fade: smooth brightness and colour cycling
// ----------------------------------------------------------
static void effectFade() {
    // Cycle hue from 0 → 360 over 5 seconds
    static uint16_t hue = 0;
    const  uint16_t STEP = 1;

    // Convert HSV to RGB via NeoPixel gamma
    uint32_t color = strip.gamma32(strip.ColorHSV(hue, 255, 200));
    setLed(color);

    hue = (hue + STEP) % 65536;    // 16-bit hue space
    delay(20);                     // ~50 fps → ~6.5 s per full cycle

    // Print hue once per revolution
    if (hue == 0) {
        Serial.println(F("fade cycle complete"));
    }
}

// ----------------------------------------------------------
// 5 — Random Blink: random colour + random on/off interval
// ----------------------------------------------------------
static void effectRandom() {
    static uint32_t prev = 0;
    static uint16_t wait = 500;        // initial interval
    static bool     on = false;
    static uint32_t color = COL_RED;

    if (millis() - prev >= wait) {
        prev = millis();
        on = !on;

        if (on) {
            // Pick a random colour (avoid very dim values)
            color = strip.Color(
                random(50, 256),
                random(50, 256),
                random(50, 256)
            );
            wait = random(80, 800);     // on-time
            setLed(color);
        }
        else {
            wait = random(80, 800);     // off-time
            setLed(COL_OFF);
        }
    }
}

// ----------------------------------------------------------
// 6 — DEMO Mode: cycle through effects 0-5 automatically
// ----------------------------------------------------------
static void effectDemo() {
    static uint8_t  demoIndex = 0;
    static uint32_t demoStart = 0;
    static bool     initialized = false;
    static uint32_t lastPrint = 0;

    if (!initialized) {
        demoStart = millis();
        initialized = true;
        lastPrint = 0;

        Serial.print(F(">>> DEMO: starting effect "));
        printEffectName(demoIndex);
        Serial.println();
    }

    // Run the current effect by temporarily overriding currentEffect
    // and recursively calling loop().  To avoid deep recursion we
    // inline the effect call.
    switch (demoIndex) {
    case 0:  effectNormalBlink(); break;
    case 1:  effectFastBlink();   break;
    case 2:  effectHeartbeat();   break;
    case 3:  effectSos();         break;
    case 4:  effectFade();        break;
    case 5:  effectRandom();      break;
    default: demoIndex = 0;       break;
    }

    // Print remaining time every ~5 seconds
    if (millis() - lastPrint > 5000) {
        lastPrint = millis();
        unsigned long elapsed = millis() - demoStart;
        unsigned long remain = (elapsed < DEMO_DURATION_MS)
            ? (DEMO_DURATION_MS - elapsed) / 1000
            : 0;
        Serial.print(F("  → switching in ~"));
        Serial.print(remain);
        Serial.println(F(" s"));
    }

    // Advance to the next effect when the timer expires
    if (millis() - demoStart >= DEMO_DURATION_MS) {
        // Ensure LED is off before switching
        setLed(COL_OFF);
        delay(500);

        demoIndex = (demoIndex + 1) % 6;   // cycle 0-5 only

        Serial.print(F(">>> DEMO: switching to effect "));
        printEffectName(demoIndex);
        Serial.println();

        demoStart = millis();
        lastPrint = 0;
    }
}

// ============================================================
// Low-level Helpers
// ============================================================

/**
 * Set the single NeoPixel to a colour and show it.
 */
static void setLed(uint32_t color) {
    strip.setPixelColor(0, color);
    strip.show();
}

/**
 * Morse dot: 1 unit on, 1 unit off.
 */
static void dot() {
    setLed(COL_WHITE);
    delay(200);
    setLed(COL_OFF);
    delay(200);
}

/**
 * Morse dash: 3 units on, 1 unit off.
 */
static void dash() {
    setLed(COL_WHITE);
    delay(600);
    setLed(COL_OFF);
    delay(200);
}

/**
 * Smooth brightness fade from `startBright` to `endBright` over
 * `durationMs` milliseconds, using `color`.
 */
static void fadeTo(uint32_t color, unsigned long durationMs,
    uint8_t startBright, uint8_t endBright) {
    // Number of steps = duration / 10 ms (100 Hz update rate)
    unsigned long steps = durationMs / 10;
    if (steps == 0) steps = 1;

    for (unsigned long i = 0; i < steps; i++) {
        uint8_t b = map(i, 0, steps - 1, startBright, endBright);
        strip.setBrightness(b);
        setLed(color);
        delay(10);
    }
}

/**
 * Print a human-readable effect name to the Serial console.
 */
static void printEffectName(uint8_t effect) {
    switch (effect) {
    case 0:  Serial.print(F("Normal Blink"));   break;
    case 1:  Serial.print(F("Fast Blink"));     break;
    case 2:  Serial.print(F("Heartbeat"));      break;
    case 3:  Serial.print(F("SOS Morse"));      break;
    case 4:  Serial.print(F("Fade"));           break;
    case 5:  Serial.print(F("Random Blink"));   break;
    case 6:  Serial.print(F("DEMO (auto-cycle)")); break;
    default: Serial.print(F("Unknown"));        break;
    }
}
