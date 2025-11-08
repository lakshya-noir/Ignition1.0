#include <WiFi.h>
#include <WebServer.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <Wire.h>
#include <MPU6050.h>

// ====== WiFi Credentials ======
const char* ssid = "darlin";
const char* password = "12345678";

// ====== GPS Configuration ======
HardwareSerial gpsSerial(1);
TinyGPSPlus gps;
#define RXD2 16
#define TXD2 17

// ====== MPU6050 Configuration ======
MPU6050 mpu;
int16_t rawTilt, rawRoll, rawYaw;
float tiltDeg, rollDeg;

// ====== Web Server ======
WebServer server(80);

// ====== Circular Stack for GPS History ======
#define BUFFER_SIZE 50
struct DataEntry {
  double lat;
  double lon;
  String time;
  double speed;
  float tilt;
  float roll;
  float accel;
  String mode;
};
DataEntry history[BUFFER_SIZE];
int head = 0;
int count = 0;

// ====== Last GPS sample for acceleration ======
double lastSpeed = 0;
unsigned long lastTime = 0;

// ====== HTML Page ======
String webPage() {
  String html = R"rawliteral(
<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8" />
<meta name="viewport" content="width=device-width,initial-scale=1" />
<title>Atherlink — Live GPS Dashboard</title>
<link href="https://fonts.googleapis.com/css2?family=Inter:wght@300;400;600;700&family=Great+Vibes&display=swap" rel="stylesheet">
<link rel="stylesheet" href="https://unpkg.com/leaflet@1.9.4/dist/leaflet.css" />
<style>
:root{--bg-1:#1b1833;--bg-2:#2a1f45;--card-bg:rgba(255,255,255,0.10);--glass:rgba(255,255,255,0.06);
--accent:rgba(255,255,255,0.95);--muted:rgba(255,255,255,0.65);--green:#2ee6a7;--shadow:0 10px 30px rgba(0,0,0,0.45);}
html,body{height:100%;margin:0;font-family:Inter,system-ui,-apple-system,Segoe UI,Roboto,"Helvetica Neue",Arial;}
body{background:linear-gradient(135deg,var(--bg-1) 0%,#3b2b5a 50%,#56427a 100%);
display:flex;align-items:center;justify-content:center;padding:28px;}
.dashboard{width:100%;max-width:1400px;display:grid;grid-template-columns:420px 1fr 380px;
gap:20px;align-items:start;background:linear-gradient(180deg,rgba(255,255,255,0.06),rgba(255,255,255,0.02));
border-radius:18px;padding:22px;box-shadow:var(--shadow);border:1px solid rgba(255,255,255,0.12);backdrop-filter:blur(8px) saturate(130%);}
.left,.center,.right{padding:18px;border-radius:12px;background:linear-gradient(180deg,rgba(255,255,255,0.04),rgba(255,255,255,0.02));}
.brand{display:flex;align-items:center;justify-content:space-between;}
.team{display:flex;align-items:center;gap:12px;}
.logo-circle{width:56px;height:56px;border-radius:12px;background:linear-gradient(135deg,#ffffff22,#ffffff12);
display:flex;align-items:center;justify-content:center;font-weight:700;color:var(--accent);font-size:20px;box-shadow:0 4px 16px rgba(0,0,0,0.25);}
.team h2{margin:0;color:var(--accent);font-family:"Great Vibes",cursive;font-size:32px;line-height:1;}
.team p{margin:0;color:var(--muted);font-size:13px;}
.stat-row{display:flex;gap:10px;}
.stat{flex:1;background:var(--card-bg);padding:14px;border-radius:12px;display:flex;flex-direction:column;gap:6px;
box-shadow:0 6px 18px rgba(0,0,0,0.15);border:1px solid rgba(255,255,255,0.05);}
.label{font-size:12px;color:var(--muted);}
.value{font-size:20px;color:var(--accent);font-weight:600;}
.small{font-size:12px;color:var(--muted);}
.speed-box{width:220px;height:120px;border-radius:12px;padding:12px;background:linear-gradient(180deg,rgba(255,255,255,0.02),rgba(255,255,255,0.01));display:flex;flex-direction:column;align-items:center;justify-content:center;}
.speed-value{font-size:28px;font-weight:700;color:var(--accent);}
.mode-tag{margin-top:8px;padding:6px 10px;border-radius:999px;font-weight:700;background:rgba(255,255,255,0.06);color:var(--muted);font-size:12px;}
#map{width:100%;height:280px;margin-top:20px;border-radius:12px;box-shadow:0 6px 18px rgba(0,0,0,0.15);border:1px solid rgba(255,255,255,0.08);}
#historyModal{display:none;position:fixed;top:0;left:0;width:100%;height:100%;background:rgba(0,0,0,0.7);justify-content:center;align-items:center;z-index:1000;}
#historyContent{background:var(--bg-2);padding:20px;border-radius:12px;width:90%;max-width:600px;height:70%;overflow-y:auto;color:#fff;}
#historyContent table{width:100%;border-collapse:collapse;}
#historyContent th,td{padding:6px;border-bottom:1px solid rgba(255,255,255,0.2);}
#historyContent th{color:#2ee6a7;}
#closeBtn{cursor:pointer;color:#ff6b6b;float:right;font-weight:bold;}
@media (max-width:980px){.dashboard{grid-template-columns:1fr;max-width:900px;}.right{order:3}}
</style>
</head>
<body>
<div class="dashboard">
<div class="left">
<div class="brand">
<div class="team">
<div class="logo-circle">A</div>
<div><h2>Atherlink</h2><p>Team Atherlink — Live Rider Telemetry</p></div>
</div>
<div><div class="small">Status</div><div id="status" style="font-weight:700;color:var(--green)">Connecting...</div></div>
</div>
<div class="stat-row">
<div class="stat"><div class="label">Latitude</div><div class="value" id="lat">—</div></div>
<div class="stat"><div class="label">Longitude</div><div class="value" id="lon">—</div></div>
</div>
<div class="stat-row">
<div class="stat"><div class="label">Speed (km/h)</div><div class="value" id="speed">—</div></div>
<div class="stat"><div class="label">Tilt (°)</div><div class="value" id="tilt">—</div></div>
</div>
<div class="stat-row">
<div class="stat"><div class="label">Roll (°)</div><div class="value" id="roll">—</div></div>
<div class="stat"><div class="label">Acceleration (m/s²)</div><div class="value" id="accel">—</div></div>
</div>
<div class="stat-row">
<div class="stat"><div class="label">Mode</div><div class="value" id="mode">—</div></div>
<div class="stat"><div class="label">Time (IST)</div><div class="value" id="timeUtc">—</div></div>
</div>
<button class="btn" onclick="showHistory()" style="margin-top:10px;">View History</button>
<div id="map"></div>
</div>
<div class="center" style="display:flex;flex-direction:column;align-items:center;justify-content:center;">
<div class="speed-box">
<div class="speed-value" id="speedBig">— km/h</div>
<div class="mode-tag" id="modeTag">—</div>
</div>
<div style="margin-top:10px;"><a id="mapsLink" href="#" target="_blank" style="color:var(--accent);text-decoration:underline;">Open in Maps</a></div>
</div>
<div class="right"><div class="ring" id="ringContainer"></div></div>
</div>

<div id="historyModal">
<div id="historyContent">
<span id="closeBtn" onclick="closeHistory()">✖</span>
<h3>GPS + Tilt/Roll History (last 50 points)</h3>
<table id="historyTable">
<thead><tr><th>Time</th><th>Lat</th><th>Lon</th><th>Speed</th><th>Tilt</th><th>Roll</th><th>Accel</th><th>Mode</th></tr></thead>
<tbody></tbody>
</table>
</div>
</div>

<script src="https://unpkg.com/leaflet@1.9.4/dist/leaflet.js"></script>
<script>
const API_BASE = location.origin;
const API_PATH = '/data';
let lastSample = null;
const map = L.map('map',{zoomControl:false,attributionControl:false}).setView([12.9716,77.5946],13);
L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png',{maxZoom:19}).addTo(map);
const marker = L.circleMarker([12.9716,77.5946],{radius:8,color:'#2ee6a7',fillColor:'#2ee6a7',fillOpacity:0.9}).addTo(map);

function detectMode(speed,accel){
  if(speed<2.5 && accel<0.2) return 'Stationary';
  if(speed<7) return 'Walking';
  return 'Biking';
}

async function poll(){
  try{
    const res = await fetch(API_BASE+API_PATH);
    const data = await res.json();
    const now = new Date();
    const lat = data.lat??0;
    const lon = data.lon??0;
    const spd = data.speed??0;
    const tilt = data.tilt??0;
    const roll = data.roll??0;
    const accel = data.accel??0;
    const mode = data.mode??'—';
    const iso = data.time??now.toISOString();

    lastSample={time:iso,lat,lon,speed:spd,tilt,roll,accel,mode};
    updateUI(lastSample);
    updateMap(lastSample);
  }catch(e){
    document.getElementById('status').textContent='Disconnected';
    document.getElementById('status').style.color='var(--danger)';
  }finally{
    setTimeout(poll,1000);
  }
}

function updateUI(s){
  document.getElementById('status').textContent='Connected';
  document.getElementById('status').style.color='var(--green)';
  document.getElementById('lat').textContent=s.lat.toFixed(6);
  document.getElementById('lon').textContent=s.lon.toFixed(6);
  document.getElementById('speed').textContent=s.speed.toFixed(2);
  document.getElementById('speedBig').textContent=s.speed.toFixed(1)+' km/h';
  document.getElementById('tilt').textContent=s.tilt.toFixed(2);
  document.getElementById('roll').textContent=s.roll.toFixed(2);
  document.getElementById('accel').textContent=s.accel.toFixed(2);
  document.getElementById('mode').textContent=s.mode;
  document.getElementById('modeTag').textContent=s.mode;
  document.getElementById('timeUtc').textContent=s.time;
  document.getElementById('mapsLink').href=`https://www.google.com/maps?q=${s.lat},${s.lon}`;
}

function updateMap(s){
  marker.setLatLng([s.lat,s.lon]);
  map.setView([s.lat,s.lon],map.getZoom(),{animate:true});
}

// History modal
async function showHistory(){
  const res = await fetch(API_BASE+'/history');
  const data = await res.json();
  const tbody = document.querySelector('#historyTable tbody');
  tbody.innerHTML = '';
  data.forEach(d=>{
    const tr = document.createElement('tr');
    tr.innerHTML = `<td>${d.time}</td><td>${d.lat.toFixed(6)}</td><td>${d.lon.toFixed(6)}</td><td>${d.speed.toFixed(2)}</td><td>${d.tilt.toFixed(2)}</td><td>${d.roll.toFixed(2)}</td><td>${d.accel.toFixed(2)}</td><td>${d.mode}</td>`;
    tbody.appendChild(tr);
  });
  document.getElementById('historyModal').style.display='flex';
}
function closeHistory(){document.getElementById('historyModal').style.display='none';}

poll();
</script>
</body>
</html>
)rawliteral";
  return html;
}

// ====== Convert UTC to IST ======
String utcToIST(int hour,int minute,int second){
  int totalSec = hour*3600 + minute*60 + second + (5*3600 + 30*60);
  totalSec %= 86400;
  int newH = totalSec/3600;
  int newM = (totalSec%3600)/60;
  int newS = totalSec%60;
  char buf[20];
  sprintf(buf,"%02d:%02d:%02d",newH,newM,newS);
  return String(buf);
}

// ====== Setup ======
void setup() {
  Serial.begin(115200);
  Wire.begin();
  gpsSerial.begin(9600, SERIAL_8N1, RXD2, TXD2);

  // Initialize MPU6050
  mpu.initialize();
  if(!mpu.testConnection()){ Serial.println("MPU6050 connection failed"); }

  WiFi.begin(ssid,password);
  Serial.print("Connecting to WiFi");
  while(WiFi.status()!=WL_CONNECTED){ delay(500); Serial.print("."); }
  Serial.println("\nConnected! IP: "); Serial.println(WiFi.localIP());

  server.on("/", [](){ server.send(200,"text/html",webPage()); });

  server.on("/data", [](){
    String json="{";

    // GPS data
    if(gps.location.isValid()){ 
      json += "\"lat\":"+String(gps.location.lat(),6)+",";
      json += "\"lon\":"+String(gps.location.lng(),6)+","; 
    }else json += "\"lat\":0,\"lon\":0,";

    double speedVal = gps.speed.isValid()?gps.speed.kmph():0;
    json += "\"speed\":"+String(speedVal,2)+",";

    // Calculate acceleration (m/s²)
    unsigned long nowMillis = millis();
    float accelVal = 0;
    if(lastTime>0){
      float dt = (nowMillis - lastTime)/1000.0;
      if(dt>0) accelVal = (speedVal - lastSpeed)/3.6/dt; // km/h -> m/s
    }
    lastSpeed = speedVal;
    lastTime = nowMillis;
    json += "\"accel\":"+String(accelVal,2)+",";

    // Read MPU6050 rotation
    mpu.getRotation(&rawTilt,&rawRoll,&rawYaw);
    tiltDeg = rawTilt/131.0;
    rollDeg = rawRoll/131.0;
    json += "\"tilt\":"+String(tiltDeg,2)+",";
    json += "\"roll\":"+String(rollDeg,2)+",";

    // Time
    if(gps.time.isValid()){
      String ist = utcToIST(gps.time.hour(),gps.time.minute(),gps.time.second());
      json += "\"time\":\""+ist+"\"";
    }else json += "\"time\":\"--:--:--\"";

    // Determine Mode
    String modeStr = "—";
    if(speedVal<2.5 && accelVal<0.2) modeStr="Stationary";
    else if(speedVal<7) modeStr="Walking";
    else if(speedVal<15) modeStr="Running";
    else modeStr="Biking";
    json += ",\"mode\":\""+modeStr+"\"";
    json += "}";

    // Add to circular history
    if(gps.location.isValid() && gps.time.isValid()){
      history[head].lat = gps.location.lat();
      history[head].lon = gps.location.lng();
      history[head].speed = speedVal;
      history[head].tilt = tiltDeg;
      history[head].roll = rollDeg;
      history[head].accel = accelVal;
      history[head].mode = modeStr;
      history[head].time = utcToIST(gps.time.hour(),gps.time.minute(),gps.time.second());
      head = (head+1)%BUFFER_SIZE;
      if(count<BUFFER_SIZE) count++;
    }

    server.send(200,"application/json",json);
  });

  server.on("/history", [](){
    String json="[";
    for(int i=0;i<count;i++){
      int idx=(head-count+i+BUFFER_SIZE)%BUFFER_SIZE;
      json += "{";
      json += "\"lat\":"+String(history[idx].lat,6)+",";
      json += "\"lon\":"+String(history[idx].lon,6)+",";
      json += "\"speed\":"+String(history[idx].speed,2)+",";
      json += "\"tilt\":"+String(history[idx].tilt,2)+",";
      json += "\"roll\":"+String(history[idx].roll,2)+",";
      json += "\"accel\":"+String(history[idx].accel,2)+",";
      json += "\"mode\":\""+history[idx].mode+"\",";
      json += "\"time\":\""+history[idx].time+"\"";
      json += "}";
      if(i<count-1) json+=",";
    }
    json+="]";
    server.send(200,"application/json",json);
  });

  server.begin();
  Serial.println("Web server started.");
}

// ====== Loop ======
void loop() {
  while(gpsSerial.available()>0) gps.encode(gpsSerial.read());
  server.handleClient();
}
