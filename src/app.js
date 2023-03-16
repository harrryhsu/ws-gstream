const express = require("express");
const bodyParser = require("body-parser");
const cookieParser = require("cookie-parser");
const Stream = require("./stream");
const ws = require("ws");
const path = require("path");

const app = express();
const sockets = [];
const wss = new ws.Server({
  noServer: true,
  perMessageDeflate: false,
});

const isDev = process.env.NODE_ENV !== "production";
const rtsp = isDev
  ? "rtsp://wowzaec2demo.streamlock.net/vod/mp4:BigBuckBunny_115k.mp4"
  : "rtsp://admin:903fjjjjj@192.168.1.203/Streaming/Channels/201";

const stream = Stream(rtsp, 1280, 720, 15);

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

stream.onFrame((buf) => wss.broadcast(buf));
stream.play();

app
  .listen(8080, () => console.log(`App listening at http://localhost:8080`))
  .on("upgrade", (request, socket, head) => {
    wss.handleUpgrade(request, socket, head, (websocket) => {
      wss.emit("connection", websocket, request);
    });
  });

const exitHandler = (...args) => {
  stream.stop();
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
