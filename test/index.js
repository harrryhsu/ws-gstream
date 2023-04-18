const start = (wsUrl) => {
  var player = new Player({
    useWorker: true,
    workerFile: "lib/Decoder.js",
    webgl: true,
  });
  const app = document.getElementById("app");
  app.appendChild(player.canvas);
  player.canvas.style.width = "1080px";
  player.canvas.style.height = "720px";
  var ws = new WebSocket(wsUrl);
  ws.onmessage = async function (data) {
    const buffer = await data.data.arrayBuffer();
    const view = new Int8Array(buffer);
    player.decode(view);
  };
};

start(`ws://localhost:8080/test1`);
start(`ws://localhost:8080/test2`);
