const child_process = require("child_process");
const Events = require("events");

class Stream extends Events {
  constructor(options) {
    super();
    const { url, onFrame, ffmpegPath = "ffmpeg" } = options;

    this.spawnOptions = [
      "-y",
      "-rtsp_transport",
      "tcp",
      "-i",
      url,
      "-r",
      "30",
      "-probesize",
      "32",
      "-analyzeduration",
      "0",
      "-coder",
      "0",
      "-bf",
      "0",
      "-b:v",
      "100k",
      "-vf",
      "scale=600:400",
      "-x264-params",
      "keyint=30:scenecut=0",
      "-profile:v",
      "baseline",
      "-preset",
      "ultrafast",
      "-tune",
      "zerolatency",
      "-c:v",
      "libx264",
      "-f",
      "h264",
      "-",
    ];
    this.stream = child_process.spawn(ffmpegPath, this.spawnOptions, {
      detached: false,
    });
    this.stream.stdout.on("data", (data) => {
      onFrame(data);
    });
    this.stream.stderr.on("data", (data) => {
      console.log(data.toString());
      return this.emit("ffmpegData", data);
    });
    this.stream.on("exit", (code, signal) => {
      console.error("RTSP stream exited with error: " + code);
      return this.emit("exit");
    });
  }
}

module.exports = Stream;
