/**
 * @file    03_hello_world.ino
 * @author  OceanLabz
 * @date    2026
 * @brief   Third example for the OceanLabz AI Explorer S3 Board.
 *
 *          The simplest possible "Hello World" — no external hardware,
 *          no external libraries, no GPIO required. Just a USB cable.
 *
 *          Concepts demonstrated:
 *            • Serial.begin()         — initialise serial communication
 *            • Serial.print()         — print text (no newline)
 *            • Serial.println()       — print text (with newline)
 *            • Serial.available()     — check if data was received
 *            • Serial.readStringUntil() — read a line of text
 *            • millis()               — non-blocking uptime counter
 */

 // ============================================================
 // setup() — runs once on power-up
 // ============================================================
void setup() {
    // Initialise the USB serial port at 115200 baud
    Serial.begin(115200);

    // Give the USB port a moment to enumerate on the host
    delay(500);

    // Print a welcome banner
    Serial.println();
    Serial.println("========================================");
    Serial.println("  OceanLabz AI Explorer S3 Board");
    Serial.println("  Example 03 — Hello World");
    Serial.println("========================================");
    Serial.println();
    Serial.println("Hello, World!");
    Serial.print("Compiled on: ");
    Serial.print(__DATE__);
    Serial.print(" at ");
    Serial.println(__TIME__);
    Serial.println();
    Serial.println("Type something and press Enter — ");
    Serial.println("the board will echo it back.");
    Serial.println("Type 'uptime' to see how long");
    Serial.println("the board has been running.");
    Serial.println();
    Serial.print("> ");
}

// ============================================================
// loop() — runs repeatedly
// ============================================================
void loop() {
    // Wait for the user to type a line and press Enter
    if (Serial.available() > 0) {
        // Read the incoming line
        String input = Serial.readStringUntil('\n');
        input.trim();               // remove leading/trailing whitespace
        input.toLowerCase();        // make it case-insensitive

        Serial.println();           // move past the typed text

        if (input == "uptime") {
            // Print the elapsed time since reset
            unsigned long seconds = millis() / 1000;
            unsigned long minutes = seconds / 60;
            unsigned long hours = minutes / 60;
            seconds %= 60;
            minutes %= 60;

            Serial.print("  Uptime: ");
            if (hours > 0) { Serial.print(hours);   Serial.print("h "); }
            if (minutes > 0) { Serial.print(minutes); Serial.print("m "); }
            Serial.print(seconds);
            Serial.println("s");
        }
        else if (input == "hello") {
            Serial.println("  Hello back! 👋");
        }
        else if (input.length() > 0) {
            // Echo whatever was typed
            Serial.print("  You said: ");
            Serial.println(input);
        }

        Serial.print("> ");
    }
}
