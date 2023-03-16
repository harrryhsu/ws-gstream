function http(
  method = "GET",
  url = "",
  data = null,
  cookie = null,
  contentType = "application/json"
) {
  const controller = new AbortController();
  const signal = controller.signal;

  var option = {
    method: method,
    mode: "cors",
    cache: "no-cache",
    credentials: "omit",
    headers: {
      "X-Auth": cookie,
      "Content-Type": contentType,
      Connection: "Keep-Alive",
    },
    redirect: "follow",
    referrerPolicy: "no-referrer",
    signal,
  };

  if (data !== null) {
    if (data instanceof FormData) {
      option.body = data;
      delete option.headers["Content-Type"];
    } else {
      option.body = JSON.stringify(data);
    }
  }

  var abort = false;
  const request = fetch(url, option).catch((err) => {
    if (err.name == "AbortError") abort = true;
    else throw err;
  });

  const wrapper = (request) => ({
    request,
    abort: () => controller.abort(),
    then: (callback) =>
      wrapper(request.then((rest) => !abort && callback(rest))),
    onabort: (callback) =>
      wrapper(request.finally((rest) => abort && callback(rest))),
    catch: (callback) =>
      wrapper(request.catch((rest) => !abort && callback(rest))),
  });

  return wrapper(request).catch((err) => {
    if (err.message === "Failed to fetch") throw { code: "ER_CONNECTION" };
    else throw err;
  });
}

function jsonUnwrap(req) {
  return req
    .then((res) => res.json().then((data) => ({ data, status: res.status })))
    .then(({ data, status }) => {
      if (data.hasOwnProperty("streams")) return data;
      if (!data.status || status != 200) throw data;
      return data?.data;
    });
}

function buildQuery(query) {
  if (!query) return "";
  const params = new URLSearchParams(query);
  return params.toString();
}

const GetToken = () => "";

const ApiWrapper = () => {
  const baseUrl = "";
  const wsUrl = `ws://${window.location.host}`;

  function get(path) {
    return jsonUnwrap(http("GET", `${baseUrl}/api/${path}`, null, GetToken()));
  }

  function post(path, body) {
    return jsonUnwrap(http("POST", `${baseUrl}/api/${path}`, body, GetToken()));
  }

  function postDownload(path, body) {
    return http("POST", `${baseUrl}/api/${path}`, body, GetToken())
      .then((res) => res.blob())
      .then((blob) => download(blob, body.filename));
  }

  function put(path, body) {
    return jsonUnwrap(http("PUT", `${baseUrl}/api/${path}`, body, GetToken()));
  }

  function del(path, body) {
    return jsonUnwrap(
      http("DELETE", `${baseUrl}/api/${path}`, body, GetToken())
    );
  }

  return {
    wsUrl,
    focusUp: () => post("focus/up"),
    focusDown: () => post("focus/down"),
    zoomUp: () => post("zoom/up"),
    zoomDown: () => post("zoom/down"),
    init: () => post("init"),
    getSetting: () => get("setting"),
    postSetting: (body) => post("setting", body),
  };
};

const api = ApiWrapper();

const start = (wsUrl) => {
  var player = new Player({
    useWorker: true,
    workerFile: "lib/Decoder.js",
    webgl: true,
  });
  const app = document.getElementById("app");
  app.appendChild(player.canvas);
  const canvas = app.querySelector("canvas");
  canvas.style.width = "1080px";
  canvas.style.height = "720px";
  var ws = new WebSocket(wsUrl);
  ws.onmessage = async function (data) {
    const buffer = await data.data.arrayBuffer();
    const view = new Int8Array(buffer);
    player.decode(view);
  };
};

start(`ws://localhost:8080`);
