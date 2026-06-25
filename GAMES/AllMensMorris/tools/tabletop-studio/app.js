const state = {
  boards: [],
  selected: null,
  tab: "graph",
  tool: "move",
  selectedPoint: null,
  dragPoint: null,
  edgeStart: null,
  millDraft: [],
  spriteTool: "black",
  spriteDrawing: false,
  spriteAssetPath: "assets/fx/banner.png",
  spriteAssetWidth: 128,
  spriteAssetHeight: 64,
  sounds: [],
  selectedSound: null,
  selectedThemeIndex: 0,
  audioEvents: [],
  audioKind: "music",
  rollDrag: null,
};

const el = {
  status: document.querySelector("#status"),
  boardSelect: document.querySelector("#boardSelect"),
  newBoardName: document.querySelector("#newBoardName"),
  createBoard: document.querySelector("#createBoard"),
  duplicateBoard: document.querySelector("#duplicateBoard"),
  saveBoard: document.querySelector("#saveBoard"),
  tabs: Array.from(document.querySelectorAll("[data-tab]")),
  tools: Array.from(document.querySelectorAll("[data-tool]")),
  showAdjacency: document.querySelector("#showAdjacency"),
  showMills: document.querySelector("#showMills"),
  showGrid: document.querySelector("#showGrid"),
  cursorInfo: document.querySelector("#cursorInfo"),
  selectionInfo: document.querySelector("#selectionInfo"),
  summary: document.querySelector("#summary"),
  validation: document.querySelector("#validation"),
  canvas: document.querySelector("#boardCanvas"),
  graphTab: document.querySelector("#graphTab"),
  rulesTab: document.querySelector("#rulesTab"),
  spritesTab: document.querySelector("#spritesTab"),
  rawTab: document.querySelector("#rawTab"),
  rulesForm: document.querySelector("#rulesForm"),
  rawJson: document.querySelector("#rawJson"),
  spriteAssetSelect: document.querySelector("#spriteAssetSelect"),
  spriteCanvas: document.querySelector("#spriteCanvas"),
  spriteTools: Array.from(document.querySelectorAll("[data-sprite-tool]")),
  spriteCursorInfo: document.querySelector("#spriteCursorInfo"),
  invertSprite: document.querySelector("#invertSprite"),
  reloadSprite: document.querySelector("#reloadSprite"),
  saveSprite: document.querySelector("#saveSprite"),
  audioTab: document.querySelector("#audioTab"),
  audioSelect: document.querySelector("#audioSelect"),
  audioThemeLabel: document.querySelector("#audioThemeLabel"),
  audioThemeSelect: document.querySelector("#audioThemeSelect"),
  audioName: document.querySelector("#audioName"),
  audioTickMs: document.querySelector("#audioTickMs"),
  newEffect: document.querySelector("#newEffect"),
  addRest: document.querySelector("#addRest"),
  playAudio: document.querySelector("#playAudio"),
  saveAudio: document.querySelector("#saveAudio"),
  audioStats: document.querySelector("#audioStats"),
  audioRoll: document.querySelector("#audioRoll"),
  audioEvents: document.querySelector("#audioEvents"),
};

const ctx = el.canvas.getContext("2d");
const spriteCtx = el.spriteCanvas.getContext("2d", { willReadFrequently: true });
spriteCtx.imageSmoothingEnabled = false;

const AUDIO_MIN_NOTE = 36;
const AUDIO_MAX_NOTE = 96;
const AUDIO_NOTE_HEIGHT = 16;
const AUDIO_PX_PER_UNIT = 10;
const AUDIO_SNAP_UNITS = 2;
function setStatus(text) {
  el.status.textContent = text;
}

function clone(value) {
  return JSON.parse(JSON.stringify(value));
}

function selectedBoard() {
  return state.selected ? state.selected.data : null;
}

function boardScale(board) {
  const canvas = board.canvas || { width: 64, height: 64 };
  return {
    x: el.canvas.width / canvas.width,
    y: el.canvas.height / canvas.height,
  };
}

function pointAt(board, index) {
  const point = board.points[index];
  const scale = boardScale(board);
  return {
    x: point.x * scale.x,
    y: point.y * scale.y,
  };
}

function canvasToBoard(event) {
  const board = selectedBoard();
  const rect = el.canvas.getBoundingClientRect();
  const canvas = board?.canvas || { width: 64, height: 64 };
  const x = ((event.clientX - rect.left) / rect.width) * canvas.width;
  const y = ((event.clientY - rect.top) / rect.height) * canvas.height;
  return {
    x: Math.max(0, Math.min(canvas.width - 1, Math.floor(x))),
    y: Math.max(0, Math.min(canvas.height - 1, Math.floor(y))),
  };
}

function hitPoint(event) {
  const board = selectedBoard();
  if (!board) return null;
  const rect = el.canvas.getBoundingClientRect();
  const x = ((event.clientX - rect.left) / rect.width) * el.canvas.width;
  const y = ((event.clientY - rect.top) / rect.height) * el.canvas.height;
  let best = null;
  let bestDistance = 18;
  (board.points || []).forEach((point, index) => {
    const p = pointAt(board, index);
    const distance = Math.hypot(p.x - x, p.y - y);
    if (distance < bestDistance) {
      best = index;
      bestDistance = distance;
    }
  });
  return best;
}

function drawLine(board, pair, color, width, dash = []) {
  if (!Array.isArray(pair) || pair.length < 2) return;
  if (!board.points[pair[0]] || !board.points[pair[1]]) return;
  const a = pointAt(board, pair[0]);
  const b = pointAt(board, pair[1]);
  ctx.strokeStyle = color;
  ctx.lineWidth = width;
  ctx.setLineDash(dash);
  ctx.beginPath();
  ctx.moveTo(a.x, a.y);
  ctx.lineTo(b.x, b.y);
  ctx.stroke();
  ctx.setLineDash([]);
}

function drawBoardConnection(board, pair) {
  drawLine(board, pair, "#050505", 3, [8, 5]);
  drawLine(board, pair, "#f7f7f7", 1.5, [8, 5]);
}

function drawPixelGrid(board) {
  if (!el.showGrid.checked) return;
  const canvas = board.canvas || { width: 64, height: 64 };
  const scale = boardScale(board);
  ctx.save();
  ctx.lineWidth = 1;
  for (let x = 0; x <= canvas.width; x++) {
    const px = Math.round(x * scale.x) + 0.5;
    ctx.strokeStyle = x % 4 === 0 ? "rgba(170, 170, 170, 0.44)" : "rgba(170, 170, 170, 0.20)";
    ctx.beginPath();
    ctx.moveTo(px, 0);
    ctx.lineTo(px, el.canvas.height);
    ctx.stroke();
  }
  for (let y = 0; y <= canvas.height; y++) {
    const py = Math.round(y * scale.y) + 0.5;
    ctx.strokeStyle = y % 4 === 0 ? "rgba(170, 170, 170, 0.44)" : "rgba(170, 170, 170, 0.20)";
    ctx.beginPath();
    ctx.moveTo(0, py);
    ctx.lineTo(el.canvas.width, py);
    ctx.stroke();
  }
  ctx.restore();
}

function millColor(index) {
  const hue = (index * 137.508) % 360;
  const lightness = index % 2 === 0 ? 62 : 72;
  return `hsl(${hue.toFixed(1)} 92% ${lightness}%)`;
}

function millSegments(mill) {
  return [
    [mill[0], mill[1]],
    [mill[1], mill[2]],
  ];
}

function sameSegment(left, right) {
  return left.length >= 2
    && right.length >= 2
    && ((left[0] === right[0] && left[1] === right[1])
      || (left[0] === right[1] && left[1] === right[0]));
}

function millSegmentOverlap(board, pair, millIndex) {
  const overlaps = [];
  (board.mills || []).forEach((mill, index) => {
    if (millSegments(mill).some((segment) => sameSegment(segment, pair))) {
      overlaps.push(index);
    }
  });
  return {
    count: overlaps.length,
    rank: overlaps.indexOf(millIndex),
  };
}

function drawMillSegment(board, pair, millIndex) {
  const overlap = millSegmentOverlap(board, pair, millIndex);
  const width = overlap.count > 1 ? 10 + (overlap.count - overlap.rank) * 4 : 10;
  drawLine(board, pair, millColor(millIndex), width);
}

function ensureGraphArrays(board) {
  board.points ||= [];
  board.adjacency ||= [];
  board.mills ||= [];
  while (board.adjacency.length < board.points.length) {
    board.adjacency.push([]);
  }
}

function edgeExists(board, a, b) {
  return (board.adjacency?.[a] || []).includes(b);
}

function toggleEdge(board, a, b) {
  ensureGraphArrays(board);
  if (a === b) return;
  [a, b].forEach((index) => {
    board.adjacency[index] ||= [];
  });
  if (edgeExists(board, a, b)) {
    board.adjacency[a] = board.adjacency[a].filter((value) => value !== b);
    board.adjacency[b] = board.adjacency[b].filter((value) => value !== a);
  } else {
    board.adjacency[a].push(b);
    board.adjacency[b].push(a);
    board.adjacency[a].sort((left, right) => left - right);
    board.adjacency[b].sort((left, right) => left - right);
  }
}

function normalizeMill(points) {
  return [...points].sort((a, b) => a - b);
}

function sameMill(left, right) {
  const a = normalizeMill(left);
  const b = normalizeMill(right);
  return a.length === 3 && b.length === 3 && a.every((value, index) => value === b[index]);
}

function toggleMill(board, points) {
  ensureGraphArrays(board);
  if (new Set(points).size !== 3) return;
  const existing = board.mills.findIndex((mill) => sameMill(mill, points));
  if (existing >= 0) {
    board.mills.splice(existing, 1);
  } else {
    board.mills.push([...points]);
  }
}

function removePoint(board, index) {
  ensureGraphArrays(board);
  board.points.splice(index, 1);
  board.adjacency.splice(index, 1);
  board.adjacency = board.adjacency.map((edges) =>
    (edges || [])
      .filter((to) => to !== index)
      .map((to) => (to > index ? to - 1 : to))
  );
  board.mills = (board.mills || [])
    .filter((mill) => !mill.includes(index))
    .map((mill) => mill.map((point) => (point > index ? point - 1 : point)));
}

function drawBoard() {
  const board = selectedBoard();
  ctx.fillStyle = "#080808";
  ctx.fillRect(0, 0, el.canvas.width, el.canvas.height);
  if (!board) return;
  drawPixelGrid(board);

  if (el.showMills.checked) {
    ctx.globalAlpha = 0.72;
    (board.mills || []).forEach((mill, index) => {
      drawMillSegment(board, [mill[0], mill[1]], index);
      drawMillSegment(board, [mill[1], mill[2]], index);
    });
    ctx.globalAlpha = 1;
  }

  if (el.showAdjacency.checked) {
    (board.adjacency || []).forEach((edges, index) => {
      (edges || []).forEach((to) => {
        if (to > index) drawBoardConnection(board, [index, to]);
      });
    });
  }

  (board.points || []).forEach((point, index) => {
    const p = pointAt(board, index);
    const selected = index === state.selectedPoint
      || state.millDraft.includes(index)
      || index === state.edgeStart;
    ctx.fillStyle = selected ? "#7ee787" : "#f7f7f7";
    ctx.beginPath();
    ctx.arc(p.x, p.y, selected ? 11 : 8, 0, Math.PI * 2);
    ctx.fill();
    ctx.fillStyle = "#050505";
    ctx.font = "14px ui-monospace, monospace";
    ctx.textAlign = "center";
    ctx.textBaseline = "middle";
    ctx.fillText(String(index), p.x, p.y);
  });
}

function validateBoard(board) {
  const issues = [];
  const points = board.points || [];
  const pointCount = points.length;

  if (!pointCount) issues.push("Board needs at least one point.");
  points.forEach((point, index) => {
    if (!Number.isFinite(point.x) || !Number.isFinite(point.y)) {
      issues.push(`Point ${index} needs numeric x/y.`);
    }
  });

  (board.adjacency || []).forEach((edges, from) => {
    if (!Array.isArray(edges)) {
      issues.push(`Adjacency ${from} must be an array.`);
      return;
    }
    const seen = new Set();
    edges.forEach((to) => {
      if (to < 0 || to >= pointCount) issues.push(`Edge ${from}-${to} references a missing point.`);
      if (to === from) issues.push(`Point ${from} cannot be adjacent to itself.`);
      if (seen.has(to)) issues.push(`Point ${from} has duplicate edge ${to}.`);
      seen.add(to);
      const reverse = board.adjacency?.[to] || [];
      if (!reverse.includes(from)) issues.push(`Edge ${from}-${to} is not bidirectional.`);
    });
  });

  (board.mills || []).forEach((mill, index) => {
    if (!Array.isArray(mill) || mill.length !== 3) {
      issues.push(`Mill ${index} must contain exactly 3 points.`);
      return;
    }
    if (new Set(mill).size !== 3) issues.push(`Mill ${index} repeats a point.`);
    mill.forEach((point) => {
      if (point < 0 || point >= pointCount) issues.push(`Mill ${index} references missing point ${point}.`);
    });
  });

  const rules = board.rules || {};
  [
    "piecesPerPlayer",
    "minPiecesToContinue",
    "flyPieceCount",
    "noCaptureDrawTurnLimit",
    "placementStopEmptyPoints",
  ].forEach((key) => {
    if (!Number.isInteger(rules[key]) || rules[key] < 0) issues.push(`Rule ${key} must be a positive integer.`);
  });
  if (!["capture", "win"].includes(rules.millAction)) {
    issues.push("Rule millAction must be capture or win.");
  }
  [
    "flyingEnabled",
    "mixedPlacementMovement",
    "protectPiecesInMills",
    "blockWinEnabled",
    "materialWinEnabled",
    "blockWinRequiresReserveEmpty",
    "materialWinRequiresReserveEmpty",
    "skipBlockedWithReserve",
  ].forEach((key) => {
    if (typeof rules[key] !== "boolean") issues.push(`Rule ${key} must be boolean.`);
  });

  return issues;
}

function renderSummary(board) {
  el.summary.innerHTML = "";
  if (!board) return;
  [
    `id: ${board.id}`,
    `label: ${board.label}`,
    `points: ${(board.points || []).length}`,
    `mills: ${(board.mills || []).length}`,
    `pieces: ${board.rules?.piecesPerPlayer ?? "?"}`,
  ].forEach((text) => {
    const item = document.createElement("div");
    item.textContent = text;
    el.summary.appendChild(item);
  });
}

function renderSelection() {
  if (state.tool === "edge" && state.edgeStart !== null) {
    el.selectionInfo.textContent = `Connection start: ${state.edgeStart}`;
  } else if (state.tool === "mill" && state.millDraft.length) {
    el.selectionInfo.textContent = `Mill: ${state.millDraft.join(", ")}`;
  } else if (state.selectedPoint !== null) {
    el.selectionInfo.textContent = `Point ${state.selectedPoint}`;
  } else {
    el.selectionInfo.textContent = `${state.tool} mode`;
  }
}

function renderValidation(board) {
  el.validation.innerHTML = "";
  if (!board) return;
  const issues = validateBoard(board);
  if (!issues.length) {
    const item = document.createElement("div");
    item.className = "ok";
    item.textContent = "Valid board profile.";
    el.validation.appendChild(item);
    return;
  }
  issues.forEach((issue) => {
    const item = document.createElement("div");
    item.className = "bad";
    item.textContent = issue;
    el.validation.appendChild(item);
  });
}

function renderRules(board) {
  el.rulesForm.innerHTML = "";
  if (!board) return;
  const rules = board.rules || {};
  Object.keys(rules).forEach((key) => {
    const label = document.createElement("label");
    label.textContent = key;
    const input = key === "millAction" ? document.createElement("select") : document.createElement("input");
    if (key === "millAction") {
      ["capture", "win"].forEach((value) => {
        const option = document.createElement("option");
        option.value = value;
        option.textContent = value;
        input.appendChild(option);
      });
      input.value = rules[key];
    } else {
      input.type = typeof rules[key] === "boolean" ? "checkbox" : "number";
    }
    if (input.type === "checkbox") {
      input.checked = Boolean(rules[key]);
    } else {
      input.value = rules[key];
    }
    input.addEventListener("input", () => {
      if (key === "millAction") {
        rules[key] = input.value;
      } else {
        rules[key] = input.type === "checkbox" ? input.checked : Number(input.value);
      }
      syncRawFromState();
      render();
    });
    label.appendChild(input);
    el.rulesForm.appendChild(label);
  });
}

function syncRawFromState() {
  const board = selectedBoard();
  el.rawJson.value = board ? JSON.stringify(board, null, 2) : "";
}

function syncStateFromRaw() {
  const data = JSON.parse(el.rawJson.value);
  state.selected.data = data;
}

function renderTabs() {
  el.graphTab.classList.toggle("hidden", state.tab !== "graph");
  el.rulesTab.classList.toggle("hidden", state.tab !== "rules");
  el.spritesTab.classList.toggle("hidden", state.tab !== "sprites");
  el.audioTab.classList.toggle("hidden", state.tab !== "audio");
  el.rawTab.classList.toggle("hidden", state.tab !== "raw");
  el.tabs.forEach((tab) => tab.classList.toggle("active", tab.dataset.tab === state.tab));
}

function render() {
  const board = selectedBoard();
  renderTabs();
  el.canvas.parentElement.classList.toggle("gridHidden", !el.showGrid.checked);
  renderSummary(board);
  renderValidation(board);
  renderRules(board);
  renderSelection();
  drawBoard();
}

function makeBlankBoard(name) {
  const id = name.toLowerCase().replace(/[^a-z0-9_-]+/g, "-").replace(/^-|-$/g, "") || "board";
  return {
    schemaVersion: 1,
    id,
    name,
    label: name.toUpperCase().slice(0, 12),
    family: "custom-morris",
    canvas: { width: 64, height: 64 },
    rules: {
      piecesPerPlayer: 6,
      minPiecesToContinue: 3,
      flyPieceCount: 3,
      noCaptureDrawTurnLimit: 50,
      placementStopEmptyPoints: 0,
      millAction: "capture",
      flyingEnabled: true,
      mixedPlacementMovement: false,
      protectPiecesInMills: true,
      blockWinEnabled: true,
      materialWinEnabled: true,
      blockWinRequiresReserveEmpty: true,
      materialWinRequiresReserveEmpty: true,
      skipBlockedWithReserve: false,
    },
    points: [],
    mills: [],
    adjacency: [],
  };
}

function selectBoard(slug) {
  state.selected = state.boards.find((board) => board.slug === slug) || state.boards[0] || null;
  if (state.selected) el.boardSelect.value = state.selected.slug;
  syncRawFromState();
  render();
}

function populateSelect() {
  el.boardSelect.innerHTML = "";
  state.boards.forEach((board) => {
    const option = document.createElement("option");
    option.value = board.slug;
    option.textContent = board.data?.name || board.name || board.slug;
    el.boardSelect.appendChild(option);
  });
}

async function loadBoards() {
  setStatus("Loading boards");
  const response = await fetch("/api/boards");
  const payload = await response.json();
  state.boards = payload.boards || [];
  populateSelect();
  selectBoard(state.boards[0]?.slug);
  setStatus(`${payload.count} board profile(s)`);
}

async function loadGlobalSprite() {
  el.spriteCanvas.width = state.spriteAssetWidth;
  el.spriteCanvas.height = state.spriteAssetHeight;
  spriteCtx.imageSmoothingEnabled = false;
  const image = new Image();
  image.onload = () => {
    spriteCtx.clearRect(0, 0, el.spriteCanvas.width, el.spriteCanvas.height);
    spriteCtx.drawImage(image, 0, 0, el.spriteCanvas.width, el.spriteCanvas.height);
    setStatus(`Loaded ${state.spriteAssetPath}`);
  };
  image.onerror = () => {
    if (state.spriteAssetPath === "assets/ui/pieces.png") {
      drawDefaultUiPieces();
      setStatus("Started new UI pieces draft");
    } else {
      setStatus(`Could not load ${state.spriteAssetPath}`);
    }
  };
  image.src = `/${state.spriteAssetPath}?cache=${Date.now()}`;
}

function drawSpriteRows(x, rows) {
  spriteCtx.fillStyle = "#fff";
  rows.forEach((row, y) => {
    for (let px = 0; px < 16; px++) {
      if ((row >> (15 - px)) & 1) {
        spriteCtx.fillRect(x + px, y, 1, 1);
      }
    }
  });
}

function drawDefaultUiPieces() {
  el.spriteCanvas.width = 32;
  el.spriteCanvas.height = 10;
  spriteCtx.imageSmoothingEnabled = false;
  spriteCtx.fillStyle = "#000";
  spriteCtx.fillRect(0, 0, 32, 10);
  drawSpriteRows(0, [
    0b0000011111100000,
    0b0001111111111000,
    0b0011111111111100,
    0b0111111111111110,
    0b0111111111111110,
    0b0011111111111100,
    0b0001111111111000,
    0b0011111111111100,
    0b0011111111111100,
    0b0000111111110000,
  ]);
  drawSpriteRows(16, [
    0b0000011111100000,
    0b0001111111111000,
    0b0011110000111100,
    0b0111100000011110,
    0b0111100000011110,
    0b0011110000111100,
    0b0001111111111000,
    0b0011000000001100,
    0b0011111111111100,
    0b0000111111110000,
  ]);
}

function spritePoint(event) {
  const rect = el.spriteCanvas.getBoundingClientRect();
  const x = ((event.clientX - rect.left) / rect.width) * el.spriteCanvas.width;
  const y = ((event.clientY - rect.top) / rect.height) * el.spriteCanvas.height;
  return {
    x: Math.max(0, Math.min(el.spriteCanvas.width - 1, Math.floor(x))),
    y: Math.max(0, Math.min(el.spriteCanvas.height - 1, Math.floor(y))),
  };
}

function paintSpritePixel(event) {
  const point = spritePoint(event);
  spriteCtx.fillStyle = state.spriteTool === "black" ? "#000" : "#fff";
  spriteCtx.fillRect(point.x, point.y, 1, 1);
  el.spriteCursorInfo.textContent = `x ${point.x} y ${point.y}`;
}

function invertSprite() {
  const image = spriteCtx.getImageData(0, 0, el.spriteCanvas.width, el.spriteCanvas.height);
  for (let i = 0; i < image.data.length; i += 4) {
    image.data[i] = 255 - image.data[i];
    image.data[i + 1] = 255 - image.data[i + 1];
    image.data[i + 2] = 255 - image.data[i + 2];
  }
  spriteCtx.putImageData(image, 0, 0);
  setStatus(`Inverted ${state.spriteAssetPath} draft`);
}

async function saveGlobalSprite() {
  const response = await fetch("/api/save-global-asset", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({
      path: state.spriteAssetPath,
      dataUrl: el.spriteCanvas.toDataURL("image/png"),
    }),
  });
  const payload = await response.json();
  if (!payload.ok) throw new Error(payload.error || "Save failed");
  setStatus(`Saved ${payload.path}`);
}

function midiName(note) {
  if (note === 0) return "REST";
  const names = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"];
  return `${names[note % 12]}${Math.floor(note / 12) - 1}`;
}

function midiFrequency(note) {
  return note === 0 ? 0 : 440 * Math.pow(2, (note - 69) / 12);
}

function audioEventsDraft() {
  return state.audioEvents.map((event) => ({
    note: Number(event.note || 0),
    duration: Number(event.duration || 1),
  }));
}

function commitSelectedAudioTheme() {
  if (state.selectedSound?.id !== "menu-music") return;
  const themes = state.selectedSound.themes || [];
  if (!themes[state.selectedThemeIndex]) return;
  themes[state.selectedThemeIndex] = {
    ...themes[state.selectedThemeIndex],
    events: audioEventsDraft(),
  };
}

function audioDraft() {
  if (state.selectedSound?.id === "menu-music") {
    commitSelectedAudioTheme();
    const themes = (state.selectedSound.themes || []).map((theme, index) => ({
      ...theme,
      events: (theme.events || []).map((event) => ({
        note: Number(event.note || 0),
        duration: Number(event.duration || 1),
      })),
    }));
    return {
      id: "menu-music",
      name: el.audioName.value.trim() || "Menu Music",
      kind: "music",
      tickMs: Number(el.audioTickMs.value || 69),
      themes,
    };
  }

  return {
    id: state.selectedSound?.id || "menu-music",
    name: el.audioName.value.trim() || "Menu Music",
    kind: state.audioKind,
    tickMs: Number(el.audioTickMs.value || 69),
    events: audioEventsDraft(),
  };
}

function audioUnitsTotal() {
  return state.audioEvents.reduce((sum, event) => sum + Number(event.duration || 0), 0);
}

function audioSequenceNotes() {
  const notes = [];
  let start = 0;
  state.audioEvents.forEach((event) => {
    const duration = Number(event.duration || 0);
    const note = Number(event.note || 0);
    if (note > 0 && duration > 0) {
      notes.push({ note, start, duration });
    }
    start += duration;
  });
  return notes;
}

function setAudioEventsFromSequence(notes) {
  const sorted = notes
    .filter((note) => note.duration > 0 && note.note >= AUDIO_MIN_NOTE && note.note <= AUDIO_MAX_NOTE)
    .sort((left, right) => left.start - right.start);
  const events = [];
  let cursor = 0;
  sorted.forEach((note) => {
    const start = Math.max(note.start, cursor);
    if (start > cursor) {
      events.push({ note: 0, duration: start - cursor });
    }
    events.push({ note: note.note, duration: note.duration });
    cursor = start + note.duration;
  });
  state.audioEvents = events.length ? events : [{ note: 0, duration: AUDIO_SNAP_UNITS }];
  renderAudio();
}

function rollWidth(notes) {
  const lastNoteEnd = notes.reduce((max, note) => Math.max(max, note.start + note.duration), 0);
  const units = Math.max(audioUnitsTotal(), lastNoteEnd, 72);
  return units * AUDIO_PX_PER_UNIT + 120;
}

function removeRollNote(target) {
  const noteToRemove = {
    note: Number(target.dataset.note),
    start: Number(target.dataset.start),
    duration: Number(target.dataset.duration),
  };
  const next = audioSequenceNotes().filter((note) => !(
    note.note === noteToRemove.note
    && note.start === noteToRemove.start
    && note.duration === noteToRemove.duration
  ));
  setAudioEventsFromSequence(next);
}

function renderAudioRoll(preview = null) {
  el.audioRoll.replaceChildren();
  const notes = audioSequenceNotes();
  const width = rollWidth(notes.concat(preview ? [preview] : []));
  const labels = document.createElement("div");
  labels.className = "pianoLabels";
  const grid = document.createElement("div");
  grid.className = "rollGrid";
  grid.style.width = `${width}px`;

  for (let note = AUDIO_MAX_NOTE; note >= AUDIO_MIN_NOTE; note--) {
    const label = document.createElement("div");
    label.className = `pianoLabel${[1, 3, 6, 8, 10].includes(note % 12) ? " sharp" : ""}`;
    label.textContent = midiName(note);
    labels.append(label);

    const row = document.createElement("div");
    row.className = `rollRow${[1, 3, 6, 8, 10].includes(note % 12) ? " sharp" : ""}`;
    row.dataset.note = note;
    row.style.width = `${width}px`;
    grid.append(row);
  }

  function addBlock(note, className) {
    const row = grid.querySelector(`[data-note="${note.note}"]`);
    if (!row) return;
    const block = document.createElement("div");
    block.className = className;
    block.style.left = `${note.start * AUDIO_PX_PER_UNIT}px`;
    block.style.width = `${Math.max(5, note.duration * AUDIO_PX_PER_UNIT)}px`;
    block.dataset.note = note.note;
    block.dataset.start = note.start;
    block.dataset.duration = note.duration;
    block.title = `${midiName(note.note)} ${note.duration} unit(s)`;
    row.append(block);
  }

  notes.forEach((note) => addBlock(note, "rollNote"));
  if (preview) addBlock(preview, "rollPreview");

  grid.addEventListener("contextmenu", (event) => event.preventDefault());
  grid.addEventListener("pointerdown", (event) => {
    if (event.button === 2) {
      const target = event.target.closest(".rollNote");
      if (target) removeRollNote(target);
      return;
    }
    const row = event.target.closest(".rollRow");
    if (!row) return;
    const rect = grid.getBoundingClientRect();
    const unit = Math.max(0, Math.round((event.clientX - rect.left) / AUDIO_PX_PER_UNIT / AUDIO_SNAP_UNITS) * AUDIO_SNAP_UNITS);
    state.rollDrag = { note: Number(row.dataset.note), start: unit, current: unit };
    grid.setPointerCapture(event.pointerId);
    renderAudioRoll({ note: state.rollDrag.note, start: unit, duration: AUDIO_SNAP_UNITS });
  });

  el.audioRoll.append(labels, grid);
}

function updateRollDrag(event) {
  if (!state.rollDrag) return;
  const grid = el.audioRoll.querySelector(".rollGrid");
  if (!grid) return;
  const rect = grid.getBoundingClientRect();
  const unit = Math.max(0, Math.round((event.clientX - rect.left) / AUDIO_PX_PER_UNIT / AUDIO_SNAP_UNITS) * AUDIO_SNAP_UNITS);
  state.rollDrag.current = unit;
  const start = Math.min(state.rollDrag.start, state.rollDrag.current);
  const end = Math.max(state.rollDrag.start, state.rollDrag.current) + AUDIO_SNAP_UNITS;
  renderAudioRoll({ note: state.rollDrag.note, start, duration: end - start });
}

function finishRollDrag() {
  if (!state.rollDrag) return;
  const start = Math.min(state.rollDrag.start, state.rollDrag.current);
  const end = Math.max(state.rollDrag.start, state.rollDrag.current) + AUDIO_SNAP_UNITS;
  const notes = audioSequenceNotes();
  notes.push({ note: state.rollDrag.note, start, duration: end - start });
  state.rollDrag = null;
  setAudioEventsFromSequence(notes);
}

function renderAudioEvents() {
  el.audioEvents.replaceChildren();
  state.audioEvents.forEach((event, index) => {
    const row = document.createElement("div");
    row.className = "audioRow";

    const note = document.createElement("select");
    const rest = new Option("REST", "0");
    note.add(rest);
    for (let value = AUDIO_MAX_NOTE; value >= AUDIO_MIN_NOTE; value--) {
      note.add(new Option(midiName(value), String(value)));
    }
    note.value = String(event.note || 0);
    note.addEventListener("input", () => {
      state.audioEvents[index].note = Number(note.value);
      renderAudio();
    });

    const duration = document.createElement("input");
    duration.type = "number";
    duration.min = "1";
    duration.max = "255";
    duration.value = event.duration;
    duration.addEventListener("input", () => {
      state.audioEvents[index].duration = Number(duration.value || 1);
      renderAudio();
    });

    const remove = document.createElement("button");
    remove.textContent = "X";
    remove.addEventListener("click", () => {
      state.audioEvents.splice(index, 1);
      renderAudio();
    });

    row.append(note, duration, remove);
    el.audioEvents.append(row);
  });
}

function updateAudioStats() {
  const units = audioUnitsTotal();
  const ms = units * Number(el.audioTickMs.value || 69);
  el.audioStats.textContent = `${state.audioEvents.length} event(s), ${units} unit(s), ${Math.round(ms / 100) / 10}s`;
}

function renderAudio() {
  renderAudioRoll();
  renderAudioEvents();
  updateAudioStats();
}

function populateAudioSelect() {
  el.audioSelect.replaceChildren();
  state.sounds.forEach((sound) => {
    el.audioSelect.add(new Option(`${sound.name} (${sound.kind})`, sound.id));
  });
}

function populateAudioThemeSelect(sound) {
  el.audioThemeSelect.replaceChildren();
  const themes = sound?.themes || [];
  el.audioThemeLabel.classList.toggle("hidden", sound?.id !== "menu-music" || themes.length <= 1);
  themes.forEach((theme, index) => {
    el.audioThemeSelect.add(new Option(theme.name || theme.id || `Theme ${index + 1}`, String(index)));
  });
  el.audioThemeSelect.value = String(state.selectedThemeIndex);
}

function selectAudioTheme(index) {
  const themes = state.selectedSound?.themes || [];
  commitSelectedAudioTheme();
  state.selectedThemeIndex = Math.max(0, Math.min(themes.length - 1, index));
  const theme = themes[state.selectedThemeIndex];
  if (!theme) return;
  state.audioEvents = (theme.events || []).map((event) => ({ ...event }));
  el.audioThemeSelect.value = String(state.selectedThemeIndex);
  renderAudio();
}

function selectAudio(id) {
  const sound = state.sounds.find((item) => item.id === id) || state.sounds[0];
  if (!sound) return;
  state.selectedSound = sound;
  state.audioKind = sound.kind || "effect";
  state.selectedThemeIndex = 0;
  state.audioEvents = (sound.themes?.[0]?.events || sound.events || []).map((event) => ({ ...event }));
  el.audioSelect.value = sound.id;
  el.audioName.value = sound.name || sound.id;
  el.audioName.disabled = sound.id === "menu-music";
  el.audioTickMs.value = sound.tickMs || 69;
  populateAudioThemeSelect(sound);
  renderAudio();
}

async function loadAudio() {
  const response = await fetch("/api/audio", { cache: "no-store" });
  const payload = await response.json();
  state.sounds = payload.sounds || [];
  populateAudioSelect();
  selectAudio(state.sounds[0]?.id);
}

function createEffectDraft() {
  const id = `effect-${Date.now()}`;
  const sound = {
    id,
    name: "New Effect",
    kind: "effect",
    tickMs: 50,
    events: [{ note: 72, duration: 2 }],
  };
  state.sounds.push(sound);
  populateAudioSelect();
  selectAudio(id);
}

async function saveAudio() {
  const response = await fetch("/api/save-audio", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(audioDraft()),
  });
  const payload = await response.json();
  if (!payload.ok) throw new Error(payload.error || "Save failed");
  const selectedThemeIndex = state.selectedThemeIndex;
  await loadAudio();
  selectAudio(payload.sound.id);
  if (payload.sound.id === "menu-music") {
    selectAudioTheme(selectedThemeIndex);
  }
  setStatus(`Saved ${payload.sound.path}`);
}

async function playAudio() {
  const context = new AudioContext();
  let time = context.currentTime;
  const tickSeconds = Number(el.audioTickMs.value || 69) / 1000;
  state.audioEvents.forEach((event) => {
    const duration = Number(event.duration || 1) * tickSeconds;
    const frequency = midiFrequency(Number(event.note || 0));
    if (frequency > 0) {
      const oscillator = context.createOscillator();
      const gain = context.createGain();
      oscillator.type = "square";
      oscillator.frequency.value = frequency;
      gain.gain.setValueAtTime(0.04, time);
      gain.gain.exponentialRampToValueAtTime(0.0001, time + duration);
      oscillator.connect(gain);
      gain.connect(context.destination);
      oscillator.start(time);
      oscillator.stop(time + duration);
    }
    time += duration;
  });
  setStatus("Played browser audio preview");
}

async function saveBoard() {
  if (!state.selected) return;
  if (state.tab === "raw") syncStateFromRaw();
  const response = await fetch("/api/save-board", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ slug: state.selected.slug, data: state.selected.data }),
  });
  const payload = await response.json();
  if (!payload.ok) throw new Error(payload.error || "Save failed");
  state.selected.slug = payload.slug;
  state.selected.path = payload.path;
  state.selected.data = payload.data;
  populateSelect();
  selectBoard(payload.slug);
  setStatus(`Saved ${payload.path}`);
}

function duplicateBoard() {
  if (!state.selected) return;
  const name = el.newBoardName.value.trim() || `${state.selected.data.name} Copy`;
  const data = clone(state.selected.data);
  data.name = name;
  data.id = name.toLowerCase().replace(/[^a-z0-9_-]+/g, "-").replace(/^-|-$/g, "") || "board";
  data.label = name.toUpperCase().slice(0, 12);
  const board = { slug: data.id, path: `boards/${data.id}.json`, data };
  state.boards.push(board);
  populateSelect();
  selectBoard(board.slug);
  setStatus("Duplicated board draft");
}

function createBoard() {
  const name = el.newBoardName.value.trim() || "New Board";
  const data = makeBlankBoard(name);
  const board = { slug: data.id, path: `boards/${data.id}.json`, data };
  state.boards.push(board);
  populateSelect();
  selectBoard(board.slug);
  setStatus("Created blank board draft");
}

function syncEditedBoard() {
  syncRawFromState();
  render();
}

function handleCanvasDown(event) {
  const board = selectedBoard();
  if (!board) return;
  ensureGraphArrays(board);
  const hit = hitPoint(event);
  if (state.tool === "point") {
    const point = canvasToBoard(event);
    board.points.push(point);
    board.adjacency.push([]);
    state.selectedPoint = board.points.length - 1;
    setStatus(`Added point ${state.selectedPoint}`);
    syncEditedBoard();
    return;
  }
  if (hit === null) return;
  state.selectedPoint = hit;
  if (state.tool === "move") {
    state.dragPoint = hit;
  } else if (state.tool === "edge") {
    if (state.edgeStart === null) {
      state.edgeStart = hit;
      setStatus(`Selected connection start ${hit}`);
    } else {
      toggleEdge(board, state.edgeStart, hit);
      setStatus(`Toggled connection ${state.edgeStart}-${hit}`);
      state.edgeStart = null;
      syncEditedBoard();
    }
  } else if (state.tool === "mill") {
    if (!state.millDraft.includes(hit)) state.millDraft.push(hit);
    if (state.millDraft.length === 3) {
      toggleMill(board, state.millDraft);
      setStatus(`Toggled mill ${state.millDraft.join("-")}`);
      state.millDraft = [];
      syncEditedBoard();
    } else {
      render();
    }
  } else if (state.tool === "erase") {
    removePoint(board, hit);
    state.selectedPoint = null;
    setStatus(`Removed point ${hit}`);
    syncEditedBoard();
  }
}

function handleCanvasMove(event) {
  const board = selectedBoard();
  if (!board) return;
  const point = canvasToBoard(event);
  el.cursorInfo.textContent = `x ${point.x} y ${point.y}`;
  if (state.dragPoint === null) return;
  board.points[state.dragPoint] = point;
  syncRawFromState();
  render();
}

function handleCanvasLeave() {
  el.cursorInfo.textContent = "x -- y --";
  handleCanvasUp();
}

function handleCanvasUp() {
  if (state.dragPoint !== null) {
    setStatus(`Moved point ${state.dragPoint}`);
  }
  state.dragPoint = null;
}

el.boardSelect.addEventListener("change", () => selectBoard(el.boardSelect.value));
el.createBoard.addEventListener("click", createBoard);
el.duplicateBoard.addEventListener("click", duplicateBoard);
el.saveBoard.addEventListener("click", () => saveBoard().catch((error) => setStatus(error.message)));
el.rawJson.addEventListener("input", () => {
  try {
    syncStateFromRaw();
    render();
    setStatus("Raw JSON parsed");
  } catch (error) {
    setStatus(error.message);
  }
});
el.tabs.forEach((tab) => {
  tab.addEventListener("click", () => {
    state.tab = tab.dataset.tab;
    if (state.tab === "raw") syncRawFromState();
    render();
  });
});
el.spriteTools.forEach((tool) => {
  tool.addEventListener("click", () => {
    state.spriteTool = tool.dataset.spriteTool;
    el.spriteTools.forEach((button) => button.classList.toggle("active", button === tool));
  });
});
el.spriteAssetSelect.addEventListener("change", () => {
  const option = el.spriteAssetSelect.selectedOptions[0];
  state.spriteAssetPath = el.spriteAssetSelect.value;
  state.spriteAssetWidth = Number(option.dataset.width || 128);
  state.spriteAssetHeight = Number(option.dataset.height || 64);
  loadGlobalSprite();
});
el.invertSprite.addEventListener("click", invertSprite);
el.reloadSprite.addEventListener("click", loadGlobalSprite);
el.saveSprite.addEventListener("click", () => saveGlobalSprite().catch((error) => setStatus(error.message)));
el.audioSelect.addEventListener("change", () => selectAudio(el.audioSelect.value));
el.audioThemeSelect.addEventListener("change", () => selectAudioTheme(Number(el.audioThemeSelect.value)));
el.audioTickMs.addEventListener("input", updateAudioStats);
el.newEffect.addEventListener("click", createEffectDraft);
el.addRest.addEventListener("click", () => {
  state.audioEvents.push({ note: 0, duration: AUDIO_SNAP_UNITS });
  renderAudio();
});
el.playAudio.addEventListener("click", () => playAudio().catch((error) => setStatus(error.message)));
el.saveAudio.addEventListener("click", () => saveAudio().catch((error) => setStatus(error.message)));
el.tools.forEach((tool) => {
  tool.addEventListener("click", () => {
    state.tool = tool.dataset.tool;
    state.edgeStart = null;
    state.millDraft = [];
    el.tools.forEach((button) => button.classList.toggle("active", button === tool));
    render();
  });
});
["change", "input"].forEach((eventName) => {
  [el.showAdjacency, el.showMills, el.showGrid].forEach((input) => {
    input.addEventListener(eventName, render);
  });
});
el.canvas.addEventListener("mousedown", handleCanvasDown);
el.canvas.addEventListener("mousemove", handleCanvasMove);
el.canvas.addEventListener("mouseleave", handleCanvasLeave);
el.spriteCanvas.addEventListener("mousedown", (event) => {
  state.spriteDrawing = true;
  paintSpritePixel(event);
});
el.spriteCanvas.addEventListener("mousemove", (event) => {
  const point = spritePoint(event);
  el.spriteCursorInfo.textContent = `x ${point.x} y ${point.y}`;
  if (state.spriteDrawing) paintSpritePixel(event);
});
el.spriteCanvas.addEventListener("mouseleave", () => {
  el.spriteCursorInfo.textContent = "x -- y --";
  state.spriteDrawing = false;
});
window.addEventListener("mouseup", handleCanvasUp);
window.addEventListener("mouseup", () => {
  state.spriteDrawing = false;
  finishRollDrag();
});
window.addEventListener("pointermove", updateRollDrag);

loadBoards().catch((error) => setStatus(error.message));
loadGlobalSprite();
loadAudio().catch((error) => setStatus(error.message));
