# ESP32 MPU6050 GUI (Vite + React)

This is a ready-to-deploy web app that connects to an ESP32 over Web Serial and parses the serial output produced by the sketch you provided. It shows live filtered linear acceleration, a small chart, and classification (Stationary/Walking/Scooter).

## How to run locally
1. Install Node.js (18+ recommended).
2. `npm install`
3. `npm run dev`
4. Open the site over `http://localhost:5173`. To use Web Serial, prefer `https` or `localhost`.

## Deploy to Vercel
1. Create a GitHub repo and push the project.
2. Import the repo into Vercel and deploy (default settings).
3. Grant the deployed site access to Web Serial via HTTPS (browser permission when prompted).

## Notes
- The browser must support the Web Serial API (Chrome, Edge).
- Close other serial monitors (Arduino IDE) before connecting.