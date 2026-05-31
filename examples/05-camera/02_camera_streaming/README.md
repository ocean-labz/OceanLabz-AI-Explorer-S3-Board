# Camera Streaming

## Description
Stream live video from the camera over WiFi to any web browser on your local network. This example creates a simple web server that serves JPEG frames.

## Required Hardware
- OceanLabz AI Explorer S3 Board
- OV2640 (or OV3660/OV5640) camera module
- WiFi router (2.4 GHz)

## Required Libraries
- ESP32 Camera (built-in)
- WiFi (built-in)

## Wiring
Connect camera via FPC ribbon cable (contacts facing board).

## Steps
1. Set **Tools → PSRAM → OPI PSRAM**
2. Set WiFi credentials in sketch (`ssid` and `password`)
3. Upload `02_camera_streaming.ino`
4. Open Serial Monitor at 115200 baud
5. Note the IP address printed
6. Open a web browser and navigate to `http://<IP_ADDRESS>`

## Expected Output
- Serial Monitor shows: `Streaming ready! http://192.168.x.x`
- Web browser shows live video feed
- Frame rate displayed on page

## Troubleshooting
| Problem | Solution |
|---------|----------|
| No WiFi connection | Check SSID/password; ensure 2.4 GHz band |
| Camera init fails | Enable OPI PSRAM; check ribbon |
| Web page loads but no image | Reduce camera resolution to QVGA |
| Low FPS | Lower resolution; disable JPEG quality |
| Stream stops after a few seconds | Power supply insufficient — use 3A supply |