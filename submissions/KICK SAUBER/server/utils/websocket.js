import { Server } from "socket.io";

export const initWebSocket = (httpServer) => {
  const io = new Server(httpServer, {
    cors: { origin: "http://localhost:5173" },
  });

  io.on("connection", (socket) => {
    console.log("Socket connected:", socket.id);
  });

  return io;
};
