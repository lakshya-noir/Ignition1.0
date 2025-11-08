const express = require("express");
const app = express();

app.use(express.json());

let latestLocation = null;
let bumps = [];
let potholes = [];

function distanceMeters(lat1, lon1, lat2, lon2) {
  const R = 6371000;
  const dLat = (lat2 - lat1) * Math.PI / 180;
  const dLon = (lon2 - lon1) * Math.PI / 180;

  const a =
    Math.sin(dLat / 2) * Math.sin(dLat / 2) +
    Math.cos(lat1 * Math.PI / 180) *
      Math.cos(lat2 * Math.PI / 180) *
      Math.sin(dLon / 2) *
      Math.sin(dLon / 2);

  const c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a));
  return R * c;
}

app.post("/location", (req, res) => {
  latestLocation = req.body;
  console.log("GPS:", latestLocation);
  res.json({ status: "ok" });
});

app.post("/bump", (req, res) => {
  if (!latestLocation) {
    console.log("BUMP received but NO GPS yet");
    return res.json({ status: "no gps" });
  }

  console.log("BUMP RECEIVED:", req.body);

  const entry = {
    lat: latestLocation.lat,
    lon: latestLocation.lon,
    time: Date.now(),
    delta: req.body.delta
  };

  bumps.push(entry);
  console.log("Stored bump at:", entry);

  const nearby = bumps.filter((b) => {
    return distanceMeters(b.lat, b.lon, entry.lat, entry.lon) < 5;
  });

  console.log("Nearby bumps:", nearby.length);

  if (nearby.length >= 5) {
    const exists = potholes.some((p) => {
      return distanceMeters(p.lat, p.lon, entry.lat, entry.lon) < 5;
    });

    if (!exists) {
      potholes.push({
        lat: entry.lat,
        lon: entry.lon,
        time: Date.now()
      });
      console.log("✅ POTHOLE CONFIRMED at:", entry);
    }
  }

  res.json({ status: "stored" });
});

app.get("/bumps", (req, res) => {
  res.json(bumps);
});

app.get("/potholes", (req, res) => {
  res.json(potholes);
});

app.listen(5000, () => {
  console.log("Server running");
});
