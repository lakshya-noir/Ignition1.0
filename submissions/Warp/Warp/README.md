# Smart Road Hazard Detection System
Real-time pothole and road anomaly detection using ESP32 + IMU + GPS + Mobile App.

---

## ✅ Overview
This system detects sudden road shocks using an **ESP32 + MPU6050** sensor and maps them using a **mobile app with GPS tracking**.  
A **Node.js backend** merges IMU bumps with GPS and displays them as:

- 🟡 **Alerts** (potential potholes)  
- 🔴 **Potholes** (confirmed after repeated detections)

The app shows real-time location, live markers, and total counts.

---

## ✅ System Components

### **ESP32 + IMU**
- Reads IMU vertical acceleration  
- Detects bumps  
- Sends bump data to server using Wi-Fi (`POST /bump`)

### **Mobile App (React Native + Expo)**
- Tracks GPS every second  
- Sends coordinates (`POST /location`)  
- Map screen: shows anomalies & potholes  
- Home screen: shows total stats

### **Backend (Node.js)**
- Receives bumps + GPS  
- Merges by timestamp  
- Stores anomalies  
- Clusters multiple anomalies → confirms potholes  
- Provides:
  - `GET /bumps`
  - `GET /potholes`

---

## ✅ How to Run

### **Backend**
cd pothole-server
npm install
node server.js

markdown
Copy code

### **Mobile App**
cd pothole-tracker
npm install
npx expo start

yaml
Copy code
Scan QR using Expo Go.

### **ESP32**
- Open `esp32-firmware/esp32_bump_code.ino` in Arduino IDE  
- Update Wi-Fi SSID/PASS and server IP  
- Upload to ESP32

✅ All devices must be on the **same Wi-Fi/hotspot**.

---

## ✅ Folder Structure
esp32-firmware/
pothole-server/
pothole-tracker/

yaml
Copy code

---

## ✅ Demo Flow
1. ESP detects bump  
2. App sends GPS  
3. Server merges data  
4. Yellow pin appears  
5. After repeated detections → red pothole  
6. Stats update live

---

## ✅ Tech Stack
ESP32, MPU6050, Arduino, Node.js, Express, React Native, Expo, Google Maps