#This is a micropython file
#This code does not work and we are unable to resolve it 

import machine
import network
import urequests
import time
import json
from machine import Pin, I2C, UART
import math

# WiFi Configuration
WIFI_SSID = "OnePlus 7T"
WIFI_PASSWORD = "LOWERCASE"

# Backend Configuration
BACKEND_URL = "http://10.104.142.48:5000/api/data"
API_KEY = "YOUR_API_KEY_HERE"  # Add your actual API key here if required

# MPU6050 (Accelerometer & Gyroscope) Configuration
MPU6050_ADDR = 0x68
MPU6050_PWR_MGMT_1 = 0x6B
MPU6050_ACCEL_XOUT = 0x3B

# GPS Configuration (Neo-6M)
GPS_UART = UART(2, baudrate=9600, tx=17, rx=16)  # Adjust pins as needed

# HC-SR04 (Ultrasonic Sensor) Configuration
TRIG_PIN = 5
ECHO_PIN = 18
trig = Pin(TRIG_PIN, Pin.OUT)
echo = Pin(ECHO_PIN, Pin.IN)

# I2C for MPU6050
i2c = I2C(0, scl=Pin(22), sda=Pin(21), freq=400000)


class SensorCollector:
    def __init__(self):
        self.wifi = network.WLAN(network.STA_IF)
        self.setup_mpu6050()

    def connect_wifi(self):
        """Connect to WiFi"""
        if not self.wifi.isconnected():
            print('Connecting to WiFi...')
            self.wifi.active(True)
            self.wifi.connect(WIFI_SSID, WIFI_PASSWORD)
            
            timeout = 20
            while not self.wifi.isconnected() and timeout > 0:
                time.sleep(1)
                timeout -= 1
                print('.', end='')
            
            if self.wifi.isconnected():
                print('\nWiFi connected!')
                print('Network config:', self.wifi.ifconfig())
            else:
                print('\nFailed to connect to WiFi')
                return False
        return True

    def setup_mpu6050(self):
        """Initialize MPU6050"""
        try:
            # Wake up MPU6050
            i2c.writeto_mem(MPU6050_ADDR, MPU6050_PWR_MGMT_1, b'\x00')
            time.sleep(0.1)
            print("MPU6050 initialized")
        except Exception as e:
            print("MPU6050 initialization failed:", e)

    def read_mpu6050(self):
        """Read acceleration and calculate tilt"""
        try:
            data = i2c.readfrom_mem(MPU6050_ADDR, MPU6050_ACCEL_XOUT, 6)
            accel_x = self.convert_accel(data[0], data[1])
            accel_y = self.convert_accel(data[2], data[3])
            accel_z = self.convert_accel(data[4], data[5])
            
            # Correct pitch and roll calculation
            pitch = math.atan2(accel_y, math.sqrt(accel_x**2 + accel_z**2)) * 180/math.pi
            roll = math.atan2(-accel_x, math.sqrt(accel_y**2 + accel_z**2)) * 180/math.pi
            
            return {
                'accel_x': accel_x,
                'accel_y': accel_y,
                'accel_z': accel_z,
                'pitch': pitch,
                'roll': roll
            }
        except Exception as e:
            print("Error reading MPU6050:", e)
            return None

    def convert_accel(self, high_byte, low_byte):
        """Convert raw accelerometer data to g-forces"""
        value = (high_byte << 8) | low_byte
        if value > 32768:
            value -= 65536
        return value / 16384.0

    def read_gps(self):
        """Read GPS data from Neo-6M"""
        try:
            if GPS_UART.any():
                data = GPS_UART.readline()
                if data:
                    gps_data = data.decode('utf-8').strip()
                    if gps_data.startswith('$GPGGA'):
                        parts = gps_data.split(',')
                        if len(parts) > 6 and parts[6] != '0':
                            return {
                                'latitude': self.parse_latitude(parts[2], parts[3]),
                                'longitude': self.parse_longitude(parts[4], parts[5]),
                                'altitude': parts[9],
                                'satellites': parts[7],
                                'fix_quality': parts[6]
                            }
            return None
        except Exception as e:
            print("Error reading GPS:", e)
            return None

    def parse_latitude(self, lat_str, direction):
        if not lat_str:
            return 0
        degrees = float(lat_str[:2])
        minutes = float(lat_str[2:])
        latitude = degrees + minutes / 60.0
        if direction == 'S':
            latitude = -latitude
        return latitude

    def parse_longitude(self, lon_str, direction):
        if not lon_str:
            return 0
        degrees = float(lon_str[:3])
        minutes = float(lon_str[3:])
        longitude = degrees + minutes / 60.0
        if direction == 'W':
            longitude = -longitude
        return longitude

    def read_distance(self):
        """Read distance from HC-SR04 ultrasonic sensor"""
        try:
            trig.off()
            time.sleep_us(2)
            trig.on()
            time.sleep_us(10)
            trig.off()
            
            timeout = 100000
            while echo.value() == 0 and timeout > 0:
                timeout -= 1
                time.sleep_us(1)
            if timeout <= 0:
                return None
            
            pulse_start = time.ticks_us()
            while echo.value() == 1:
                pulse_end = time.ticks_us()
                if time.ticks_diff(pulse_end, pulse_start) > 38000:
                    br