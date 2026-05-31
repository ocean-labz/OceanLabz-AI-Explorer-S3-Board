# 📡 Communication Guide — WiFi, BLE & USB

This guide covers the wireless and wired communication capabilities of the OceanLabz AI Explorer S3 Board.

---

## Overview

The ESP32-S3 features:
- **WiFi 4 (802.11 b/g/n)** — 2.4 GHz, up to 150 Mbps
- **Bluetooth 5.0 LE** — BLE, Mesh
- **Native USB Serial/JTAG** — Built-in USB-serial bridge (no external chip needed)
- **I²C / SPI / UART** — General-purpose communication buses

---

## WiFi

### Specifications
| Feature | Value |
|---------|-------|
| Standard | 802.11 b/g/n |
| Frequency | 2.4 GHz |
| Max Data Rate | 150 Mbps (HT40) |
| Modes | Station (STA), Access Point (AP), STA+AP |

### Basic WiFi Usage
```cpp
#include <WiFi.h>

const char* ssid = "YourSSID";
const char* password = "YourPassword";

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected! IP: " + WiFi.localIP().toString());
}
```

### WiFi Tips
- Use **2.4 GHz only** — ESP32-S3 does not support 5 GHz
- For best range, keep the board in open air (not inside a metal enclosure)
- When using WiFi + Camera, reduce camera resolution to QVGA for stable streaming

---

## Bluetooth LE (BLE)

### Specifications
| Feature | Value |
|---------|-------|
| Standard | Bluetooth 5.0 LE |
| Range | ~100m (open air) |
| Data Rate | Up to 1.3 Mbps (LE 2M PHY) |
| Profiles | GATT Server/Client |

### Basic BLE Usage
```cpp
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

void setup() {
    BLEDevice::init("AI Explorer S3");
    BLEServer *pServer = BLEDevice::createServer();
    // ... create services and characteristics
}
```

### BLE Tips
- BLE and WiFi can operate simultaneously (shared antenna)
- BLE uses less power than WiFi — good for sensor node applications
- Use BLE for control commands, WiFi for data streaming

---

## USB Serial

The ESP32-S3 has **native USB support** — no external USB-to-UART chip is required.

### Serial Configuration
Set in Arduino IDE: **Tools** → **USB CDC On Boot** → **Enabled**

```cpp
void setup() {
    Serial.begin(115200);  // USB serial
    delay(1000);
    Serial.println("USB Serial Ready!");
}
```

### Serial Tips
- Set baud rate to **115200** for reliable communication
- `USB CDC On Boot: Enabled` allows the board to show up as a COM port immediately
- For high-speed data transfer (> 2 Mbps), use the **USB JTAG/Serial** interface directly

---

## I²C Bus

| Property | Value |
|----------|-------|
| Pins | SDA=GPIO43, SCL=GPIO44 |
| Speed | 100 kHz (standard), 400 kHz (fast) |
| Shared with | SPI TFT (CLK/MOSI) |

> ⚠️ The I²C bus shares pins with the SPI TFT display. Do not connect both an I²C OLED and an SPI TFT at the same time.

### Scanning for I²C Devices
```cpp
#include <Wire.h>

void setup() {
    Wire.begin(43, 44);  // SDA, SCL
    Serial.begin(115200);
    
    for (byte addr = 1; addr < 127; addr++) {
        Wire.beginTransmission(addr);
        if (Wire.endTransmission() == 0) {
            Serial.printf("Found device at 0x%02X\n", addr);
        }
    }
}
```

---

## SPI Bus (TFT)

| Property | Value |
|----------|-------|
| Pins | MOSI=44, CLK=43, CS=45, DC=38 |
| Speed | Up to 40 MHz (ESP32-S3) |
| Exclusive to | SPI TFT Display |

---

## Troubleshooting

| Problem | Solution |
|---------|----------|
| WiFi won't connect | Check SSID/password; ensure 2.4 GHz band; reboot router |
| Serial Monitor blank | Set baud to 115200; enable USB CDC On Boot |
| BLE not advertising | Call `BLEDevice::init()` before creating server |
| I²C no devices found | Check pull-up resistors; verify SDA/SCL wiring |
| WiFi + BLE interference | Use different antennas; reduce transmit power |