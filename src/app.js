require("dotenv").config();
const express = require("express");
const bodyParser = require("body-parser");
const cookieParser = require("cookie-parser");
const ci2c = require("./ci2c");
const stream = requrie("./stream");

const app = express();

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

app.get("/api/init", (req, res) => {
  ci2c.Lens_FindMCU();
  ci2c.Lens_ICRMode(true);
  ci2c.Lens_ReadLensData();
  ci2c.Lens_Initial();
  ci2c.Piris_Initial();
  okay(res);
});

app.get("/api/focus/up", (req, res) => {
  ci2c.Lens_Focusmove(16);
  okay(res);
});

app.get("/api/focus/down", (req, res) => {
  ci2c.Lens_Focusmove(-16);
  okay(res);
});

app.get("/api/zoom/up", (req, res) => {
  ci2c.Lens_Zoommove(Lens_GetZoommoveStep() + 1);
  okay(res);
});

app.get("/api/zoom/down", (req, res) => {
  ci2c.Lens_Zoommove(Lens_GetZoommoveStep() - 1);
  okay(res);
});

stream.play();
app.listen(8080, () => console.log(`App listening at http://localhost:8080`));
