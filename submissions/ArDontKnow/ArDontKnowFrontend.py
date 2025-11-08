<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
  <title>GPS Tracker - Frontend</title>

  <!-- Leaflet Map -->
  <link rel="stylesheet" href="https://unpkg.com/leaflet/dist/leaflet.css" />
  <script src="https://unpkg.com/leaflet/dist/leaflet.js"></script>

  <!-- Google Font -->
  <link href="https://fonts.googleapis.com/css2?family=Inter:wght@400;600;700&display=swap" rel="stylesheet">

  <style>
    :root {
      --bg: #f5f7fa;
      --card-bg: rgba(255, 255, 255, 0.6);
      --text: #1c1c1c;
      --accent: #0077ff;
      --shadow: rgba(0, 0, 0, 0.1);
    }

    [data-theme="dark"] {
      --bg: #0f172a;
      --card-bg: rgba(30, 41, 59, 0.6);
      --text: #e2e8f0;
      --accent: #38bdf8;
      --shadow: rgba(0, 0, 0, 0.4);
    }

    * { box-sizing: border-box; }

    body {
      margin: 0;
      font-family: 'Inter', sans-serif;
      background: var(--bg);
      color: var(--text);
      transition: background 0.4s ease, color 0.4s ease;
    }

    header {
      display: flex;
      justify-content: space-between;
      align-items: center;
      padding: 1rem 2rem;
      background: var(--card-bg);
      backdrop-filter: blur(12px);
      box-shadow: 0 2px 8px var(--shadow);
      position: sticky;
      top: 0;
      z-index: 10;
    }

    header h1 {
      font-size: 1.5rem;
      font-weight: 700;
      color: var(--accent);
      letter-spacing: 0.4px;
    }

    button.theme-toggle {
      background: var(--accent);
      border: none;
      padding: 0.6rem 1.2rem;
      color: white;
      border-radius: 10px;
      font-weight: 600;
      cursor: pointer;
      transition: 0.3s;
      box-shadow: 0 3px 6px var(--shadow);
    }

    button.theme-toggle:hover {
      opacity: 0.85;
      transform: scale(1.05);
    }

    main {
      display: grid;
      grid-template-columns: 1fr 380px;
      gap: 1.5rem;
      padding: 1.5rem;
    }

    #map {
      height: 80vh;
      border-radius: 18px;
      box-shadow: 0 4px 15px var(--shadow);
      overflow: hidden;
    }

    .sidebar {
      background: var(--card-bg);
      backdrop-filter: blur(15px);
      border-radius: 18px;
      padding: 1.5rem;
      box-shadow: 0 4px 12px var(--shadow);
      animation: fadeIn 0.6s ease;
    }

    .card {
      margin-bottom: 1rem;
      padding: 1rem;
      border-radius: 14px;
      background: var(--card-bg);
      box-shadow: 0 3px 8px var(--shadow);
      transition: transform 0.2s ease;
    }

    .card:hover { transform: translateY(-4px); }

    .card h3 {
      margin-top: 0;
      color: var(--accent);
      font-size: 1.1rem;
    }

    .value {
      font-weight: 600;
      font-size: 1.1rem;
    }

    @keyframes fadeIn {
      from { opacity: 0; transform: translateY(10px); }
      to { opacity: 1; transform: translateY(0); }
    }

    footer {
      text-align: center;
      padding: 1rem;
      opacity: 0.8;
      font-size: 0.9rem;
    }

    @media (max-width: 850px) {
      main {
        grid-template-columns: 1fr;
      }
      #map {
        height: 60vh;
      }
    }
  </style>
</head>
<body data-theme="light">
  <header>
    <h1>📍 GPS Tracker Dashboard</h1>
    <button class="theme-toggle" onclick="toggleTheme()">Toggle Theme</button>
  </header>

  <main>
    <!-- Map -->
    <div id="map"></div>

    <!-- Sidebar -->
    <div class="sidebar">
      <div class="card">
        <h3>Trip Data</h3>
        <p><b>Speed:</b> <span id="speed" class="value">-- km/h</span></p>
        <p><b>Acceleration:</b> <span id="accel" class="value">-- m/s²</span></p>
        <p><b>Distance:</b> <span id="distance" class="value">-- km</span></p>
        <p><b>Avg Speed:</b> <span id="avg-speed" class="value">-- km/h</span></p>
        <p><b>Duration:</b> <span id="duration" class="value">--:--:--</span></p>
      </div>

      <div class="card">
        <h3>Coordinates</h3>
        <p><b>Latitude:</b> <span id="lat" class="value">--</span></p>
        <p><b>Longitude:</b> <span id="lng" class="value">--</span></p>
      </div>

      <div class="card">
        <h3>Status</h3>
        <p id="status">Awaiting backend data...</p>
      </div>
    </div>
  </main>

  <footer>Made with 💙 — Frontend ready for backend integration</footer>

  <script>
    // === Theme Toggle ===
    function toggleTheme() {
      const body = document.body;
      const newTheme = body.getAttribute("data-theme") === "light" ? "dark" : "light";
      body.setAttribute("data-theme", newTheme);
      localStorage.setItem("theme", newTheme);
    }
    document.body.setAttribute("data-theme", localStorage.getItem("theme") || "light");

    // === Map Setup ===
    const map = L.map("map").setView([12.9716, 77.5946], 14);
    L.tileLayer("https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png", {
      maxZoom: 19,
      attribution: '© OpenStreetMap contributors'
    }).addTo(map);

    // Marker placeholder — backend will update
    const marker = L.marker([12.9716, 77.5946]).addTo(map);
    marker.bindPopup("Waiting for live GPS data...").openPopup();

    // === Backend integration placeholder ===
    // You’ll replace this section later with live updates like:
    // fetch('/api/latest').then(...).then(updateUI);
  </script>
</body>
</html>