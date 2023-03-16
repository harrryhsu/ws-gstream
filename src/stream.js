const gstreamer = require("./gstreamer-superficial");

const Stream = (rtsp) => {
  var stream = new gstreamer.Pipeline(`
		rtspsrc location=${rtsp} name=src !
		rtph264depay ! h264parse ! avdec_h264 ! videoconvert ! 
		x264enc bitrate=1000000 bframes=0 key-int-max=10 weightb=false speed-preset=1 cabac=false tune=zerolatency ! 
		queue ! appsink name=sink
	`);

  const appsink = stream.findChild("sink");
  const onData = (cb) => (buf, caps) => {
    if (buf) cb(buf);
    appsink.pull(onData(cb));
  };

  stream.onFrame = (cb) => {
    appsink.pull(onData(cb));
  };

  stream.restart = () => {
    stream.stop();
    stream.play();
  };

  return stream;
};

module.exports = Stream;
