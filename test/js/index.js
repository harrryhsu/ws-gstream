const start = (type, url) => {
  const app = document.getElementById("app");
  const canvas = document.createElement("canvas");
  app.appendChild(canvas);
  canvas.style.width = "1080px";
  canvas.style.height = "720px";

  const decoder = new WebDecoder({
    url,
    canvas,
    type,
    width: 1080,
    height: 720,
  });
};

start("h264", `ws://localhost:8080/test1`);
// start("h264", `ws://localhost:8080/test2`);
