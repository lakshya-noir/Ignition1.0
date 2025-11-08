import { useState, useEffect } from "react";
import { View, Text, StyleSheet } from "react-native";
import * as Location from "expo-location";

const SERVER = "http://192.168.91.145:5000";

export default function Index() {
  const [location, setLocation] = useState<any>(null);
  const [anomalies, setAnomalies] = useState(0);
  const [potholes, setPotholes] = useState(0);
  const [errorMsg, setErrorMsg] = useState<string | null>(null);

  useEffect(() => {
    (async () => {
      let { status } = await Location.requestForegroundPermissionsAsync();
      if (status !== "granted") {
        setErrorMsg("Permission to access location was denied");
        return;
      }

      await Location.watchPositionAsync(
        {
          accuracy: Location.Accuracy.High,
          timeInterval: 1000,
          distanceInterval: 1,
        },
        (pos) => {
          setLocation(pos.coords);

          fetch(`${SERVER}/location`, {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({
              lat: pos.coords.latitude,
              lon: pos.coords.longitude,
              acc: pos.coords.accuracy,
              time: Date.now(),
            }),
          });
        }
      );
    })();
  }, []);

  useEffect(() => {
    const interval = setInterval(async () => {
      try {
        const r1 = await fetch(`${SERVER}/bumps`);
        const b = await r1.json();
        setAnomalies(b.length);

        const r2 = await fetch(`${SERVER}/potholes`);
        const p = await r2.json();
        setPotholes(p.length);
      } catch {}
    }, 2000);

    return () => clearInterval(interval);
  }, []);

  return (
    <View style={styles.container}>
      <Text style={styles.header}>Ride Telemetry</Text>

      {errorMsg && <Text style={styles.error}>{errorMsg}</Text>}

      {location ? (
        <>
          <Text style={styles.text}>Latitude: {location.latitude}</Text>
          <Text style={styles.text}>Longitude: {location.longitude}</Text>
          <Text style={styles.text}>Accuracy: {location.accuracy}m</Text>
        </>
      ) : (
        <Text style={styles.text}>Waiting for GPS...</Text>
      )}

      <View style={styles.statsBox}>
        <View style={styles.statItem}>
          <Text style={styles.statNumber}>{anomalies}</Text>
          <Text style={styles.statLabel}>Alerts</Text>
        </View>

        <View style={styles.statItem}>
          <Text style={styles.statNumber}>{potholes}</Text>
          <Text style={styles.statLabel}>Potholes</Text>
        </View>
      </View>
    </View>
  );
}

const styles = StyleSheet.create({
  container: { flex: 1, padding: 20, justifyContent: "center" },
  header: { fontSize: 26, fontWeight: "bold", marginBottom: 20 },
  text: { fontSize: 18, marginBottom: 8 },
  error: { color: "red", marginBottom: 10 },
  statsBox: {
    marginTop: 30,
    flexDirection: "row",
    justifyContent: "space-around",
    paddingVertical: 20,
    borderRadius: 12,
    backgroundColor: "#f2f2f2",
    elevation: 4,
  },
  statItem: {
    alignItems: "center",
  },
  statNumber: {
    fontSize: 32,
    fontWeight: "bold",
  },
  statLabel: {
    fontSize: 16,
    opacity: 0.7,
  },
});
