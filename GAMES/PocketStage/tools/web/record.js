// Pocket Stage - recording the game in the emulator, for the post's clip,
// pictures and video. ardens.html loads this with record=1; tools/post_media.py
// serves the page, writes the scripts and keeps what comes back.
//
// rec.start() takes the emulator's clock over: from then on time moves only
// when rec.step() says so, 16 ms a step (one of the game's frames: Arduboy2's
// 60 frames a second are 16 ms each), however quickly or slowly the page runs.
// After each step the screen is read off the canvas (128 x 64, a bit a pixel),
// and the sound the emulator made is taken out of its audio buffer, which
// nothing else reads (the page is muted). So picture and sound are the game's
// own, frame for frame, and in step with each other.
//
// rec.run(name) fetches scripts/<name>.json ({frames, keys: [[frame, key, 1
// down or 0 up], ...]}), records that many frames pressing the keys on their
// frames, and posts rec/<name>.frames (1024 bytes a frame, rows of 16 bytes,
// the left pixel in the top bit), rec/<name>.pcm (16-bit mono) and
// rec/<name>.json. rec.status says how far it is.
(function () {
  'use strict';
  const STEP_MS = 16;
  const KEYS = { A: ['KeyA', 'a'], B: ['KeyS', 's'], UP: ['ArrowUp', 'ArrowUp'],
                 DOWN: ['ArrowDown', 'ArrowDown'], LEFT: ['ArrowLeft', 'ArrowLeft'],
                 RIGHT: ['ArrowRight', 'ArrowRight'] };
  const realNow = performance.now.bind(performance);

  // the emulator's sound: sokol_audio makes a ScriptProcessorNode with as many
  // outputs as it has channels; remember how many. The emulator makes no sound
  // while it hears that its audio is suspended, which on a muted page it always
  // is (ardens.html keeps it so, and never lets it reach the speakers): tell it
  // the audio runs.
  const AC = window.AudioContext || window.webkitAudioContext;
  const createSP = AC.prototype.createScriptProcessor;
  AC.prototype.createScriptProcessor = function (size, ins, outs) {
    window.recChannels = outs;
    Object.defineProperty(this, 'state', { get: () => 'running', configurable: true });
    return createSP.call(this, size, ins, outs);
  };

  const rec = window.rec = { manual: false, t: 0, status: 'idle', synced: false };
  performance.now = () => (rec.manual ? rec.t : realNow());

  // ---------------------------------------------------------------- the clock
  rec.step = function () {
    rec.t += STEP_MS;
    window.runFrames(rec.t);
  };

  // ---------------------------------------------------------------- the picture
  let gl = null;
  rec.grab = function () {
    gl = gl || Module.canvas.getContext('webgl2');
    const W = gl.drawingBufferWidth, H = gl.drawingBufferHeight;
    const cell = Math.min(W / 128, H / 64);
    const left = (W - 128 * cell) / 2, top = (H - 64 * cell) / 2;
    const y0 = Math.max(0, Math.floor(H - top - 64 * cell)), h = Math.min(H - y0, Math.ceil(64 * cell) + 1);
    const buf = new Uint8Array(W * h * 4);
    const bound = gl.getParameter(gl.READ_FRAMEBUFFER_BINDING);
    gl.bindFramebuffer(gl.READ_FRAMEBUFFER, null);
    gl.readPixels(0, y0, W, h, gl.RGBA, gl.UNSIGNED_BYTE, buf);
    gl.bindFramebuffer(gl.READ_FRAMEBUFFER, bound);
    const out = new Uint8Array(1024);
    for (let j = 0; j < 64; j++) {
      const r = (H - 1 - Math.floor(top + (j + 0.5) * cell)) - y0;
      for (let i = 0; i < 128; i++) {
        const x = Math.floor(left + (i + 0.5) * cell);
        if (buf[(r * W + x) * 4] > 127) out[j * 16 + (i >> 3)] |= 0x80 >> (i & 7);
      }
    }
    return out;
  };

  // ---------------------------------------------------------------- the sound
  // __saudio_emsc_pull(n) hands over one buffer of exactly n frames when the
  // emulator has made that much, else a buffer of silence (taking nothing).
  // So: empty the buffer while the title's music plays (buffers of music come
  // until it is nearly empty), then step until a buffer comes again: from that
  // moment less than a step's sound is left in it, and we know the fill level
  // from the sample rate as the steps go on.
  function pullBuffer() {
    const n = rec.bufferFrames, ch = window.recChannels || 1;
    const ptr = Module.__saudio_emsc_pull(n);
    if (!ptr) return null;
    const f = Module.HEAPF32.subarray(ptr >> 2, (ptr >> 2) + n * ch);
    const mono = new Float32Array(n);
    let any = false;
    for (let i = 0; i < n; i++) {
      mono[i] = f[i * ch];
      if (mono[i] !== 0) any = true;
    }
    return any ? mono : null;
  }

  rec.start = function () {
    if (rec.manual) return;
    rec.manual = true;
    rec.t = realNow();
    rec.rate = Module._saudio_context.sampleRate;
    rec.bufferFrames = Module._saudio_node.bufferSize;
    // the emulator runs each frame for the frame time sokol_app measures, which
    // it smooths over the last frames: let it settle on 16 ms first
    for (let i = 0; i < 120; i++) {
      rec.step();
      pullBuffer();
    }
    let drained = 0;
    while (drained < 200 && pullBuffer()) drained++;
    let steps = 0;
    while (steps < 400) {
      rec.step();
      steps++;
      if (pullBuffer()) break;
    }
    rec.fill = 0;                      // frames of sound waiting, by the clock
    rec.synced = steps < 400;
    rec.syncInfo = { drained, steps, rate: rec.rate, bufferFrames: rec.bufferFrames,
                     channels: window.recChannels };
    return rec.syncInfo;
  };

  function takeSound(pcm, final) {
    const n = rec.bufferFrames;
    rec.fill += rec.rate * STEP_MS / 1000;
    while (rec.fill >= n * (final ? 1 : 2)) {
      const ch = window.recChannels || 1;
      const ptr = Module.__saudio_emsc_pull(n);
      const f = Module.HEAPF32.subarray(ptr >> 2, (ptr >> 2) + n * ch);
      for (let i = 0; i < n; i++) pcm.push(f[i * ch]);
      rec.fill -= n;
    }
  }

  // ---------------------------------------------------------------- a script
  function key(name, down) {
    const [code, k] = KEYS[name];
    Module.canvas.dispatchEvent(new KeyboardEvent(down ? 'keydown' : 'keyup',
                                                  { code: code, key: k, bubbles: true }));
  }

  rec.run = async function (name) {
    rec.status = 'fetching ' + name;
    const script = await (await fetch('scripts/' + name + '.json?t=' + Date.now())).json();
    if (!rec.manual) rec.start();
    const total = script.frames;
    const frames = new Uint8Array(total * 1024);
    const pcm = [];
    const byFrame = new Map();
    for (const [f, k, d] of script.keys) {
      if (!byFrame.has(f)) byFrame.set(f, []);
      byFrame.get(f).push([k, d]);
    }
    for (let f = 0; f < total; f++) {
      for (const [k, d] of byFrame.get(f) || []) key(k, d);
      rec.step();
      frames.set(rec.grab(), f * 1024);
      takeSound(pcm, false);
      if (f % 50 === 49) {
        rec.status = name + ': frame ' + (f + 1) + ' of ' + total;
        await new Promise(r => setTimeout(r, 0));
      }
    }
    // the sound of the last steps is still in the buffer: run on a little,
    // and keep only the sound that belongs to the recorded frames
    const want = Math.round(total * STEP_MS / 1000 * rec.rate);
    while (pcm.length < want) {
      rec.step();
      takeSound(pcm, true);
    }
    pcm.length = want;
    const s16 = new Int16Array(want);
    for (let i = 0; i < want; i++) s16[i] = Math.max(-32768, Math.min(32767, Math.round(pcm[i] * 32767)));
    rec.status = name + ': sending';
    const post = (file, body, type) =>
      fetch('rec/' + file, { method: 'POST', headers: { 'Content-Type': type }, body: body });
    await post(name + '.frames', frames, 'application/octet-stream');
    await post(name + '.pcm', new Uint8Array(s16.buffer), 'application/octet-stream');
    await post(name + '.json', JSON.stringify({ frames: total, step_ms: STEP_MS, rate: rec.rate,
                                               samples: want, sync: rec.syncInfo }), 'application/json');
    rec.status = name + ': done, ' + total + ' frames, ' + want + ' samples';
    return rec.status;
  };
})();
