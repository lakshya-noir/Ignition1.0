#include <Wire.h>
#include <WiFi.h>
#include <WebSocketsServer.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <math.h>

// ------------------- MPU6050 -------------------
const int MPU_ADDR = 0x68;
int16_t accelX, accelY, accelZ;
int16_t gyroX, gyroY, gyroZ;
float tiltAngle = 0.0;
float brakingForce = 0.0;
float prevY = 0.0;
float yDisp = 0.0;

// ------------------- GPS -------------------
HardwareSerial GPS_Serial(2); // UART2
TinyGPSPlus gps;
double prevLat = 0, prevLon = 0;
float distanceTravelled = 0.0;
double gpsLat = 0, gpsLon = 0;
float gpsSpeed = 0.0;

// ------------------- WiFi -------------------
const char* ssid = "ESP32_Test";
const char* password = "12345678";

// ------------------- WebSocket -------------------
WebSocketsServer webSocket(81);

// ------------------- Timing -------------------
unsigned long prevMillis = 0;
const int sendInterval = 1000; // 1 second

// ------------------- Activity Probabilities -------------------
float probWalking = 0.0;
float probRunning = 0.0;
float probVehicle = 0.0;

// ------------------- Setup -------------------
void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); // SDA, SCL
  GPS_Serial.begin(9600, SERIAL_8N1, 16, 17); // RX=16, TX=17
  delay(100);

  // Wake up MPU6050
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  Serial.println("MPU6050 Initialized!");

  // WiFi AP
  WiFi.softAP(ssid, password);
  Serial.print("ESP32 AP IP: "); Serial.println(WiFi.softAPIP());

  // WebSocket
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

// ------------------- Main Loop -------------------
void loop() {
  webSocket.loop();
  readGPS();

  if (millis() - prevMillis >= sendInterval) {
    prevMillis = millis();
    readMPU6050();
    computeActivityProbabilities();
    sendData();
  }
}

// ------------------- Read MPU6050 -------------------
void readMPU6050() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 14, true);

  accelX = (Wire.read() << 8) | Wire.read();
  accelY = (Wire.read() << 8) | Wire.read();
  accelZ = (Wire.read() << 8) | Wire.read();

  Wire.read(); Wire.read(); // temp
  gyroX = (Wire.read() << 8) | Wire.read();
  gyroY = (Wire.read() << 8) | Wire.read();
  gyroZ = (Wire.read() << 8) | Wire.read();

  float axg = accelX / 16384.0;
  float ayg = accelY / 16384.0;
  float azg = accelZ / 16384.0;

  tiltAngle = atan2(axg, sqrt(ayg*ayg + azg*azg)) * 57.2958; // degrees
  brakingForce = abs(axg * 9.8); // m/s²

  // Y-coordinate displacement
  yDisp = (accelY - prevY) / 16384.0; // meters
  prevY = accelY;
}

// ------------------- Read GPS -------------------
void readGPS() {
  while (GPS_Serial.available() > 0) gps.encode(GPS_Serial.read());

  if (gps.location.isUpdated()) {
    gpsLat = gps.location.lat();
    gpsLon = gps.location.lng();
    gpsSpeed = gps.speed.mps(); // speed in m/s

    if(prevLat != 0 && prevLon != 0){
      distanceTravelled += TinyGPSPlus::distanceBetween(prevLat, prevLon, gpsLat, gpsLon);
    }
    prevLat = gpsLat;
    prevLon = gpsLon;
  }
}

// ------------------- Compute Activity Probabilities -------------------
void computeActivityProbabilities() {
  probWalking = probRunning = probVehicle = 0.0;

  // Walking probability
  if (gpsSpeed >= 0 && gpsSpeed <= 2.0) probWalking = 100.0;
  else if (gpsSpeed > 2.0 && gpsSpeed <= 3.0) probWalking = 20.0;

  // Running probability
  if (gpsSpeed > 2.0 && gpsSpeed <= 3.0) probRunning = 100.0;
  else if (gpsSpeed >= 0 && gpsSpeed <= 2.0) probRunning = 20.0;

  // Vehicle probability
  if (gpsSpeed > 3.0 || fabs(yDisp) > 0.3) probVehicle = 100.0;
  else if (gpsSpeed > 2.0 && gpsSpeed <= 3.0) probVehicle = 50.0;
}

// ------------------- Send JSON -------------------
void sendData() {
  String jsonData = "{";
  jsonData += "\"tilt\":"+String(tiltAngle,2)+",";
  jsonData += "\"accel\":"+String(sqrt(sq(accelX/16384.0)+sq(accelY/16384.0)+sq(accelZ/16384.0)),2)+",";
  jsonData += "\"brake\":"+String(brakingForce,2)+",";
  jsonData += "\"ychange\":"+String(yDisp,2)+",";
  jsonData += "\"distance\":"+String(distanceTravelled,2)+",";
  jsonData += "\"lat\":"+String(gpsLat,6)+",";
  jsonData += "\"lon\":"+String(gpsLon,6)+",";
  jsonData += "\"gpsSpeed\":"+String(gpsSpeed,2)+",";
  jsonData += "\"probWalking\":"+String(probWalking,2)+",";
  jsonData += "\"probRunning\":"+String(probRunning,2)+",";
  jsonData += "\"probVehicle\":"+String(probVehicle,2)+",";
  jsonData += "\"timestamp\":"+String(millis());
  jsonData += "}";

  webSocket.broadcastTXT(jsonData);
  Serial.println(jsonData);
}

// ------------------- WebSocket Events -------------------
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if(type==WStype_CONNECTED){
    IPAddress ip = webSocket.remoteIP(num);
    Serial.printf("Client %d connected from %s\n", num, ip.toString().c_str());
  } else if(type==WStype_DISCONNECTED){
    Serial.printf("Client %d disconnected\n", num);
  }
}