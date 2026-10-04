#!/usr/bin/env python3
"""
Pocket Stage - the clip, the pictures and the video for the post, recorded from
the game itself in the Ardens emulator.

  python3 tools/post_media.py serve [port]      the emulator page for recording
                                                (port 8644; launch config
                                                pocket-stage-record): open
                                                ardens.html?pkg=game/PocketStage.arduboy&mute=1&record=1
  python3 tools/post_media.py scripts           write the scenes' key scripts
                                                (build/post/scripts/<scene>.json)
  python3 tools/post_media.py sheet <scene> [s]  a picture of the recording, a frame
                                                every s seconds (1), with the times

Then, in the page, rec.run("<scene>") records a scene: every frame of the game
(16 ms) and its sound, frame for frame (tools/web/record.js), into
build/post/rec/<scene>.frames, .pcm and .json.

The scenes are below (SCENES): which keys to press on which frame, starting
from the title page of a fresh emulator (nothing saved: sound on, DEMO off).
"""
import array
import http.server
import json
import math
import os
import subprocess
import sys

from PIL import Image, ImageDraw

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
BUILD = os.path.join(ROOT, "build")
WEB = os.path.join(HERE, "web")
POST = os.path.join(BUILD, "post")
REC = os.path.join(POST, "rec")
SCRIPTS = os.path.join(POST, "scripts")
OUT = os.path.join(ROOT, "post")                 # what goes in the post (pictures, clip, video: not in git)

STEP_MS = 16                    # one of the game's frames
FPS = 1000.0 / STEP_MS          # 62.5


# ---------------------------------------------------------------- the scenes
class Script:
    """keys pressed on frames: wait() moves on, press() holds a key a while"""

    def __init__(self):
        self.frame = 0
        self.keys = []

    def wait(self, seconds):
        self.frame += round(seconds * FPS)
        return self

    def press(self, key, hold=0.1, then=0.4):
        self.keys.append([self.frame, key, 1])
        self.keys.append([self.frame + max(2, round(hold * FPS)), key, 0])
        return self.wait(hold + then)

    def times(self, key, n, then=0.4):
        for _ in range(n):
            self.press(key, then=then)
        return self

    def json(self, extra=0.0):
        return {"frames": self.frame + round(extra * FPS), "keys": self.keys}


def scene_test():
    return Script().wait(3.0).json()


def scene_sync():
    """the title page for 16 s: its music loops every 16 beats of 405 ms, so the
    sound must repeat exactly 6.48 s apart if no sound was lost or doubled"""
    return Script().wait(16.0).json()


def menus_to_demo(s):
    """from the title: DEMO on in the settings, back on the main menu at PLAY"""
    s.wait(4.0).press("B", then=1.4)                 # the main menu
    s.press("DOWN", then=0.5).press("B", then=1.2)   # the settings
    s.times("DOWN", 2, then=0.4).press("B", then=1.0)  # DEMO on
    s.press("A", then=1.0)                           # back: the main menu, on PLAY again
    return s


def scene_tour():
    """the title, the menus, the setlist's previews, Breakpoint on Expert played
    by the game, its results"""
    s = menus_to_demo(Script())
    s.press("B", then=1.0)                           # the difficulty (Hard)
    s.press("DOWN", then=0.6).press("B", then=2.6)   # Expert; the setlist, Candlelight's preview
    s.press("DOWN", then=2.2).press("DOWN", then=2.2)  # Stage Dive, Street Dog
    s.times("DOWN", 5, then=0.25)                    # on past Overdrive ... to Bored Again
    s.press("DOWN", then=2.6)                        # Breakpoint (the 9th)
    s.press("B", then=58.0)                          # the song, played by the game
    s.wait(5.0)                                      # the results
    return s.json()


def scene_booed():
    """a song nobody plays: the rock meter runs out, BOOED OFF"""
    s = Script().wait(1.0).press("B", then=1.0)      # the main menu
    s.press("B", then=0.8)                           # the difficulty (Hard)
    s.press("B", then=1.0)                           # the setlist
    s.times("DOWN", 12, then=0.2)                    # Deadline (the 13th)
    s.press("B", then=20.0)                          # the song, untouched; then the boos
    return s.json()


def scene_credits():
    """the settings, the credits rolling"""
    s = Script().wait(1.0).press("B", then=1.0)
    s.press("DOWN", then=0.5).press("B", then=1.0)   # the settings
    s.times("DOWN", 4, then=0.5).press("B", then=16.0)  # CREDITS
    return s.json()


SCENES = {"test": scene_test, "sync": scene_sync, "tour": scene_tour, "booed": scene_booed,
          "credits": scene_credits}


def cmd_scripts(args):
    os.makedirs(SCRIPTS, exist_ok=True)
    for name, make in SCENES.items():
        data = make()
        with open(os.path.join(SCRIPTS, name + ".json"), "w") as f:
            json.dump(data, f)
        print("%-8s %5d frames (%.1f s), %d key events" % (name, data["frames"], data["frames"] / FPS,
                                                        len(data["keys"])))


# ---------------------------------------------------------------- recordings
def load(scene):
    """a recording: (frames as a list of 1024-byte frames, the sound as 16-bit samples, its info)"""
    with open(os.path.join(REC, scene + ".frames"), "rb") as f:
        data = f.read()
    frames = [data[k:k + 1024] for k in range(0, len(data), 1024)]
    sound = array.array("h")
    with open(os.path.join(REC, scene + ".pcm"), "rb") as f:
        sound.frombytes(f.read())
    with open(os.path.join(REC, scene + ".json")) as f:
        info = json.load(f)
    return frames, sound, info


def frame_image(frame, zoom=1):
    img = Image.frombytes("1", (128, 64), frame).convert("L")
    return img.resize((128 * zoom, 64 * zoom), Image.NEAREST) if zoom > 1 else img


def at(seconds):
    return round(seconds * FPS)


def cmd_sheet(args):
    scene = args[0]
    every = float(args[1]) if len(args) > 1 else 1.0
    frames, sound, info = load(scene)
    picks = list(range(0, len(frames), max(1, at(every))))
    per_row = 8
    rows = (len(picks) + per_row - 1) // per_row
    sheet = Image.new("L", (per_row * 136, rows * 80), 40)
    draw = ImageDraw.Draw(sheet)
    for n, k in enumerate(picks):
        x, y = (n % per_row) * 136 + 4, (n // per_row) * 80 + 12
        sheet.paste(frame_image(frames[k]), (x, y))
        draw.text((x, y - 11), "%.1f s" % (k / FPS), fill=200)
    out = os.path.join(POST, scene + "-sheet.png")
    sheet.save(out)
    print("wrote %s: %d frames (%.1f s), %d shown" % (out, len(frames), len(frames) / FPS, len(picks)))


# ---------------------------------------------------------------- what goes in the post
ZOOM = 5                        # the pictures and the clip: 640 x 320, as Small Theft Auto's

# the pictures: name -> (scene, second)
PICTURES = {
    "title": ("tour", 2.0),
    "menu": ("tour", 5.0),
    "setlist": ("tour", 22.8),
    "stage": ("tour", 30.8),
    "guitarist": ("tour", 34.3),
    "bassist": ("tour", 48.5),
    "drummer": ("tour", 53.5),
    "beams": ("tour", 58.0),
    "results": ("tour", 84.5),
    "booed": ("booed", 19.8),
    "credits": ("credits", 8.9),
}

# the clip: a short tour without sound, (scene, from, to) one after another.
# The tour's keys: B on the title at 4.0 s, PLAY at 10.6, Expert at 11.7, the
# setlist at 12.4, Breakpoint picked at 21.5 and started at 24.2. The game's
# cameras during the song, a cut every four bars: 24.26 the whole stage, 31.10
# the guitarist, 37.58 the stage again, 44.08 the bassist (star power from
# 45.5), 50.56 the drummer, 57.06 the stage, 63.54 from below, 70.02 and
# 76.51 more of the stage; the results from 82.9. Joel: the whole stage
# first, then two or three close-ups.
CLIP = [
    ("tour", 0.4, 2.8),                  # the title
    ("tour", 4.05, 4.95),                # the main menu
    ("tour", 10.65, 12.4),               # the difficulty: Expert
    ("tour", 21.5, 28.2),                # Breakpoint picked; B: the whole stage, the first notes
    ("tour", 33.0, 35.6),                # the guitarist
    ("tour", 46.0, 48.6),                # the bassist, star power
    ("tour", 52.5, 55.1),                # the drummer
    ("tour", 83.4, 85.3),                # the results
]

# the video: (scene, from, to, sound) one after another. The sound is music
# rendered from the game's own streams and lined up with the recording
# ("riff" the menus' music, ("preview", n) the setlist's loop of song n,
# ("song", n) song n), or the emulator's own sound ("emulator"). Every cut
# starts where the game started what plays in it (a frame after the key), or
# on a bar line of the song (Breakpoint's first beat sounds at 26.231 s, a bar
# is 4 x 60/148 s), or one loop of the menus' music (6.4865 s) after the last
# cut ended, so the music never stumbles.
SONG = 8                                 # Breakpoint
SONG_START, BAR = 26.231, 4 * 60 / 148.0
CUTS = [
    ("tour", 0.0, 4.6, "riff"),                                  # the title; B: the main menu
    ("tour", 4.6 + 6.4865, 12.432, "riff"),                      # the difficulty: Expert
    ("tour", 12.432, 15.136, ("preview", 0)),                    # the setlist: Candlelight
    ("tour", 15.136, 17.44, ("preview", 1)),                     # Stage Dive
    ("tour", 17.44, 19.744, ("preview", 2)),                     # Street Dog
    ("tour", 21.504, 24.208, ("preview", SONG)),                 # Breakpoint (the quick scroll to it left out)
    ("tour", 24.208, SONG_START + 22 * BAR, ("song", SONG)),     # Breakpoint on Expert, to the end of bar 22
    ("tour", SONG_START + 33 * BAR, 87.3, ("song", SONG)),       # its last bar; the results
    ("credits", 6.8, 13.0, "riff"),                              # the credits
]
START_FADE, END_FADE = 0.3, 1.0  # seconds the sound takes to come up at the start, to fade out at the end


def cmd_pictures(args):
    """PICTURES: name -> (scene, second)"""
    os.makedirs(OUT, exist_ok=True)
    cache = {}
    for name, (scene, second) in PICTURES.items():
        if scene not in cache:
            cache[scene] = load(scene)[0]
        frame_image(cache[scene][at(second)], ZOOM).save(os.path.join(OUT, "picture-%s.png" % name))
        print("picture-%s.png  %s at %.2f s" % (name, scene, second))


def cmd_clip(args):
    """CLIP's cuts one after another, every second frame (31.25 a second; GIF
    counts in hundredths, so 3, 3, 3, 4, 3 hundredths in turn: 32 ms on average)"""
    cache, frames = {}, []
    for scene, first, last in CLIP:
        if scene not in cache:
            cache[scene] = load(scene)[0]
        frames += cache[scene][at(first):at(last)]
    pictures = [frame_image(f, ZOOM).convert("P") for f in frames[::2]]
    pattern = [30, 30, 30, 40, 30]
    durations = [pattern[k % 5] for k in range(len(pictures))]
    out = os.path.join(OUT, "clip.gif")
    pictures[0].save(out, save_all=True, append_images=pictures[1:], duration=durations, loop=0,
                     optimize=False, disposal=1)
    print("wrote %s: %d cuts, %.1f s, %d pictures, %.1f MB" % (out, len(CLIP), len(frames) / FPS, len(pictures),
                                                              os.path.getsize(out) / 1048576))


# ---------------------------------------------------------------- the sound
# The music in the video is the songs page's (tools/web/songs.html): the
# streams the game plays (build/game/fxdata.bin), each pin's level averaged
# over a sample, the speaker hearing the difference of the two pins, and the
# piezo's high-pass. The emulator's own sound has the same notes at the same
# times, but it reads the pins 50,000 times a second and keeps their raw
# difference: its edges jitter, and the offset the piezo never hears jumps at
# every note. So it is used to line the music up (by matching the two) and
# heard only where there is no music to render (the boos), with the offset
# taken out and the edges softened.
RATE = 50000                             # the emulator's rate, and what tools/video.swift takes
LEVEL = 0.55                             # of full scale, for the louder parts
CMD_NOP, CMD_END = 0xFF, 0xFE
DRUM_KICK = 0x7C


class Pin:
    """a pin a timer toggles: a period in counts of 2 MHz, or 0 for quiet"""

    def __init__(self):
        self.period = 0
        self.level = 0
        self.acc = 0.0
        self.drum_left = 0
        self.kick = False
        self.dice = 0xBEEF
        self.note = 0

    def play(self, midi):
        self.period = round(1e6 / (440.0 * 2 ** ((midi - 69) / 12.0))) if midi else 0
        if not midi:
            self.level = 0
        self.acc = 0.0

    def drum(self, kind):
        self.kick = kind == DRUM_KICK
        self.period = 300 if self.kick else 150
        self.drum_left = 44 if self.kick else 220
        self.acc = 0.0

    def sample(self, counts):
        """the pin's level averaged over the next sample"""
        if not self.period:
            return 0.0
        left, total = counts, 0.0
        while self.period and self.acc + left >= self.period:
            step = self.period - self.acc
            total += self.level * step
            left -= step
            self.acc = 0.0
            self.level ^= 1
            if self.drum_left:
                self.drum_left -= 1
                if self.drum_left:
                    if self.kick:
                        self.period += self.period >> 4
                    else:
                        self.dice = ((self.dice >> 1) ^ (-(self.dice & 1) & 0xB400)) & 0xFFFF
                        self.period = 100 + (self.dice & 0x1FF)
                else:
                    self.play(self.note)
        if self.period:
            self.acc += left
            total += self.level * left
        return total / counts


def streams():
    """the menus' music and every song's music and preview, as the game has them"""
    with open(os.path.join(BUILD, "game", "fxdata.bin"), "rb") as f:
        blob = f.read()

    def take(at):
        end = at
        while blob[end + 1] != CMD_END:
            end += 2
        return blob[at:end + 2]
    out = {"riff": take(int.from_bytes(blob[4:7], "little"))}
    for i in range(blob[3]):
        e = blob[0x10 + 64 * i:0x10 + 64 * (i + 1)]
        out[("song", i)] = take(int.from_bytes(e[0:3], "little"))
        out[("preview", i)] = take(int.from_bytes(e[56:59], "little"))
    return out


def render(stream, seconds, loop):
    """a music stream from its start: the two pins' difference, a sample at a time"""
    ev, ms, p = [], 0, 0
    while True:
        ms += stream[p]
        cmd = stream[p + 1]
        p += 2
        if cmd == CMD_END:
            break
        if cmd != CMD_NOP:
            ev.append((ms & ~1, cmd))          # the clock plays an event on the even millisecond
    length = ms
    n = int(seconds * RATE)
    out = array.array("f", bytes(4 * n))
    pins = [Pin(), Pin()]
    counts = 2e6 / RATE
    e, base = 0, 0.0
    for i in range(n):
        t = i * 1000.0 / RATE - base
        if loop and e >= len(ev) and t >= length:
            base += length
            t -= length
            e = 0
        while e < len(ev) and ev[e][0] <= t:
            cmd = ev[e][1]
            e += 1
            v, note = cmd >> 7, cmd & 0x7F
            if v == 1 and note >= DRUM_KICK:
                pins[1].drum(note)
            else:
                pins[v].note = note
                if not pins[v].drum_left:
                    pins[v].play(note)
        out[i] = pins[0].sample(counts) - pins[1].sample(counts)
    return out, length / 1000.0


def piezo(x):
    """what the piezo hears: changes only (the songs page's high-pass)"""
    out = array.array("f", bytes(4 * len(x)))
    px = py = 0.0
    for i, v in enumerate(x):
        py = v - px + 0.995 * py
        px = v
        out[i] = 0.5 * py
    return out


def soften(x, cutoff=6000.0):
    """a gentle low-pass (two poles) for the emulator's raw edges"""
    w = 2 * math.pi * cutoff / RATE
    alpha = math.sin(w) / (2 * 0.7071)
    b0 = (1 - math.cos(w)) / 2
    b1 = 1 - math.cos(w)
    a0, a1, a2 = 1 + alpha, -2 * math.cos(w), 1 - alpha
    b0, b1, b2, a1, a2 = b0 / a0, b1 / a0, b0 / a0, a1 / a0, a2 / a0
    out = array.array("f", bytes(4 * len(x)))
    x1 = x2 = y1 = y2 = 0.0
    for i, v in enumerate(x):
        y = b0 * v + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2
        x2, x1, y2, y1 = x1, v, y1, y
        out[i] = y
    return out


def decimate(x, k):
    return [sum(x[i:i + k]) / k for i in range(0, len(x) - k + 1, k)]


def best_shift(emu, model, w0, w1, shifts, step=1):
    """the shift d (in samples of these arrays) for which model[k - d] matches
    emu[k] best over w0 <= k < w1"""
    best, best_d = -2.0, None
    seg = emu[w0:w1:step]
    norm = math.sqrt(sum(v * v for v in seg)) or 1.0
    for d in shifts:
        if w0 - d < 0 or w1 - d > len(model):
            continue
        m = model[w0 - d:w1 - d:step]
        c = sum(a * b for a, b in zip(seg, m)) / (norm * (math.sqrt(sum(v * v for v in m)) or 1.0))
        if c > best:
            best, best_d = c, d
    return best_d, best


def line_up(emu, model, loop_s, first, last):
    """where (in recording seconds) the model's start must lie for its music to
    match the emulator's in [first, last): near first for a stream the game
    started there (a frame or so after a key), anywhere within a loop for the
    menus' music, which has been going round since the game started"""
    pad = RATE                                  # a second of quiet before the model's start, for a
    padded = array.array("f", bytes(4 * pad)) + model   # start that comes after the cut's first sample
    w0, w1 = int(first * RATE), int(min(last, first + 6.0) * RATE)
    if loop_s:
        guesses = range(int((first - loop_s) * RATE), int(first * RATE) + 1, 100)
    else:
        guesses = range(int((first - 0.06) * RATE), int((first + 0.16) * RATE) + 1, 100)
    k = 100                                     # coarse: 2 ms steps on 500 a second
    E, M = decimate(emu, k), decimate(padded, k)
    d, c = best_shift(E, M, w0 // k, w1 // k, [(g - pad) // k for g in guesses])
    k2 = 25                                     # finer: 0.5 ms steps on 2,000 a second
    E2, M2 = decimate(emu, k2), decimate(padded, k2)
    d2, c2 = best_shift(E2, M2, w0 // k2, w1 // k2, range(d * 4 - 8, d * 4 + 9))
    d3, c3 = best_shift(emu, padded, w0, w1, range(d2 * k2 - 30, d2 * k2 + 31), step=3)
    return (d3 + pad) / RATE, c


def cmd_video(args):
    """CUTS one after another, the sound with them (a short fade at every cut),
    made into tour.mp4 by tools/video.swift"""
    music = streams()
    rendered, placed, recordings = {}, {}, {}
    frames, sound = [], array.array("f")
    fade = int(0.006 * RATE)
    left_off = None                             # the last cut's scene, and where its menus' music was
    for scene, first, last, source in CUTS:
        if scene not in recordings:
            f, s, info = load(scene)
            assert info["rate"] == RATE and info["step_ms"] == STEP_MS
            recordings[scene] = (f, piezo([v / 3535.0 for v in s]))
        f, emu = recordings[scene]
        if source == "emulator":
            how = "the emulator's own"
        else:
            if source not in rendered:
                loop = source == "riff" or source[0] == "preview"
                x, loop_s = render(music[source], 60.0 if source != "riff" else 14.0, loop)
                rendered[source] = (piezo(x), loop_s if loop else 0.0)
            model, loop_s = rendered[source]
            # the menus' music has been going round since the game started:
            # look for it anywhere in its loop, at every cut. A preview or a
            # song starts when it is picked, near the start of its cut, and a
            # song goes on from where it was found
            anywhere = loop_s if source == "riff" else 0.0
            key = (scene, source)
            if key not in placed or anywhere:
                placed[key] = line_up(emu, model, anywhere, first, last)
            begin, match = placed[key]
            if anywhere and left_off and left_off[0] == scene:
                # the menus' music again after a cut: start where it is at the
                # point the last cut left it (the game's music can lose a moment
                # while the chip is written, so a whole loop later is not enough)
                t = begin + left_off[1]
                while t < first - 1e-6:
                    t += loop_s
                assert t < last, "no place in this cut where the music goes on"
                first = round(t * FPS) / FPS
            how = "%s from %.3f s, matching %.2f" % (source if isinstance(source, str) else "%s %d" % source,
                                                     begin, match)
        a, b = at(first), at(last)
        frames += f[a:b]
        n = (b - a) * RATE * STEP_MS // 1000
        start = a * RATE * STEP_MS // 1000
        if source == "emulator":
            piece = soften(emu[start:start + n])
            left_off = None
        else:
            off = start - int(round(begin * RATE))
            if anywhere:
                off %= int(round(loop_s * RATE))
                left_off = (scene, ((start + n) / RATE - begin) % loop_s)
            else:
                left_off = None
            piece = model[off:off + n] if off >= 0 else array.array("f", bytes(4 * -off)) + model[0:off + n]
        piece = array.array("f", piece) + array.array("f", bytes(4 * (n - len(piece))))
        for i in range(min(fade, n)):
            piece[i] *= i / fade
            piece[n - 1 - i] *= i / fade
        sound += piece
        print("  %-8s %6.3f to %6.3f s  %s" % (scene, first, last, how))
    head, tail = int(START_FADE * RATE), int(END_FADE * RATE)
    for i in range(head):
        sound[i] *= i / head
    for i in range(tail):
        sound[len(sound) - tail + i] *= 1 - i / tail
    peak = max(abs(v) for v in sound) or 1.0
    pcm = array.array("h", (int(round(v / peak * LEVEL * 32767)) for v in sound))
    print("%d cuts: %.1f s" % (len(CUTS), len(frames) * STEP_MS / 1000.0))
    os.makedirs(OUT, exist_ok=True)
    tmp_frames = os.path.join(POST, "tour.frames")
    tmp_sound = os.path.join(POST, "tour.pcm")
    with open(tmp_frames, "wb") as f:
        f.write(b"".join(frames))
    with open(tmp_sound, "wb") as f:
        f.write(pcm.tobytes())
    tool = os.path.join(POST, "video")
    src = os.path.join(HERE, "video.swift")
    if not os.path.exists(tool) or os.path.getmtime(tool) < os.path.getmtime(src):
        subprocess.run(["swiftc", "-O", "-o", tool, src], check=True)
    subprocess.run([tool, tmp_frames, tmp_sound, os.path.join(OUT, "tour.mp4")] + args, check=True)


# ---------------------------------------------------------------- the page
class Handler(http.server.SimpleHTTPRequestHandler):
    """build/ as the site, ardens.html and record.js from tools/web, scripts
    from build/post/scripts; a POST to rec/<file> is written to build/post/rec"""

    def __init__(self, *args, **kw):
        super().__init__(*args, directory=BUILD, **kw)

    def translate_path(self, path):
        name = path.split("?", 1)[0].lstrip("/")
        if name in ("ardens.html", "record.js"):
            return os.path.join(WEB, name)
        if name.startswith("scripts/"):
            return os.path.join(POST, name)
        return super().translate_path(path)

    def end_headers(self):
        self.send_header("Cache-Control", "no-store")
        super().end_headers()

    def do_POST(self):
        name = self.path.split("?", 1)[0].lstrip("/")
        if not name.startswith("rec/") or "/" in name[4:] or name[4:].startswith("."):
            self.send_error(404)
            return
        size = int(self.headers.get("Content-Length", 0))
        body = self.rfile.read(size)
        os.makedirs(REC, exist_ok=True)
        with open(os.path.join(REC, name[4:]), "wb") as f:
            f.write(body)
        self.send_response(200)
        self.end_headers()
        self.wfile.write(b"ok")

    def log_message(self, fmt, *args):
        if self.command == "POST":
            sys.stderr.write("wrote %s\n" % self.path)


def cmd_serve(args):
    port = int(args[0]) if args else 8644
    print("serving %s on http://localhost:%d/ardens.html?pkg=game/PocketStage.arduboy&mute=1&record=1"
          % (BUILD, port), flush=True)
    http.server.ThreadingHTTPServer(("127.0.0.1", port), Handler).serve_forever()


def main():
    commands = {"serve": cmd_serve, "scripts": cmd_scripts, "sheet": cmd_sheet, "pictures": cmd_pictures,
                "clip": cmd_clip, "video": cmd_video}
    if len(sys.argv) < 2 or sys.argv[1] not in commands:
        sys.exit(__doc__)
    commands[sys.argv[1]](sys.argv[2:])


if __name__ == "__main__":
    main()
