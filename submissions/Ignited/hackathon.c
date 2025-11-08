//To detect the acceleration of the vehicle which will help us detect if the person is either walking, or is on a scooter or bike.


#include <Wire.h>

#define SDA_PIN 21
#define SCL_PIN 22
uint8_t addr = 0x68; // will switch to 0x69 automatically if needed

// ---- helpers ----
uint8_t i2cRead8(uint8_t reg) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom((int)addr, 1, true);
  return Wire.read();
}
void i2cWrite8(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission(true);
}

void setup() {
  Serial.begin(115200);
  delay(800);
  Serial.println("\n--- MPU6050 deep-diagnose ---");

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000); // slow & stable

  // 1) scan
  bool seen68=false, seen69=false;
  for (uint8_t a=1; a<127; a++) {
    Wire.beginTransmission(a);
    if (Wire.endTransmission()==0) {
      Serial.print("Found I2C device at 0x"); Serial.println(a, HEX);
      if (a==0x68) seen68=true;
      if (a==0x69) seen69=true;
    }
  }
  if (!seen68 && !seen69) {
    Serial.println("No I2C devices found. (But you had 0x68 earlier.)");
    while(1){delay(1000);}
  }
  if (seen69) addr = 0x69; // AD0 high case
  Serial.print("Using address 0x"); Serial.println(addr, HEX);

  // 2) WHO_AM_I should be 0x68 regardless of AD0
  uint8_t who = 0xFF;
  who = i2cRead8(0x75);
  Serial.print("WHO_AM_I = 0x"); Serial.println(who, HEX);

  // 3) Force wake + basic config (works on finicky clones)
  i2cWrite8(0x6B, 0x00);   // PWR_MGMT_1 = 0x00 (wake)
  delay(50);
  i2cWrite8(0x6B, 0x01);   // set clock source
  i2cWrite8(0x1A, 0x03);   // DLPF_CFG
  i2cWrite8(0x19, 0x07);   // SMPLRT_DIV
  i2cWrite8(0x1B, 0x00);   // GYRO_CONFIG  ±250 dps
  i2cWrite8(0x1C, 0x00);   // ACCEL_CONFIG ±2g
  delay(50);

  Serial.println("Init sequence sent. Now reading accel...");
}

void loop() {
  // Read 6 accel bytes starting at 0x3B
  Wire.beginTransmission(addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom((int)addr, 6, true);
  if (Wire.available()==6) {
    int16_t ax = (Wire.read()<<8) | Wire.read();
    int16_t ay = (Wire.read()<<8) | Wire.read();
    int16_t az = (Wire.read()<<8) | Wire.read();
    // Convert to g (for ±2g scale, 16384 LSB/g)
    float AX = ax/16384.0, AY = ay/16384.0, AZ = az/16384.0;

    Serial.print("AX: "); Serial.print(AX,2);
    Serial.print("  AY: "); Serial.print(AY,2);
    Serial.print("  AZ: "); Serial.println(AZ,2);
  } else {
    Serial.println("Read failed (no bytes).");
  }
  delay(300);
}