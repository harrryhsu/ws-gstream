const gstreamer = require("gstreamer-superficial");
const pipeline = new gstreamer.Pipeline(
  `nvarguscamerasrc sensor-id=0 name=src ! 
	'video/x-raw(memory:NVMM),width=1920, height=1080, framerate=30/1, format=NV12' ! 
	nvv4l2h264enc ! 
	h264parse ! 
	rtspclientsink location=rtsp://localhost:8554/live protocols=tcp`
);

// docker run --restart always -d --network=host aler9/rtsp-simple-server

// pipeline.pollBus((msg) => {
//   console.log(msg);
// });

module.exports = pipeline;
