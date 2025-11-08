#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <MPU6050.h>

const char* SSID = "Galaxy M34 5G E469";
const char* PASS = "00000000";
const char* HOST = "192.168.91.145";

MPU6050 imu;
int16_t smoothAz = 16384;
const float SMOOTH = 0.8f;
const int THRESH = 20000;
unsigned long lastBump = 0;
unsigned long lastPrint = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("STARTING");

  Wire.begin(21, 22);
  imu.initialize();
  Serial.println("IMU INIT DONE");

  Serial.println("WIFI START");
  WiFi.begin(SSID, PASS);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("");

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WIFI OK");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WIFI FAIL");
  }

  Serial.println("READY");
}

void loop() {
  int16_t ax, ay, az;
  imu.getAcceleration(&ax, &ay, &az);

  smoothAz = (int16_t)(SMOOTH * smoothAz + (1.0f - SMOOTH) * az);
  int16_t delta = abs(az - smoothAz);

  unsigned long now = millis();

  if (delta > THRESH && (now - lastBump) > 1000) {
    lastBump = now;
    Serial.println("BUMP");

    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      String url = String("http://") + HOST + ":5000/bump";
      String body = String("{\"time\":") + now + ",\"delta\":" + delta + "}";
      http.begin(url);
      http.addHeader("Content-Type", "application/json");
      int r = http.POST(body);
      Serial.println(r);
      http.end();
    }
  }

  if (now - lastPrint > 500) {
    Serial.print("az="); Serial.print(az);
    Serial.print(" smooth="); Serial.print(smoothAz);
    Serial.print(" delta="); Serial.println(delta);
    lastPrint = now;
  }

  delay(20);
}
