import React from "react";
import RiderTelemetryApp from "./RiderTelemetryUI";
import RiderClassifier from "./riderClassifier";

function App() {
  return (
    <div style={{ backgroundColor: "#0b0d10", minHeight: "100vh", color: "#e5e7eb" }}>
      {/* 🔹 Top bar to hide unwanted UI overlays */}
      <header
        style={{
          position: "fixed",
          top: 0,
          left: 0,
          width: "100%",
          height: "45px",
          background: "linear-gradient(to right, #0f172a, #1e293b)",
          borderBottom: "1px solid #334155",
          boxShadow: "0 2px 4px rgba(0,0,0,0.4)",
          display: "flex",
          alignItems: "center",
          justifyContent: "space-between",
          padding: "0 20px",
          zIndex: 9999,
        }}
      >
        <h3 style={{ color: "#cbd5e1", fontWeight: "500", fontSize: "1rem", letterSpacing: "0.5px" }}>
          🚀 VegaVath – Rider Dashboard
        </h3>
      </header>

      {/* 🔹 Content area shifted below the header */}
      <main style={{ paddingTop: "55px" }}>
        <RiderTelemetryApp />
        <RiderClassifier />
      </main>
    </div>
  );
}

export default App;
