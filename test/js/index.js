const start = (type, url) => {
  const app = document.getElementById("app");
  const canvas = document.createElement("canvas");
  app.appendChild(canvas);

  const decoder = new WebDecoder({
    url,
    canvas,
    type,
    width: 1080,
    height: 720,
  });
};

start("h265", `ws://localhost:8080/test1`);
// start("h264", `ws://localhost:8080/test2`);
