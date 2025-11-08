from flask import Flask, request, jsonify, render_template, send_from_directory
from collections import deque
import math, time, numpy as np
import os, json
from datetime import datetime

app = Flask(__name__)
data_buffer = deque(maxlen=200)

LOG_DIR = "logs"
os.makedirs(LOG_DIR, exist_ok=True)

def get_tilt(accel):
    ax, ay, az = accel.get("x", 0), accel.get("y", 0), accel.get("z", 0)
    if ax == ay == az == 0:
        return 0.0, 0.0
    pitch = math.degrees(math.atan2(ax, math.sqrt(ay**2 + az**2)))
    roll = math.degrees(math.atan2(ay, math.sqrt(ax**2 + az**2)))
    return pitch, roll

def classify(accel, gyro, acc_mag_var):
    pitch, roll = get_tilt(accel)
    gyro_activity = abs(gyro.get("x", 0)) + abs(gyro.get("y", 0)) + abs(gyro.get("z", 0))
    acc_mag = math.sqrt(accel["x"]**2 + accel["y"]**2 + accel["z"]**2)

    if gyro_activity < 0.03 and acc_mag_var < 0.05:
        status = "Standing Still"
    elif acc_mag_var > 0.15:
        status = "Walking"
    elif roll > 80:
        status = "Riding Scooter"
    elif 35 < roll < 55:
        status = "Riding Bike"
    elif 35< acc_mag_var <50:
        status = "Sudden Jerk Detected!"
    else:
        status = "Unknown"

    return status, pitch, roll, acc_mag

@app.route('/upload', methods=['POST'])
def upload():
    payload = request.get_json()
    if not payload:
        return jsonify({"error": "Invalid JSON"}), 400

    readings = payload.get("data", [payload])
    acc_mags = [math.sqrt(r.get("accelerometer", {}).get("x", 0)**2 +
                          r.get("accelerometer", {}).get("y", 0)**2 +
                          r.get("accelerometer", {}).get("z", 0)**2)
                 for r in readings if "accelerometer" in r]
    acc_mag_var = np.var(acc_mags) if acc_mags else 0.0

    all_records = []

    for entry in readings:
        accel = entry.get("accelerometer", {})
        gyro = entry.get("gyroscope", {})
        compass = entry.get("compass", 0.0)
        gps = entry.get("gps", {})
        timestamp = entry.get("timestamp", time.time())

        status, pitch, roll, acc_mag = classify(accel, gyro, acc_mag_var)

        record = {
            "timestamp": timestamp,
            "pitch": round(pitch, 2),
            "roll": round(roll, 2),
            "speed": gps.get("speed", 0.0),
            "acc_mag": round(acc_mag, 2),
            "compass": round(compass, 2),
            "status": status,
        }

        lat = gps.get("latitude") or gps.get("lat")
        lon = gps.get("longitude") or gps.get("lon")
        if lat is not None and lon is not None:
            record["lat"] = lat
            record["lon"] = lon


        data_buffer.append(record)
        all_records.append(record)

    filename = datetime.now().strftime("%Y-%m-%d") + ".log"
    with open(os.path.join(LOG_DIR, filename), "a") as f:
        for rec in all_records:
            f.write(json.dumps(rec) + "\n")

    print(f"[{len(all_records)} samples] Last: {data_buffer[-1]['status']}")

    return jsonify({"message": f"{len(readings)} samples processed", "status": data_buffer[-1]['status']}), 200

@app.route('/data')
def get_data():
    return jsonify(list(data_buffer))

@app.route('/gpsdata')
def get_gpsdata():
    points = [{"lat": r["lat"], "lon": r["lon"], "timestamp": r["timestamp"]}
              for r in data_buffer if "lat" in r and "lon" in r]
    return jsonify(points)


@app.route('/map')
def map_page():
    if os.path.exists("map.html"):
        return send_from_directory(".", "map.html")
    elif os.path.exists(os.path.join("templates", "map.html")):
        return render_template("map.html")
    else:
        return "map.html not found", 404

@app.route('/dashboard')
def dashboard():
    if os.path.exists("dashboard-2.html"):
        return send_from_directory(".", "dashboard-2.html")
    elif os.path.exists(os.path.join("templates", "dashboard.html")):
        return render_template("dashboard.html")
    else:
        return "dashboard-2.html not found", 404

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
