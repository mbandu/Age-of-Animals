// ============================================================================
//  Age of Animals — Online Lobby / Matchmaking Server
//  
//  A lightweight Node.js server that handles:
//  - Room creation and listing
//  - Player join/leave
//  - Match state tracking
//  - IP exchange for direct P2P connection
//  
//  Once matched, players connect directly via Unreal Engine's network
//  replication (listen server). This server just facilitates discovery.
//
//  Run: node lobby_server.js
//  Default port: 3000
// ============================================================================

const http = require("http");
const url = require("url");
const crypto = require("crypto");

const PORT = process.env.PORT || 3000;
const MAX_ROOMS = 100;
const ROOM_TIMEOUT_MS = 60 * 1000; // 60 seconds without heartbeat = room expires

// In-memory room store (in production, use Redis or a database)
const rooms = new Map();

// ---- Room management ----

function createRoom(body) {
  const room = {
    id: crypto.randomBytes(8).toString("hex"),
    roomName: body.roomName || "Untitled Game",
    hostName: body.playerName || "Host",
    hostEmpire: body.empireIndex || 0,
    hostIP: body.hostIP || "127.0.0.1",
    hostPort: body.hostPort || 7777,
    maxPlayers: body.maxPlayers || 5,
    isPrivate: body.isPrivate || false,
    mapName: body.mapName || "Random Map",
    players: [
      { name: body.playerName || "Host", empire: body.empireIndex || 0, ready: false }
    ],
    createdAt: Date.now(),
    lastHeartbeat: Date.now(),
  };
  rooms.set(room.id, room);
  return room;
}

function joinRoom(roomId, body) {
  const room = rooms.get(roomId);
  if (!room) return { error: "Room not found" };
  if (room.players.length >= room.maxPlayers) return { error: "Room is full" };
  
  room.players.push({
    name: body.playerName || "Player",
    empire: body.empireIndex || 0,
    ready: false,
  });
  room.lastHeartbeat = Date.now();
  return room;
}

function leaveRoom(roomId, body) {
  const room = rooms.get(roomId);
  if (!room) return { ok: true };
  
  room.players = room.players.filter(p => p.name !== body.playerName);
  if (room.players.length === 0) {
    rooms.delete(roomId);
  }
  return { ok: true };
}

function listRooms() {
  const now = Date.now();
  // Clean up expired rooms
  for (const [id, room] of rooms) {
    if (now - room.lastHeartbeat > ROOM_TIMEOUT_MS) {
      rooms.delete(id);
    }
  }
  
  return Array.from(rooms.values())
    .filter(r => !r.isPrivate)
    .map(r => ({
      id: r.id,
      roomName: r.roomName,
      hostName: r.hostName,
      hostEmpire: r.hostEmpire,
      numPlayers: r.players.length,
      maxPlayers: r.maxPlayers,
      hostIP: r.hostIP,
      hostPort: r.hostPort,
      isPrivate: r.isPrivate,
      mapName: r.mapName,
    }));
}

function heartbeat(roomId) {
  const room = rooms.get(roomId);
  if (room) room.lastHeartbeat = Date.now();
}

// ---- HTTP request handler ----

function readBody(req) {
  return new Promise((resolve) => {
    let data = "";
    req.on("data", chunk => { data += chunk; });
    req.on("end", () => {
      try { resolve(JSON.parse(data)); }
      catch { resolve({}); }
    });
  });
}

function sendJSON(res, status, data) {
  res.writeHead(status, {
    "Content-Type": "application/json",
    "Access-Control-Allow-Origin": "*",
    "Access-Control-Allow-Methods": "GET, POST, OPTIONS",
    "Access-Control-Allow-Headers": "Content-Type",
  });
  res.end(JSON.stringify(data));
}

const server = http.createServer(async (req, res) => {
  const parsed = url.parse(req.url, true);
  const path = parsed.pathname;

  // CORS preflight
  if (req.method === "OPTIONS") {
    sendJSON(res, 200, {});
    return;
  }

  // ---- API Routes ----

  // GET /api/rooms — list available rooms
  if (path === "/api/rooms" && req.method === "GET") {
    sendJSON(res, 200, { rooms: listRooms() });
    return;
  }

  // POST /api/rooms/create — create a new room
  if (path === "/api/rooms/create" && req.method === "POST") {
    if (rooms.size >= MAX_ROOMS) {
      sendJSON(res, 503, { error: "Server is full" });
      return;
    }
    const body = await readBody(req);
    const room = createRoom(body);
    sendJSON(res, 200, room);
    return;
  }

  // POST /api/rooms/:id/join — join a room
  const joinMatch = path.match(/^\/api\/rooms\/([^\/]+)\/join$/);
  if (joinMatch && req.method === "POST") {
    const body = await readBody(req);
    const result = joinRoom(joinMatch[1], body);
    if (result.error) {
      sendJSON(res, 400, result);
    } else {
      sendJSON(res, 200, {
        hostIP: result.hostIP,
        hostPort: result.hostPort,
        roomId: result.id,
      });
    }
    return;
  }

  // POST /api/rooms/:id/leave — leave a room
  const leaveMatch = path.match(/^\/api\/rooms\/([^\/]+)\/leave$/);
  if (leaveMatch && req.method === "POST") {
    const body = await readBody(req);
    leaveRoom(leaveMatch[1], body);
    sendJSON(res, 200, { ok: true });
    return;
  }

  // POST /api/rooms/:id/heartbeat — keep room alive
  const hbMatch = path.match(/^\/api\/rooms\/([^\/]+)\/heartbeat$/);
  if (hbMatch && req.method === "POST") {
    heartbeat(hbMatch[1]);
    sendJSON(res, 200, { ok: true });
    return;
  }

  // GET /api/health — server health check
  if (path === "/api/health") {
    sendJSON(res, 200, { status: "ok", rooms: rooms.size });
    return;
  }

  // GET / — serve a simple browser lobby page
  if (path === "/" && req.method === "GET") {
    res.writeHead(200, { "Content-Type": "text/html" });
    res.end(`
<!DOCTYPE html>
<html>
<head><title>Age of Animals — Online Lobby</title>
<style>
  body { font-family: 'Segoe UI', sans-serif; background: #1a1a2e; color: #e0e0e0; margin: 0; padding: 20px; }
  h1 { color: #4CAF50; text-align: center; }
  #rooms { max-width: 800px; margin: 0 auto; }
  .room { background: #16213e; border-radius: 8px; padding: 16px; margin: 8px 0; display: flex; justify-content: space-between; align-items: center; }
  .room button { background: #4CAF50; color: white; border: none; padding: 8px 16px; border-radius: 4px; cursor: pointer; }
  .room button:hover { background: #45a049; }
  .meta { color: #888; font-size: 0.85em; }
  #create { text-align: center; margin: 20px 0; }
  #create button { background: #2196F3; color: white; border: none; padding: 10px 24px; border-radius: 4px; cursor: pointer; font-size: 1.1em; }
</style>
</head>
<body>
  <h1>🐼 Age of Animals — Online Lobby 🐱</h1>
  <div id="create"><button onclick="createRoom()">Create New Game</button></div>
  <div id="rooms"></div>
  <script>
    async function refresh() {
      const res = await fetch('/api/rooms');
      const data = await res.json();
      const el = document.getElementById('rooms');
      el.innerHTML = '';
      for (const r of data.rooms) {
        const div = document.createElement('div');
        div.className = 'room';
        div.innerHTML = '<div><strong>' + r.roomName + '</strong><br><span class="meta">' + r.mapName + ' — ' + r.hostName + ' — ' + r.numPlayers + '/' + r.maxPlayers + ' players</span></div><button onclick="join(\\''+r.id+'\\')">Join</button>';
        el.appendChild(div);
      }
      if (data.rooms.length === 0) el.innerHTML = '<p style="text-align:center;color:#888">No games available. Create one!</p>';
    }
    async function createRoom() {
      const name = prompt('Room name:', 'My Game');
      if (!name) return;
      await fetch('/api/rooms/create', { method:'POST', headers:{'Content-Type':'application/json'}, body: JSON.stringify({ roomName:name, playerName:'Browser', maxPlayers:5, mapName:'Random Map' }) });
      refresh();
    }
    async function join(id) {
      alert('Join room: ' + id + '\\nIn the game client, use the Online Lobby to connect.');
    }
    refresh();
    setInterval(refresh, 3000);
  </script>
</body>
</html>
    `);
    return;
  }

  sendJSON(res, 404, { error: "Not found" });
});

server.listen(PORT, () => {
  console.log(`Age of Animals lobby server running on port ${PORT}`);
  console.log(`Open http://localhost:${PORT} for the browser lobby`);
});
