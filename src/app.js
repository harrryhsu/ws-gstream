require("dotenv").config();
const express = require("express");
const bodyParser = require("body-parser");
const cookieParser = require("cookie-parser");
const ci2c = require("./ci2c");
const stream = require("./stream");
const ws = require("ws");
const path = require("path");
const wsStream = require("./wsStream");

const app = express();
const sockets = [];
const wss = new ws.Server({
  noServer: true,
  perMessageDeflate: false,
});

const isDev = process.env.NODE_ENV === "development";

wss.on("connection", (socket, request) => {
  sockets.push(socket);
  socket.reqUrl = request.url;

  socket.on("close", () => {
    const index = sockets.indexOf(socket);
    if (index !== -1) sockets.splice(index, 1);
  });
});

wss.broadcast = (data) => sockets.forEach((x) => x.send(data));

const okay = (res, data) => {
  res.contentType("application/json").status(200).send({
    status: true,
    data: data,
  });
};

const fail = (res, code = "", msg = "", status = 400, data = {}) => {
  res.contentType("application/json").status(status).send({
    status: false,
    code: code,
    message: msg,
    data: data,
  });
};

const error = (res) => (msg) => {
  log.error(msg.toString());
  res.contentType("application/json").status(500).send({
    status: false,
    message: msg.toString(),
    code: "ER_UNKNOWN",
  });
};

app.use(bodyParser.json({ limit: "200mb" }));
app.use(bodyParser.urlencoded({ limit: "200mb", extended: true }));
app.use(cookieParser());

app.use(function (req, res, next) {
  const origin = req.header("origin");
  if (origin) {
    res.header("Access-Control-Allow-Origin", origin);
  } else {
    res.header("Access-Control-Allow-Origin", "*");
  }
  res.header("Access-Control-Allow-Credentials", true);
  res.header(
    "Access-Control-Allow-Headers",
    "Origin, X-Requested-With, Content-Type, Accept, X-Auth"
  );
  res.header("Access-Control-Allow-Methods", "GET, POST, PUT, OPTION, DELETE");
  req.reqTime = new Date();
  next();
});

app.use(express.static("public"));

app.post("/api/init", (req, res) => {
  console.log("API: Init");
  if (!isDev) {
    ci2c.Lens_ICRMode(true);
    ci2c.Lens_ReadLensData();
    ci2c.Lens_Initial();
    ci2c.Piris_Initial();
  }
  okay(res);
});

app.post("/api/focus/up", (req, res) => {
  console.log("API: Focus up");
  if (!isDev) ci2c.Lens_Focusmove(16);
  okay(res);
});

app.post("/api/focus/down", (req, res) => {
  console.log("API: Focus down");
  if (!isDev) ci2c.Lens_Focusmove(-16);
  okay(res);
});

app.post("/api/zoom/up", (req, res) => {
  console.log("API: Zoom up");
  if (!isDev) ci2c.Lens_Zoommove(ci2c.Lens_GetZoommoveStep() + 1);
  okay(res);
});

app.post("/api/zoom/down", (req, res) => {
  console.log("API: Zoom down");
  if (!isDev) ci2c.Lens_Zoommove(ci2c.Lens_GetZoommoveStep() - 1);
  okay(res);
});

app.get("/api/setting", (req, res) => {
  okay(res, stream.setting);
});

app.post("/api/setting", (req, res) => {
  const setting = req.body;
  if (process.env.NODE_ENV === "production") {
    Object.keys(setting).forEach((key) => stream.setSrcProp(key, setting[key]));
    // stream.restart();
  } else
    Object.keys(setting).forEach((key) => (stream.setting[key] = setting[key]));
  okay(res);
});

app.get("*.map", function (req, res) {
  fail(res, "ER_INVALID", "Does not support debug symbol", 404);
});

const indexFile = path.join(process.cwd(), "./public/index.html");

// Fallback for react router
app.get("*", function (req, res) {
  res.sendFile(indexFile);
});

app.use((err, req, res, next) => {
  error(res)(err);
});

if (!isDev && !ci2c.Lens_FindMCU()) throw "No i2c bus found";
// stream.onFrame((buf) => wss.broadcast(buf));
stream.play();

app
  .listen(8080, () => console.log(`App listening at http://localhost:8080`))
  .on("upgrade", (request, socket, head) => {
    wss.handleUpgrade(request, socket, head, (websocket) => {
      wss.emit("connection", websocket, request);
    });
  });

const waitForRtsp = () => {
  setTimeout(() => {
    const sstream = new wsStream({
      url:
        process.env.NODE_ENV == "development"
          ? "rtsp://host.docker.internal:8554/live"
          : "rtsp://localhost:8554/live",
      onFrame: (frame) => wss.broadcast(frame),
    });

    sstream.on("exit", waitForRtsp);
  }, 3000);
};

waitForRtsp();

const exitHandler = (...args) => {
  console.log(args);
  stream.stop();
  ci2c.i2c_close();
  process.exit();
};

//do something when app is closing
process.on("exit", exitHandler);

//catches ctrl+c event
process.on("SIGINT", exitHandler);

// catches "kill pid" (for example: nodemon restart)
process.on("SIGUSR1", exitHandler);
process.on("SIGUSR2", exitHandler);

//catches uncaught exceptions
process.on("uncaughtException", exitHandler);
