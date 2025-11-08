import React, { useEffect, useState, useRef } from "react";

export default function RiderClassifier() {
  const [status, setStatus] = useState("Waiting for data...");
  const [lastData, setLastData] = useState(null);

  const accelBuffer = useRef([]);
  const gyroBuffer = useRef([]);

  useEffect(() => {
    // 🛰️ Replace with your actual WebSocket endpoint
    const socket = new WebSocket("ws://192.168.x.x:8080");

    socket.onopen = () => console.log("✅ Connected to sensor stream");

    socket.onmessage = (event) => {
      try {
        const data = JSON.parse(event.data);
        setLastData(data);
        classify(data);
      } catch (e) {
        console.error("Invalid packet", e);
      }
    };

    socket.onclose = () => console.log("❌ Disconnected");
    return () => socket.close();
  }, []);

  function classify({ Ax = 0, Ay = 0, Az = 0, Gx = 0, Gy = 0, Gz = 0 }) {
    const aMag = Math.sqrt(Ax ** 2 + Ay ** 2 + Az ** 2);
    const gMag = Math.sqrt(Gx ** 2 + Gy ** 2 + Gz ** 2);

    // Keep short history for smoothing
    accelBuffer.current.push(aMag);
    gyroBuffer.current.push(gMag);

    if (accelBuffer.current.length > 30) accelBuffer.current.shift();
    if (gyroBuffer.current.length > 30) gyroBuffer.current.shift();

    // Compute averages
    const avgA = accelBuffer.current.reduce((a, b) => a + b, 0) / accelBuffer.current.length;
    const avgG = gyroBuffer.current.reduce((a, b) => a + b, 0) / gyroBuffer.current.length;

    const varA = variance(accelBuffer.current);
    const varG = variance(gyroBuffer.current);

    /*
      Heuristic logic purely from IMU:
      - Standing: very low avgA, very low avgG, very low var
      - Walking: moderate avgA, higher varA, rhythmic gyro
      - Scooter: slightly higher avgA but low varG
      - Bike: high avgA + noticeable varG (more steering)
    */

    let mode = "🤔 Unknown";

    if (avgA < 0.3 && avgG < 0.2 && varA < 0.05) {
      mode = "🧍 Standing Still";
    } 
    else if (avgA >= 0.3 && avgA < 1.2 && varA > 0.1 && avgG < 1.0) {
      mode = "🚶 Walking";
    } 
    else if (avgA >= 1.0 && avgA < 2.0 && varA < 0.3 && avgG < 2.0) {
      mode = "🛴 Scooter";
    } 
    else if (avgA >= 1.5 && varA > 0.4 && avgG >= 2.0) {
      mode = "🏍 Motorbike";
    }

    setStatus(`${mode}  |  aMag=${avgA.toFixed(2)}  gMag=${avgG.toFixed(2)}`);
  }

  function variance(arr) {
    const mean = arr.reduce((a, b) => a + b, 0) / arr.length;
    return arr.reduce((a, b) => a + (b - mean) ** 2, 0) / arr.length;
  }

  return (
    <div style={{
      fontFamily: "system-ui, sans-serif",
      background: "#f7f8fa",
      color: "#111",
      padding: 20,
      borderRadius: 10,
      maxWidth: 600,
      margin: "40px auto",
      boxShadow: "0 0 10px rgba(0,0,0,0.1)"
    }}>
      <h2>Rider Classifier (IMU-based)</h2>
      <h3 style={{ color: "#333", margin: "10px 0" }}>{status}</h3>

      {lastData && (
        <pre style={{
          background: "#111",
          color: "#0f0",
          padding: 10,
          borderRadius: 8,
          overflow: "auto",
          maxHeight: 250
        }}>
{JSON.stringify(lastData, null, 2)}
        </pre>
      )}
    </div>
  );
}