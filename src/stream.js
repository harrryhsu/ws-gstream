const gstreamer = require("./gstreamer-superficial");
var stream;
if (process.env.NODE_ENV == "development") {
  stream = new gstreamer.Pipeline(`
		rtspsrc location=rtsp://wowzaec2demo.streamlock.net/vod/mp4:BigBuckBunny_115k.mp4 name=src !
		rtph264depay ! h264parse ! avdec_h264 ! videoconvert ! 
		x264enc bframes=0 weightb=false speed-preset=1 cabac=false ! 
		queue ! h264parse ! rtspclientsink location=rtsp://host.docker.internal:8554/live protocols=tcp
	`);
} else {
  stream = new gstreamer.Pipeline(`
		nvarguscamerasrc sensor-id=0 name=src ! 
		video/x-raw(memory:NVMM),width=1080,height=720,framerate=30/1,format=NV12 !
		nvv4l2h264enc num-B-Frames=0 disable-cabac=true preset-level=1 ! 
		queue ! h264parse ! rtspclientsink location=rtsp://localhost:8554/live protocols=tcp
	`);
}

// docker run --restart always -d --network=host aler9/rtsp-simple-server

// stream.pollBus((msg) => {
//   console.log(msg);
// });

const appsink = stream.findChild("appsink");
const onData = (cb) => (buf, caps) => {
  // if (caps) {
  //   console.log("CAPS", caps);
  // }
  // if (buf) {
  //   console.log("BUFFER size", buf.length);
  // }
  if (buf) cb(buf);
  appsink.pull(onData(cb));
};

stream.onFrame = (cb) => {
  appsink.pull(onData(cb));
};

const src = stream.findChild("src");
stream.setting = {};
stream.setSrcProp = (prop, value) => {
  src[prop] = value;
  stream.setting[prop] = value;
};

stream.restart = () => {
  stream.stop();
  stream.play();
};

const defaultSetting = {
  blocksize: 4096,
  "num-buffers": -1,
  "do-timestamp": true,
  silent: true,
  timeout: 0,
  wbmode: 1,
  saturation: 1,
  exposuretimerange: "34000 358733000",
  gainrange: "1 16",
  ispdigitalgainrange: "1 8",
  "tnr-strength": -1,
  "tnr-mode": 1,
  "ee-mode": 1,
  "ee-strength": -1,
  aeantibanding: 1,
  exposurecompensation: 0,
  aelock: false,
  awblock: false,
  maxperf: false,
  "bufapi-version": false,
};

if (process.env.NODE_ENV === "production") {
  Object.keys(defaultSetting).forEach((key) =>
    stream.setSrcProp(key, defaultSetting[key])
  );
} else {
  stream.setSrcProp("buffer-mode", 1);
  stream.setting = defaultSetting;
}

module.exports = stream;
