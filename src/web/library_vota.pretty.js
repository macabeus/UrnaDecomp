function js_use_external_keyboard() {
  Module.uenuxExternalKeyboard = true;
}
function js_ler_dimensao_tela(indice, valorPadrao) {
  const lerNumeroUrl = (nome) => {
    if (!globalThis.location) {
      return 0;
    }
    const parametros = new URLSearchParams(globalThis.location.search);
    const valor = Number(parametros.get(nome));
    return Number.isFinite(valor) && valor > 0 ? valor : 0;
  };
  const lerResolucaoUrl = () => {
    if (!globalThis.location) {
      return 0;
    }
    const parametros = new URLSearchParams(globalThis.location.search);
    const texto = parametros.get("screen") || parametros.get("resolution") || parametros.get("resolucao");
    const resultado = /^([0-9]+)x([0-9]+)$/i.exec(texto || "");
    if (!resultado) {
      return 0;
    }
    const valor = Number(resultado[indice + 1]);
    return Number.isFinite(valor) && valor > 0 ? valor : 0;
  };
  const nomeUrl = indice == 0 ? "screenWidth" : "screenHeight";
  const nomeModulo = indice == 0 ? "votaScreenWidth" : "votaScreenHeight";
  let valor = lerNumeroUrl(nomeUrl) || lerResolucaoUrl();
  if (!valor && typeof Module !== "undefined") {
    valor = Number(Module[nomeModulo]) || 0;
  }
  if (!valor && typeof globalThis !== "undefined") {
    valor = Number(globalThis[nomeModulo]) || 0;
  }
  return valor > 0 ? valor : valorPadrao;
}
function js_wasm_beep_init() {
  if (typeof window === "undefined") {
    return;
  }
  const AudioContextCtor = window.AudioContext || window.webkitAudioContext;
  if (!AudioContextCtor) {
    return;
  }
  if (Module.uenuxBeep) {
    return;
  }
  const estado = (Module.uenuxBeep = {
    contexto: null,
    saida: null,
    volume: 0.2,
    fila: [],
    tocando: false,
    proximoIdReproducao: 0,
    idReproducaoAtual: 0,
    retomadaPendente: false,
    tamanhoMaximoFila: 32,
  });
  function garantirContexto() {
    if (!estado.contexto) {
      estado.contexto = new AudioContextCtor();
      estado.saida = estado.contexto.createGain();
      estado.saida.gain.value = estado.volume;
      estado.saida.connect(estado.contexto.destination);
    }
    return estado.contexto;
  }
  function ajustarVolumeSaida() {
    if (estado.contexto && estado.saida) {
      estado.saida.gain.setValueAtTime(estado.volume, estado.contexto.currentTime);
    }
  }
  let tocarProximo = function () {};
  function retomarAudio() {
    const contexto = garantirContexto();
    if (!contexto) {
      return;
    }
    if (contexto.state === "running") {
      tocarProximo();
      return;
    }
    if (estado.retomadaPendente || typeof contexto.resume !== "function") {
      return;
    }
    estado.retomadaPendente = true;
    contexto
      .resume()
      .then(function () {
        estado.retomadaPendente = false;
        if (contexto.state === "running") {
          tocarProximo();
        } else {
          estado.fila.length = 0;
        }
      })
      .catch(function () {
        estado.retomadaPendente = false;
        estado.fila.length = 0;
      });
  }
  tocarProximo = function () {
    if (estado.tocando) {
      return;
    }
    const contexto = garantirContexto();
    if (!contexto) {
      estado.fila.length = 0;
      return;
    }
    if (contexto.state !== "running") {
      retomarAudio();
      return;
    }
    const item = estado.fila.shift();
    if (!item) {
      return;
    }
    estado.tocando = true;
    const idReproducao = ++estado.proximoIdReproducao;
    estado.idReproducaoAtual = idReproducao;
    const duracaoMs = Math.max(0, item.duracaoMs);
    const duracaoSegundos = duracaoMs / 1e3;
    const frequencia = Math.max(0, item.frequencia);
    if (frequencia < 20 || duracaoMs <= 0) {
      setTimeout(function () {
        if (estado.idReproducaoAtual !== idReproducao) {
          return;
        }
        estado.tocando = false;
        tocarProximo();
      }, duracaoMs);
      return;
    }
    const oscilador = contexto.createOscillator();
    const ganho = contexto.createGain();
    const inicio = contexto.currentTime + 0.002;
    const fim = inicio + duracaoSegundos;
    const envelope = Math.min(0.008, Math.max(0.001, duracaoSegundos / 4));
    oscilador.type = "square";
    oscilador.frequency.setValueAtTime(Math.min(2e4, frequencia), inicio);
    ganho.gain.setValueAtTime(0, inicio);
    ganho.gain.linearRampToValueAtTime(1, inicio + envelope);
    ganho.gain.setValueAtTime(1, Math.max(inicio + envelope, fim - envelope));
    ganho.gain.linearRampToValueAtTime(0, fim);
    oscilador.connect(ganho);
    ganho.connect(estado.saida);
    oscilador.onended = function () {
      if (estado.idReproducaoAtual !== idReproducao) {
        return;
      }
      try {
        oscilador.disconnect();
        ganho.disconnect();
      } catch (erro) {}
      estado.tocando = false;
      tocarProximo();
    };
    oscilador.start(inicio);
    oscilador.stop(fim);
    setTimeout(function () {
      if (estado.tocando && estado.idReproducaoAtual === idReproducao && contexto.currentTime > fim + 0.25) {
        estado.tocando = false;
        tocarProximo();
      }
    }, duracaoMs + 500);
  };
  function enfileirar(frequencia, duracaoMs) {
    const contexto = garantirContexto();
    if (!contexto) {
      return;
    }
    if (estado.fila.length >= estado.tamanhoMaximoFila) {
      estado.fila.shift();
    }
    estado.fila.push({ frequencia: Number(frequencia) || 0, duracaoMs: Math.max(0, Number(duracaoMs) || 0) });
    if (contexto.state !== "running") {
      retomarAudio();
      return;
    }
    tocarProximo();
  }
  function instalarDesbloqueio() {
    if (typeof document === "undefined") {
      return;
    }
    const desbloquear = function () {
      retomarAudio();
    };
    ["pointerdown", "keydown", "touchstart"].forEach(function (evento) {
      document.addEventListener(evento, desbloquear, { capture: true, passive: true });
    });
  }
  Module.uenuxBeepSetVolume = function (volume) {
    estado.volume = Math.max(0, Math.min(1, (Number(volume) || 0) / 100));
    ajustarVolumeSaida();
  };
  Module.uenuxBeepQueue = enfileirar;
  instalarDesbloqueio();
}
function js_wasm_beep_set_volume(volume) {
  if (Module.uenuxBeepSetVolume) {
    Module.uenuxBeepSetVolume(volume);
  }
}
function js_wasm_beep_queue(frequency, durationMs) {
  if (Module.uenuxBeepQueue) {
    Module.uenuxBeepQueue(frequency, durationMs);
  }
}
function wasm_input_has_key() {
  return Module.uenuxKeys && Module.uenuxKeys.length ? 1 : 0;
}
function wasm_input_get_key() {
  if (!Module.uenuxKeys || !Module.uenuxKeys.length) {
    return 0;
  }
  return Module.uenuxKeys.shift().charCodeAt(0);
}
function wasm_input_clear() {
  Module.uenuxKeys = [];
}
function js_thread_log(msg) {
  console.log("[CWasmThread]", UTF8ToString(msg));
}
function js_emit_event(name, detail) {
  const eventName = UTF8ToString(name);
  const rawDetail = UTF8ToString(detail);
  let parsedDetail = {};
  try {
    parsedDetail = rawDetail ? JSON.parse(rawDetail) : {};
  } catch (error) {
    parsedDetail = { raw: rawDetail, parseError: String(error) };
  }
  window.dispatchEvent(new CustomEvent(eventName, { detail: parsedDetail }));
}
function js_push_key(key) {
  Module.uenuxKeys = Module.uenuxKeys || [];
  Module.uenuxKeys.push(UTF8ToString(key));
}
function js_console_error(message) {
  console.error("[vota_web_wasm]", UTF8ToString(message));
}
function js_wasm_web_sound_play_wav(header, headerSize, data, dataSize, volume, modo) {
  function getState() {
    if (!Module.uenuxWebSound) {
      Module.uenuxWebSound = { audio: null, muted: false, queue: [], url: null, status: 0 };
    }
    if (!Module.uenuxWebSound.queue) {
      Module.uenuxWebSound.queue = [];
    }
    return Module.uenuxWebSound;
  }
  function stopCurrent(state, clearQueue) {
    if (clearQueue) {
      state.queue = [];
    }
    if (state.audio) {
      state.audio.pause();
      state.audio.removeAttribute("src");
      if (typeof state.audio.load === "function") {
        state.audio.load();
      }
    }
    if (state.url) {
      URL.revokeObjectURL(state.url);
    }
    state.audio = null;
    state.url = null;
    state.status = state.queue.length > 0 ? 1 : 0;
  }
  function startNext(state) {
    if (state.audio || state.queue.length === 0 || state.muted) {
      if (!state.audio && state.queue.length === 0) {
        state.status = 0;
      }
      return;
    }
    var item = state.queue.shift();
    var blob = new Blob([item.bytes], { type: "audio/wav" });
    var url = URL.createObjectURL(blob);
    var audio = new Audio(url);
    audio.volume = Math.max(0, Math.min(1, item.volume / 100));
    state.audio = audio;
    state.url = url;
    state.status = 1;
    var cleanup = function () {
      if (state.audio === audio) {
        if (state.url) {
          URL.revokeObjectURL(state.url);
        }
        state.audio = null;
        state.url = null;
        state.status = state.queue.length > 0 ? 1 : 0;
        startNext(state);
      }
    };
    audio.onended = cleanup;
    audio.onerror = cleanup;
    var playPromise = audio.play();
    if (playPromise && typeof playPromise.catch === "function") {
      playPromise.catch(cleanup);
    }
  }
  var state = getState();
  if (state.muted || !header || !data || headerSize <= 0 || dataSize <= 0) {
    if (modo === 0) {
      stopCurrent(state, true);
    }
    return;
  }
  if (modo === 0) {
    stopCurrent(state, true);
  }
  var bytes = new Uint8Array(headerSize + dataSize);
  bytes.set(HEAPU8.subarray(header, header + headerSize), 0);
  bytes.set(HEAPU8.subarray(data, data + dataSize), headerSize);
  state.queue.push({ bytes, volume });
  state.status = 1;
  startNext(state);
}
function js_wasm_web_sound_pause() {
  var state = Module.uenuxWebSound;
  if (state && state.audio) {
    state.audio.pause();
    state.status = 2;
  }
}
function js_wasm_web_sound_stop() {
  var state = Module.uenuxWebSound;
  if (!state) {
    return;
  }
  state.queue = [];
  if (state.audio) {
    state.audio.pause();
    state.audio.removeAttribute("src");
    if (typeof state.audio.load === "function") {
      state.audio.load();
    }
  }
  if (state.url) {
    URL.revokeObjectURL(state.url);
  }
  state.audio = null;
  state.url = null;
  state.status = 0;
}
function js_wasm_web_sound_mute(mute) {
  if (!Module.uenuxWebSound) {
    Module.uenuxWebSound = { audio: null, muted: false, queue: [], url: null, status: 0 };
  }
  Module.uenuxWebSound.muted = !!mute;
  if (!Module.uenuxWebSound.queue) {
    Module.uenuxWebSound.queue = [];
  }
  if (mute && Module.uenuxWebSound.audio) {
    Module.uenuxWebSound.queue = [];
    Module.uenuxWebSound.audio.pause();
    Module.uenuxWebSound.audio.removeAttribute("src");
    if (typeof Module.uenuxWebSound.audio.load === "function") {
      Module.uenuxWebSound.audio.load();
    }
    if (Module.uenuxWebSound.url) {
      URL.revokeObjectURL(Module.uenuxWebSound.url);
    }
    Module.uenuxWebSound.audio = null;
    Module.uenuxWebSound.url = null;
    Module.uenuxWebSound.status = 0;
  }
}
function js_wasm_web_sound_get_status() {
  var state = Module.uenuxWebSound;
  if (!state || !state.audio) {
    return state && state.queue && state.queue.length > 0 ? 1 : 0;
  }
  if (state.audio.ended) {
    return state.queue && state.queue.length > 0 ? 1 : 0;
  }
  if (state.status === 2) {
    return 2;
  }
  if (state.status === 1) {
    return 1;
  }
  return state.audio.paused ? 2 : 1;
}
function js_wasm_web_sound_wait_async(id) {
  function getStatus() {
    var state = Module.uenuxWebSound;
    if (!state || !state.audio) {
      return state && state.queue && state.queue.length > 0 ? 1 : 0;
    }
    if (state.audio.ended) {
      return state.queue && state.queue.length > 0 ? 1 : 0;
    }
    if (state.status === 2) {
      return 2;
    }
    if (state.status === 1) {
      return 1;
    }
    return state.audio.paused ? 2 : 1;
  }
  if (!Module.uenuxWebSoundWaits) {
    Module.uenuxWebSoundWaits = {};
  }
  var waits = Module.uenuxWebSoundWaits;
  if (waits[id]) {
    clearInterval(waits[id]);
  }
  waits[id] = setInterval(function () {
    var cancelado = Module.ccall("uenux_wasm_web_sound_wait_cancel_requested", "number", ["number"], [id]);
    if (cancelado || getStatus() !== 1) {
      clearInterval(waits[id]);
      delete waits[id];
      Module.ccall("uenux_wasm_web_sound_wait_finished", null, ["number", "number"], [id, cancelado ? 0 : 1]);
    }
  }, 20);
}
function js_wasm_web_sound_cancel_wait(id) {
  var waits = Module.uenuxWebSoundWaits;
  if (waits && waits[id]) {
    clearInterval(waits[id]);
    delete waits[id];
  }
}
function js_resource_log(action, resource, path, size) {
  if (Module.uenuxDebug) {
    console.debug("[CWasmResource]", UTF8ToString(action), UTF8ToString(resource), UTF8ToString(path), size);
  }
}
function js_init(width, height) {
  if (Module.uenuxDebug) {
    console.debug("[CWasmScreen] js_init", width, height);
  }
  Module.uenuxKeys = Module.uenuxKeys || [];
  const canvas = document.getElementById("uenux-screen");
  canvas.width = width;
  canvas.height = height;
  const ctx = canvas.getContext("2d");
  ctx.textBaseline = "top";
  ctx.imageSmoothingEnabled = false;
  function pushKey(key) {
    Module.uenuxKeys.push(key);
  }
  const hasExternalKeyboard = Module.uenuxExternalKeyboard && document.querySelector("[data-vota-key]");
  if (!hasExternalKeyboard) {
    document.querySelectorAll("[data-key]").forEach(function (button) {
      button.addEventListener("click", function () {
        pushKey(button.dataset.key);
      });
    });
    document.addEventListener("keydown", function (event) {
      const key = event.key;
      if (key.length == 1 && key >= "0" && key <= "9") pushKey(key);
      if (key == "Enter") pushKey("C");
      if (key == "Backspace" || key == "Escape") pushKey("D");
      if (key == " ") pushKey("B");
    });
  }
}
function js_clear(c) {
  const canvas = document.getElementById("uenux-screen");
  const ctx = canvas.getContext("2d");
  if (Module.uenuxGifAnimations) {
    Module.uenuxGifAnimations.forEach(function (animation) {
      if (animation.url) URL.revokeObjectURL(animation.url);
      if (animation.image && animation.image.parentNode)
        animation.image.parentNode.removeChild(animation.image);
    });
    Module.uenuxGifAnimations.clear();
  }
  ctx.fillStyle = UTF8ToString(c);
  ctx.fillRect(0, 0, canvas.width, canvas.height);
}
function js_fill(x, y, w, h, c) {
  const ctx = document.getElementById("uenux-screen").getContext("2d");
  ctx.fillStyle = UTF8ToString(c);
  ctx.fillRect(x, y, w, h);
}
function js_line(x1, y1, x2, y2, c, width) {
  const ctx = document.getElementById("uenux-screen").getContext("2d");
  ctx.strokeStyle = UTF8ToString(c);
  ctx.lineWidth = Math.max(width, 1);
  ctx.beginPath();
  ctx.moveTo(x1, y1);
  ctx.lineTo(x2, y2);
  ctx.stroke();
}
function js_text(x, y, maxw, text, size, bold, italic, c, align) {
  const ctx = document.getElementById("uenux-screen").getContext("2d");
  const value = UTF8ToString(text);
  const style = (italic ? "italic " : "") + (bold ? "bold " : "");
  ctx.font =
    style +
    Math.max(size, 1) +
    'px "Atkinson Hyperlegible Next", "AtkinsonHyperlegibleNext", Arial, sans-serif';
  ctx.fillStyle = UTF8ToString(c);
  ctx.textAlign = align == 1 ? "center" : align == 2 ? "right" : "left";
  ctx.textBaseline = "alphabetic";
  ctx.fillText(value, x, y);
}
function js_measure_text_width(text, size, bold, italic) {
  const ctx = document.getElementById("uenux-screen").getContext("2d");
  const value = UTF8ToString(text);
  const style = (italic ? "italic " : "") + (bold ? "bold " : "");
  ctx.font =
    style +
    Math.max(size, 1) +
    'px "Atkinson Hyperlegible Next", "AtkinsonHyperlegibleNext", Arial, sans-serif';
  return Math.ceil(ctx.measureText(value).width);
}
function js_circle(x, y, r, c, width) {
  const ctx = document.getElementById("uenux-screen").getContext("2d");
  ctx.strokeStyle = UTF8ToString(c);
  ctx.lineWidth = Math.max(width, 1);
  ctx.beginPath();
  ctx.arc(x, y, r, 0, 2 * Math.PI);
  ctx.stroke();
}
function js_round_rect(x, y, w, h, r, c, width) {
  const ctx = document.getElementById("uenux-screen").getContext("2d");
  ctx.strokeStyle = UTF8ToString(c);
  ctx.lineWidth = Math.max(width, 1);
  const radius = Math.max(0, Math.min(r, Math.floor(Math.min(w, h) / 2)));
  ctx.beginPath();
  if (ctx.roundRect) {
    ctx.roundRect(x, y, w, h, radius);
  } else {
    ctx.moveTo(x + radius, y);
    ctx.lineTo(x + w - radius, y);
    ctx.quadraticCurveTo(x + w, y, x + w, y + radius);
    ctx.lineTo(x + w, y + h - radius);
    ctx.quadraticCurveTo(x + w, y + h, x + w - radius, y + h);
    ctx.lineTo(x + radius, y + h);
    ctx.quadraticCurveTo(x, y + h, x, y + h - radius);
    ctx.lineTo(x, y + radius);
    ctx.quadraticCurveTo(x, y, x + radius, y);
  }
  ctx.stroke();
}
function js_gray_gradient(x, y, w, h) {
  const ctx = document.getElementById("uenux-screen").getContext("2d");
  const gradient = ctx.createLinearGradient(x, y, x + w, y);
  gradient.addColorStop(0, "#ffffff");
  gradient.addColorStop(1, "#000000");
  ctx.fillStyle = gradient;
  ctx.fillRect(x, y, w, h);
}
function js_path(count, types, values, c, fill, width) {
  const ctx = document.getElementById("uenux-screen").getContext("2d");
  ctx.beginPath();
  for (let i = 0; i < count; ++i) {
    const type = HEAP32[(types >> 2) + i];
    const base = (values >> 3) + i * 6;
    const x1 = HEAPF64[base];
    const y1 = HEAPF64[base + 1];
    const x2 = HEAPF64[base + 2];
    const y2 = HEAPF64[base + 3];
    const qtStartAngle = HEAPF64[base + 4];
    const qtSweepAngle = HEAPF64[base + 5];
    if (type == 0) {
      ctx.moveTo(x1, y1);
    } else if (type == 1) {
      ctx.lineTo(x1, y1);
    } else if (type == 2) {
      const startAngle = (-qtStartAngle * Math.PI) / 180;
      const endAngle = (-(qtStartAngle + qtSweepAngle) * Math.PI) / 180;
      ctx.ellipse(
        x1 + x2 / 2,
        y1 + y2 / 2,
        Math.abs(x2 / 2),
        Math.abs(y2 / 2),
        0,
        startAngle,
        endAngle,
        qtSweepAngle > 0,
      );
    } else if (type == 3) {
      ctx.closePath();
    }
  }
  if (fill) {
    ctx.fillStyle = UTF8ToString(c);
    ctx.fill();
  } else {
    ctx.strokeStyle = UTF8ToString(c);
    ctx.lineWidth = Math.max(width, 1);
    ctx.stroke();
  }
}
function js_image(data, size, x, y, w, h) {
  if (!data || size <= 0 || w <= 0 || h <= 0) return;
  const canvas = document.getElementById("uenux-screen");
  const ctx = canvas.getContext("2d");
  const bytes = HEAPU8.slice(data, data + size);
  let mime = "application/octet-stream";
  if (size >= 8 && bytes[0] == 137 && bytes[1] == 80 && bytes[2] == 78 && bytes[3] == 71) mime = "image/png";
  else if (size >= 3 && bytes[0] == 255 && bytes[1] == 216 && bytes[2] == 255) mime = "image/jpeg";
  else if (size >= 3 && bytes[0] == 71 && bytes[1] == 73 && bytes[2] == 70) mime = "image/gif";
  let hash = 2166136261;
  for (let i = 0; i < size; ++i) {
    hash ^= bytes[i];
    hash = Math.imul(hash, 16777619) >>> 0;
  }
  let key = mime + ":" + size + ":" + hash.toString(16);
  Module.uenuxImageCache = Module.uenuxImageCache || new Map();
  Module.uenuxImageLogKeys = Module.uenuxImageLogKeys || new Set();
  if (Module.uenuxDebug && !Module.uenuxImageLogKeys.has(key)) {
    Module.uenuxImageLogKeys.add(key);
    console.debug("[CWasmScreen] image", mime, size, x, y, w, h);
  }
  const draw = function (image) {
    ctx.drawImage(image, x, y, w, h);
  };
  if (mime == "image/gif") {
    Module.uenuxGifAnimations = Module.uenuxGifAnimations || new Map();
    const syncLayer = function (layer) {
      const rect = canvas.getBoundingClientRect();
      layer.style.left = canvas.offsetLeft + "px";
      layer.style.top = canvas.offsetTop + "px";
      layer.style.width = (rect.width || canvas.clientWidth || canvas.width) + "px";
      layer.style.height = (rect.height || canvas.clientHeight || canvas.height) + "px";
    };
    const ensureLayer = function () {
      let layer = document.getElementById("uenux-gif-layer");
      if (layer) {
        syncLayer(layer);
        return layer;
      }
      const parent = canvas.parentElement || canvas;
      if (parent != canvas && getComputedStyle(parent).position == "static")
        parent.style.position = "relative";
      canvas.style.position = canvas.style.position || "relative";
      canvas.style.zIndex = canvas.style.zIndex || "1";
      layer = document.createElement("div");
      layer.id = "uenux-gif-layer";
      layer.style.position = "absolute";
      syncLayer(layer);
      layer.style.pointerEvents = "none";
      layer.style.overflow = "hidden";
      layer.style.zIndex = "2";
      if (parent == canvas) {
        document.body.appendChild(layer);
      } else {
        parent.appendChild(layer);
      }
      return layer;
    };
    const placeImage = function (image) {
      image.style.position = "absolute";
      image.style.left = "";
      image.style.right = "";
      image.style.top = "";
      image.style.bottom = "";
      if (x + w >= canvas.width) {
        image.style.right = "0";
      } else {
        image.style.left = (x / canvas.width) * 100 + "%";
      }
      if (y + h >= canvas.height) {
        image.style.bottom = "0";
      } else {
        image.style.top = (y / canvas.height) * 100 + "%";
      }
      image.style.width = (w / canvas.width) * 100 + "%";
      image.style.height = (h / canvas.height) * 100 + "%";
      image.style.objectFit = "fill";
      image.style.pointerEvents = "none";
    };
    ensureLayer();
    if (Module.uenuxGifAnimations.has(key)) {
      const animation = Module.uenuxGifAnimations.get(key);
      animation.x = x;
      animation.y = y;
      animation.w = w;
      animation.h = h;
      placeImage(animation.image);
      return;
    }
    Module.uenuxGifAnimations.forEach(function (animation) {
      if (animation.url) URL.revokeObjectURL(animation.url);
      if (animation.image && animation.image.parentNode)
        animation.image.parentNode.removeChild(animation.image);
    });
    Module.uenuxGifAnimations.clear();
    const blob = new Blob([bytes], { type: mime });
    const image = new Image();
    const animation = { x, y, w, h, image, url: URL.createObjectURL(blob) };
    placeImage(image);
    image.src = animation.url;
    ensureLayer().appendChild(image);
    Module.uenuxGifAnimations.set(key, animation);
    return;
  }
  if (Module.uenuxImageCache.has(key)) {
    Module.uenuxImageCache.get(key).then(draw);
    return;
  }
  const blob = new Blob([bytes], { type: mime });
  let promise;
  if (typeof createImageBitmap == "function") {
    promise = createImageBitmap(blob);
  } else {
    promise = new Promise(function (resolve, reject) {
      const image = new Image();
      image.onload = function () {
        URL.revokeObjectURL(image.src);
        resolve(image);
      };
      image.onerror = reject;
      image.src = URL.createObjectURL(blob);
    });
  }
  Module.uenuxImageCache.set(key, promise);
  promise.then(draw).catch(function (error) {
    console.warn("[CWasmScreen] image decode failed", error);
  });
}
function js_log(msg) {
  if (Module.uenuxDebug) {
    console.debug("[CWasmScreen]", UTF8ToString(msg));
  }
}
function js_mt_set(text, busy) {
  const mt = document.getElementById("mt");
  mt.textContent = UTF8ToString(text);
  mt.style.borderColor = busy ? "#34a853" : "#5f6368";
}
function js_obter_data_hora_local_navegador() {
  const agora = new Date();
  return agora.getTime() / 1e3 - agora.getTimezoneOffset() * 60;
}
