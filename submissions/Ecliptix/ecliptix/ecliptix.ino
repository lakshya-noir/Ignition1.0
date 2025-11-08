#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <Wire.h>
#include <MPU6050.h>  // Electronic Cats library
#include <HardwareSerial.h>

// ---- Wi-Fi credentials ----
const char* ssid = "Ecliptix";
const char* password = "12345678";

// ---- Objects ----
WebServer server(80);
WebSocketsServer webSocket(81);
MPU6050 mpu;

// ---- Variables ----
unsigned long lastSend = 0;
bool mpuConnected = false;
uint8_t detectedAddress = 0; // 0 if unknown

// ---- HTML Page (unchanged) ----
String htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<title>Rider Telemetry Dashboard</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="stylesheet" href="https://unpkg.com/leaflet@1.9.4/dist/leaflet.css" />
<script src="https://unpkg.com/leaflet@1.9.4/dist/leaflet.js"></script>
<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
<style>
body { font-family: 'Segoe UI', sans-serif; background: #111; color: #eee; text-align: center; margin: 0; }
h2 { background: #222; margin: 0; padding: 0.6em; }
canvas { background: #222; margin: 10px auto; display: block; border-radius: 10px; }
#state { font-size: 1.4em; color: #0f0; margin: 10px 0; }
#map { height: 300px; width: 90%; margin: 0 auto; border-radius: 10px; }
footer { font-size: 0.8em; margin-top: 10px; color: #888; }
</style>
</head>
<body>
<h2>Rider Telemetry Dashboard</h2>
<div id="state">Connecting...</div>
<div id="map"></div>
<canvas id="accelChart" width="380" height="150"></canvas>
<canvas id="speedChart" width="380" height="150"></canvas>
<footer>ESP32 + MPU6050 + Phone GPS</footer>

<script>
let ws = new WebSocket("ws://" + location.hostname + ":81/");
let accelMag = 0, currentSpeed = 0, mode = "Unknown";
let map, marker, pathLine;
let gpsCoords = [];

function initMap(lat=12.9716, lon=77.5946) {
  map = L.map('map').setView([lat, lon], 16);
  L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', { maxZoom: 19 }).addTo(map);
  marker = L.marker([lat, lon]).addTo(map);
  pathLine = L.polyline([], {color: 'lime'}).addTo(map);
}

let accelChart = new Chart(document.getElementById('accelChart'), {
 type:'line',
 data:{labels:[],datasets:[{label:'Accel (m/s²)',data:[],borderColor:'#0f0',fill:false}]},
 options:{animation:false,scales:{x:{display:false},y:{min:0,max:20}}}
});
let speedChart = new Chart(document.getElementById('speedChart'), {
 type:'line',
 data:{labels:[],datasets:[{label:'Speed (km/h)',data:[],borderColor:'#08f',fill:false}]},
 options:{animation:false,scales:{x:{display:false},y:{min:0,max:80}}}
});

ws.onmessage = (event)=>{
 let d = JSON.parse(event.data);
 accelMag = d.accel;
};

function classify(){
 if (currentSpeed < 2.0) mode = "Walking";
 else if (currentSpeed < 25.0) mode = "Scooter";
 else mode = "Motorcycle";
 document.getElementById('state').innerText =
  `Mode: ${mode} | Speed: ${currentSpeed.toFixed(1)} km/h | Accel: ${accelMag.toFixed(2)} m/s²`;
}

function plot(){
 accelChart.data.labels.push('');
 accelChart.data.datasets[0].data.push(accelMag);
 speedChart.data.labels.push('');
 speedChart.data.datasets[0].data.push(currentSpeed);
 accelChart.update(); speedChart.update();
 if(accelChart.data.labels.length>50){accelChart.data.labels.shift();accelChart.data.datasets[0].data.shift();}
 if(speedChart.data.labels.length>50){speedChart.data.labels.shift();speedChart.data.datasets[0].data.shift();}
}

function updateMap(lat, lon){
 if (!map) initMap(lat, lon);
 marker.setLatLng([lat, lon]);
 gpsCoords.push([lat, lon]);
 pathLine.setLatLngs(gpsCoords);
 map.panTo([lat, lon]);
}

function startGPS(){
 if (navigator.geolocation) {
   navigator.geolocation.watchPosition((pos)=>{
     let lat = pos.coords.latitude;
     let lon = pos.coords.longitude;
     currentSpeed = (pos.coords.speed || 0) * 3.6; // convert m/s → km/h
     classify();
     plot();
     updateMap(lat, lon);
   }, (err)=>{ console.log(err); }, {enableHighAccuracy:true});
 } else alert("GPS not supported");
}

window.onload = startGPS;
</script>
</body>
</html>
)rawliteral";

// ---- WebSocket Event Handler ----
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED:
      Serial.println("[WebSocket] Client connected");
      break;
    case WStype_DISCONNECTED:
      Serial.println("[WebSocket] Client disconnected");
      break;
    case WStype_TEXT:
      Serial.printf("[WebSocket] Message: %s\n", payload);
      break;
  }
}

// ---- Helper: read WHO_AM_I (register 0x75) directly using Wire ----
int readWhoAmIAt(uint8_t address) {
  Wire.beginTransmission(address);
  Wire.write(0x75); // WHO_AM_I register
  if (Wire.endTransmission(false) != 0) { // hold bus
    return -1;
  }
  Wire.requestFrom((int)address, 1);
  if (Wire.available()) {
    int id = Wire.read();
    return id;
  }
  return -1;
}

// ---- Setup ----
void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("Booting...");

  // Wi-Fi AP start
  WiFi.softAP(ssid, password);
  Serial.println("Access Point Started");
  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.softAPIP());

  // Begin I2C on expected pins
  Wire.begin(21, 22);
  delay(50);

  // Try to detect WHO_AM_I at 0x68 and 0x69
  int id68 = readWhoAmIAt(0x68);
  int id69 = readWhoAmIAt(0x69);
  if (id68 > 0) {
    Serial.print("WHO_AM_I @0x68 = 0x"); Serial.println(id68, HEX);
    detectedAddress = 0x68;
  } else if (id69 > 0) {
    Serial.print("WHO_AM_I @0x69 = 0x"); Serial.println(id69, HEX);
    detectedAddress = 0x69;
  } else {
    Serial.println("WHO_AM_I not found at 0x68/0x69. I2C device may be absent or AD0 floating.");
    detectedAddress = 0;
  }

  Serial.println("Initializing MPU6050 (library) ...");
  // Try library init and testConnection with retries
  const int MAX_TRIES = 5;
  int tries = 0;
  bool libOK = false;
  while (tries < MAX_TRIES) {
    mpu.initialize(); // library init (uses default / internal TwoWire)
    delay(50);
    if (mpu.testConnection()) {
      libOK = true;
      break;
    }
    tries++;
    Serial.print("mpu.testConnection() failed, retry "); Serial.println(tries);
    delay(150);
  }

  if (libOK) {
    Serial.println("MPU6050: library testConnection() OK");
    mpuConnected = true;
  } else {
    // If library failed but WHO_AM_I succeeded, continue with warning
    if (detectedAddress != 0) {
      Serial.println("WARNING: library testConnection() failed BUT WHO_AM_I detected. Proceeding anyway.");
      mpuConnected = true; // optimistic; library may still work for getMotion6
    } else {
      Serial.println("ERROR: MPU6050 not detected. Check wiring, power, and SDA/SCL pins.");
      mpuConnected = false;
    }
  }

  // start webserver & websocket
  server.on("/", []() { server.send(200, "text/html", htmlPage); });
  server.begin();
  Serial.println("Web Server Started");

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println("WebSocket Started");
}

// ---- Loop ----
void loop() {
  server.handleClient();
  webSocket.loop();

  if (!mpuConnected) return; // skip reads until MPU is present

  int16_t ax, ay, az, gx, gy, gz;
  // read raw sensor values (library call)
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  if (millis() - lastSend > 300) {
    lastSend = millis();

    // Convert to m/s² (assuming default FS = +/-2g => 16384 LSB/g)
    float ax_ms2 = (float)ax * 9.81 / 16384.0;
    float ay_ms2 = (float)ay * 9.81 / 16384.0;
    float az_ms2 = (float)az * 9.81 / 16384.0;
    float accelMag = sqrt(ax_ms2 * ax_ms2 + ay_ms2 * ay_ms2 + az_ms2 * az_ms2);

    Serial.print("Accel (m/s^2): X=");
    Serial.print(ax_ms2, 2);
    Serial.print(" Y=");
    Serial.print(ay_ms2, 2);
    Serial.print(" Z=");
    Serial.print(az_ms2, 2);
    Serial.print(" | Magnitude=");
    Serial.println(accelMag, 2);

    String json = "{\"accel\":" + String(accelMag, 2) + "}";
    webSocket.broadcastTXT(json);
  }
}
