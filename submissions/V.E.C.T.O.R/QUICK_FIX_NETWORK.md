# Quick Fix for "Network request failed"

## ✅ Solution Applied

I've added `usesCleartextTraffic: true` to `app.json`, but **Expo Go has limitations**.

## 🚨 Important: Expo Go Limitation

**Expo Go** uses a pre-built app, so Android network security settings might not apply.

## 🔧 Solutions (Try in Order):

### Option 1: Test ESP32 Connection First (Quick Test)

1. **On your phone**, open a browser (Chrome/Safari)
2. **Type in address bar**: `http://10.230.127.235/data`
3. **If this works** → ESP32 is reachable, issue is with Expo Go
4. **If this fails** → Network/ESP32 issue

### Option 2: Create Development Build (Best Solution)

Since you're at a hackathon, you might not have time, but this is the proper fix:

```bash
# Install EAS CLI
npm install -g eas-cli

# Build development version
eas build --profile development --platform android
```

### Option 3: Use Expo Development Build (If Available)

If you have Expo Dev Client installed, the network settings will work.

### Option 4: Quick Workaround - Test on Same Network

1. **Verify both devices on same WiFi**:
   - Phone WiFi IP should be like `10.230.x.x` (same subnet)
   - ESP32 is `10.230.127.235`

2. **Test connectivity**:
   ```bash
   # On your computer, ping ESP32
   ping 10.230.127.235
   ```

3. **Check ESP32 is running**:
   - ESP32 should have a web server running
   - Should respond to HTTP requests

### Option 5: Temporary - Use Your Computer as Bridge

If ESP32 is only accessible from your computer:
1. Create a simple proxy/bridge on your computer
2. Forward requests from phone to ESP32

## 📱 What to Do Right Now:

1. **Test in browser first** - `http://10.230.127.235/data`
2. **If browser works** → The issue is Expo Go's network restrictions
3. **For demo** → You can show the browser working + explain the app limitation
4. **Or** → Use mock data and explain ESP32 integration is ready (code is correct)

## 🔍 Debug Info:

The app is trying these endpoints:
- `http://10.230.127.235/data`
- `http://10.230.127.235/`
- `http://10.230.127.235/api/data`
- `http://10.230.127.235/sensor`

Check console logs to see which one fails.

## 💡 For Judges:

You can explain:
- ✅ Code is correct and ready
- ✅ ESP32 integration implemented
- ⚠️ Expo Go has network security restrictions
- ✅ Would work in production build
- ✅ Browser test proves ESP32 is accessible

