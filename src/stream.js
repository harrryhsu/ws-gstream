const gstreamer = require("gstreamer-superficial");
var stream;
if (process.env.NODE_ENV == "development") {
  stream = new gstreamer.Pipeline(`
		rtspsrc location=rtsp://wowzaec2demo.streamlock.net/vod/mp4:BigBuckBunny_115k.mp4 name=src !
		rtph264depay ! h264parse ! avdec_h264 ! videoconvert ! 
		x264enc bframes=0 weightb=false speed-preset=1 cabac=false ! 
		queue !
		appsink name=appsink
	`);
} else {
  stream = new gstreamer.Pipeline(`
		nvarguscamerasrc sensor-id=0 name=src ! 
		nvv4l2h264enc ! 
		tee name=t ! queue ! h264parse ! rtspclientsink location=rtsp://localhost:8554/live protocols=tcp
		t. ! appsink name=appsink
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
stream.setSrcProp = (prop, value) => {
  src[prop] = value;
};

stream.restart = () => {
  stream.stop();
  stream.play();
};

module.exports = stream;
