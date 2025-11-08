# Quick Setup Guide

## Phase 1 is Ready! 🚀

### Installation Steps:

1. **Navigate to the project:**
   ```bash
   cd C:\Users\HOME\rider-telemetry-mobile
   ```

2. **Install dependencies:**
   ```bash
   npm install
   ```

3. **Create placeholder assets** (Expo requires these):
   - Create a simple 1024x1024 PNG for `assets/icon.png`
   - Create a simple 1024x1024 PNG for `assets/splash.png`
   - Create a simple 1024x1024 PNG for `assets/adaptive-icon.png`
   - Create a simple 48x48 PNG for `assets/favicon.png`
   
   Or use Expo's asset generator:
   ```bash
   npx expo install expo-asset
   ```

4. **Start the app:**
   ```bash
   npm start
   ```

5. **Run on device:**
   - Install "Expo Go" app on your phone
   - Scan the QR code shown in terminal
   - Grant GPS permissions when prompted

### What's Working (Phase 1):

✅ **GPS Tracking** - Real-time location with distance calculation  
✅ **Sensor Integration** - WiFi fetch from ESP32 (falls back to mock data)  
✅ **Dashboard** - Live metrics, charts, vehicle mode detection  
✅ **Map Screen** - Real-time position tracking with route polyline  
✅ **Start/Stop Ride** - Control ride sessions  
✅ **Vehicle Detection** - Auto-detects Walking/Scooter/Motorcycle  

### Testing Without Hardware:

The app automatically uses **mock data** when:
- ESP32 sensor is not connected
- WiFi connection fails
- GPS permission denied

### Next Steps (Phase 2):

- SQLite database for data persistence
- Data Logs screen with pagination
- Ride Summary screen
- CSV/JSON export functionality
- Auto-export at 1000 records

### Troubleshooting:

- **GPS not working?** Check app permissions in phone settings
- **Sensor not connecting?** Verify ESP32 IP is `192.168.4.1` (or update in `src/hooks/useTelemetry.js`)
- **App crashes?** Check Metro bundler logs for errors

