const gstreamer = require("./gstreamer-superficial");

const Stream = ({ rtsp, width, height, fps }) => {
  var stream = new gstreamer.Pipeline(`
		rtspsrc location=${rtsp} name=src !
		rtph264depay ! h264parse ! avdec_h264 ! 
		queue ! videoconvert ! videoscale ! video/x-raw,width=${width},height=${height},framerate=${fps}/1 !
		x264enc bitrate=1000000 bframes=0 key-int-max=10 weightb=false speed-preset=1 cabac=false tune=zerolatency ! 
		appsink name=sink
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
