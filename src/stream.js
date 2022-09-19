const gstreamer = require("gstreamer-superficial");
const pipeline = new gstreamer.Pipeline(
  `nvarguscamerasrc sensor-id=0 name=src ! 
	nvv4l2h264enc ! 
	h264parse ! 
	rtspclientsink location=rtsp://localhost:8554/live protocols=tcp`
);

// docker run --restart always -d --network=host aler9/rtsp-simple-server

// pipeline.pollBus((msg) => {
//   console.log(msg);
// });

module.exports = pipeline;
