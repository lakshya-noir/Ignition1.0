export const receiveSensorData = (req, res) => {
  try {
    const io = req.app.get("io"); // get socket instance
    const data = req.body; // sensor sends JSON

    console.log(" Received sensor data:", data);

    // ✅ Broadcast data to all connected frontends
    io.emit("sensor:update", data);

    res.status(200).json({ success: true });
  } catch (err) {
    console.error("Error receiving data:", err);
    res.status(500).json({ error: "Failed to process sensor data" });
  }
};
