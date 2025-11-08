import React, { useEffect, useRef, useState } from "react";
import { View, Text, StyleSheet, Platform } from "react-native";
import MapView, { Marker, PROVIDER_GOOGLE, Region } from "react-native-maps";
import * as Location from "expo-location";

const SERVER = "http://192.168.91.145:5000";

export default function MapScreen() {
  const mapRef = useRef<MapView | null>(null);

  const [me, setMe] = useState<{ latitude: number; longitude: number } | null>(
    null
  );
  const [region, setRegion] = useState<Region>({
    latitude: 12.9716,
    longitude: 77.5946,
    latitudeDelta: 0.01,
    longitudeDelta: 0.01,
  });

  const [bumps, setBumps] = useState([]);
  const [potholes, setPotholes] = useState([]);
  const [hasPerm, setHasPerm] = useState(false);
  const [layer, setLayer] = useState<"all" | "bumps" | "potholes">("all");

  useEffect(() => {
    (async () => {
      const { status } = await Location.requestForegroundPermissionsAsync();
      if (status !== "granted") return;

      setHasPerm(true);

      const cur = await Location.getCurrentPositionAsync({
        accuracy: Location.Accuracy.High,
      });

      setMe({
        latitude: cur.coords.latitude,
        longitude: cur.coords.longitude,
      });

      setRegion((r) => ({
        ...r,
        latitude: cur.coords.latitude,
        longitude: cur.coords.longitude,
      }));

      if (mapRef.current) {
        mapRef.current.animateToRegion(
          {
            latitude: cur.coords.latitude,
            longitude: cur.coords.longitude,
            latitudeDelta: 0.01,
            longitudeDelta: 0.01,
          },
          500
        );
      }

      fetch(`${SERVER}/location`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
          lat: cur.coords.latitude,
          lon: cur.coords.longitude,
          acc: cur.coords.accuracy,
          time: Date.now(),
        }),
      });

      await Location.watchPositionAsync(
        {
          accuracy: Location.Accuracy.High,
          timeInterval: 1000,
          distanceInterval: 1,
        },
        (pos) => {
          setMe({
            latitude: pos.coords.latitude,
            longitude: pos.coords.longitude,
          });

          if (mapRef.current) {
            mapRef.current.animateToRegion(
              {
                latitude: pos.coords.latitude,
                longitude: pos.coords.longitude,
                latitudeDelta: 0.01,
                longitudeDelta: 0.01,
              },
              500
            );
          }

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
    let intervalId: number | undefined;

    const fetchData = async () => {
      try {
        const r1 = await fetch(`${SERVER}/bumps`);
        const bumpsData = await r1.json();
        setBumps(bumpsData);

        const r2 = await fetch(`${SERVER}/potholes`);
        const potholeData = await r2.json();
        setPotholes(potholeData);
      } catch {}
    };

    fetchData();
    intervalId = setInterval(fetchData, 2000);

    return () => {
      if (intervalId !== undefined) clearInterval(intervalId);
    };
  }, []);

  return (
    <View style={styles.container}>
      <View style={styles.layerBar}>
        <View style={styles.layerButtons}>
          <Text
            style={[styles.layerButton, layer === "all" && styles.activeButton]}
            onPress={() => setLayer("all")}
          >
            All
          </Text>

          <Text
            style={[styles.layerButton, layer === "bumps" && styles.activeButton]}
            onPress={() => setLayer("bumps")}
          >
            Alerts
          </Text>

          <Text
            style={[
              styles.layerButton,
              layer === "potholes" && styles.activeButton,
            ]}
            onPress={() => setLayer("potholes")}
          >
            Potholes
          </Text>
        </View>
      </View>

      <MapView
        ref={mapRef}
        provider={PROVIDER_GOOGLE}
        style={styles.map}
        initialRegion={region}  
        showsUserLocation={true}
        rotateEnabled={false}
      >

        {(layer === "all" || layer === "bumps") &&
          bumps.map((b: any, i: number) => (
            <Marker
              key={`b${i}`}
              coordinate={{ latitude: b.lat, longitude: b.lon }}
              pinColor="yellow"
            />
          ))}

        {(layer === "all" || layer === "potholes") &&
          potholes.map((p: any, i: number) => (
            <Marker
              key={`p${i}`}
              coordinate={{ latitude: p.lat, longitude: p.lon }}
              pinColor="red"
            />
          ))}
      </MapView>
    </View>
  );
}

const styles = StyleSheet.create({
  container: { flex: 1 },
  map: { flex: 1 },
  layerBar: {
    position: "absolute",
    top: 20,
    left: 0,
    right: 0,
    alignItems: "center",
    zIndex: 10,
  },
  layerButtons: {
    flexDirection: "row",
    backgroundColor: "white",
    padding: 10,
    borderRadius: 10,
    elevation: 4,
    gap: 10,
  },
  layerButton: {
    paddingHorizontal: 15,
    paddingVertical: 6,
    borderRadius: 6,
    backgroundColor: "#eee",
  },
  activeButton: {
    backgroundColor: "#0078ff",
    color: "white",
  },
});
