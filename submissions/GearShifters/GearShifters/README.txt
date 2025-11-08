================================================================================
                    RIDER TELEMETRY SYSTEM - README.txt
                        Hackathon Project Guide
================================================================================

TABLE OF CONTENTS
=================
1. Hardware Requirements
2. Software Requirements
3. System Architecture
4. Installation Steps
5. How to Run the System
6. Troubleshooting
7. Data Format
8. Vehicle Detection Logic
9. Evaluation Criteria

================================================================================
1. HARDWARE REQUIREMENTS
================================================================================

- ESP32 Development Board (any ESP32 variant)
- MPU6050 6-axis Accelerometer/Gyroscope Sensor
- Jumper Wires (4 minimum for I2C connection)
- Power Bank or USB Power Supply
- Two Smartphones (Android/iOS with GPS and WiFi)
- USB Cable (for uploading code to ESP32)

WIRING DIAGRAM:
================
MPU6050          ESP32
--------         -----
VCC       →      3.3V
GND       →      GND
SCL       →      GPIO 22
SDA       →      GPIO 21

================================================================================
2. SOFTWARE REQUIREMENTS
================================================================================

ON YOUR COMPUTER:
- Arduino IDE (v1.8 or higher)
- ESP32 Board Support (installed via Board Manager)
- Libraries:
  * MPU6050_tockn
  * ArduinoJson (by Benoit Blanchon)
  * WiFi (pre-installed)
  * WebServer (pre-installed)

ON PHONE 1:
- Chrome Browser (for
