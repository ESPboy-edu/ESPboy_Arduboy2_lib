# Pocket Stage

> These are the working notes of the project, as they were written along
> the way, for the author's own console and computer. Some things they speak
> of are not in this repository: the tool that puts the game into the
> console's list of games (`../CartManager`) and `tools/console.py`, which
> talks to the console with it, the page the songs were picked on
> (`tools/songs_page.py`), the notes in `notes/`, and the text in `post/`.
> Any tool for the Arduboy FX that takes an `.arduboy` file will put the game
> on a console.

A Guitar Hero-like game for the Arduboy FX, after the Java phone versions of
2007 to 2009: three lanes instead of five, the band behind the note highway,
the songs short and without singing. It was Pocket Hero until 2026-09-30.

**Status: title, menus, fifteen rock tracks with drums, scores kept, star
power, a pause menu, the stage as a photograph of the concert, the rock
meter.** A title page with the logo over the stage, spotlights swinging,
the crowd jumping to a riff that loops; menus after Guitar Hero's (PLAY /
SETTINGS, the difficulty, the setlist with every song's stars and best
score, the picked song's preview playing); a highway in perspective, three lanes, notes coming down to the
beat lines, holds, star notes and star power, score, streak and
multiplier; behind the highway the concert as a photograph from seven
cameras that cut every four bars, the band and the crowd moving to the
music; a rock meter that boos you off the stage when it runs out; a
results page with stars. Miss a note and the tune drops out until the next hit. Best
scores and settings are kept on the flash chip. What was found out before
starting is in `notes/`.

**The FX and the FX-C, one build** (Joel, 2026-10-02: "make sure the game is
also compatible with the FX-C"). The flash chip's select line is PD1 (SDA)
on the Arduboy FX and PE2 (HWB) on the FX-C and the Mini, which keep SDA and
SCL for the link cable; otherwise the FX-C is the same to a game (same
display, same flash size, the homemade board package's FX-C board differs
from its FX board in this one setting). Games built with the ArduboyFX
library need a build for each, and the community's cart builder keeps a
list for each. Pocket Stage looks for its data with PD1 and then with PE2
at start-up and from then on drives only the line it found it on
(`PocketStage/flash.h`; PE2 is never driven high while it looks, so the
FX's HWB pin takes no load); the status line says which (`cs=PD1`). Tried
on Joel's FX and in Ardens with both wirings (a package marked
`ArduboyMini` puts Ardens' chip on PE2): the title, the setlist, a song
played to the end, its best score saved and read back; the build from
before shows a blank light there. There is no FX-C here to try it on.

## Songs

The console's speaker gets two square waves, a tune and a bass, and the
drums on the bass's pin. Rock is what the game is for, and no rock song is
old enough to be free, so the tracks are our own, written for the game, the
band names puns on real bands. The setlist is the fifteen Joel picked on
2026-10-02, easiest first:

| Song | What |
|---|---|
| Candlelight (Ghosting) | Occult rock, E minor at 118 |
| Stage Dive (Piezo Priest) | Heavy metal, A minor at 120 |
| Street Dog (Rolling Tones) | Classic rock, G major at 124 |
| Overdrive (The Square Waves) | Hard rock, E minor at 144 |
| Undefined (Linker Park) | Alt metal, E minor at 104 |
| Fix Me Later (TODO) | AOR shuffle, C# minor / E major at 104 |
| Signal Fire (I2C) | Arena rock, D major at 124 |
| Bored Again (Screen Day) | Pop punk, E major at 176 |
| Breakpoint (Killswitch) | Metalcore, C sharp minor at 148 |
| Duckwalk (Chip Berry) | Rock and roll, B flat at 168 |
| Hot Plate (Red Hot Chippers) | Funk rock, E at 104 |
| Night Driver (Deep Pixel) | Organ hard rock, G minor at 168 |
| Deadline (Metal Oxide) | Thrash metal, E minor at 196 |
| Warhorse (Tin Maiden) | Heavy metal gallop, E minor at 164 |
| Hot Fingers (Van Halogen) | Shred hard rock, E at 136 |

Each is about fifty-five seconds. `songs/title_riff.py` is the four bars
the title page and the menus loop: Breakpoint's verse riff with the band
(Joel's pick on 2026-10-02; Overdrive's riff before). In the setlist the
picked song's preview plays instead, eight bars from its chorus (two songs
open on their hook), round and round until another song is picked; each
song names its first bar in PREVIEW. (The first three songs were
public-domain classical pieces; Joel wanted rock, they went on 2026-09-30.)

**How the fifteen were picked.** Fifty songs are in `songs/`, each in a rock
style of its own; `songs/candidates.py` numbers them from the easiest to the
hardest and marks fifteen suggested beforehand. `python3
tools/songs_page.py` makes the page they were picked on,
`build/look/songs.html`: every song played through on the play page the
way the game draws it, on any level, with the sound the console makes.
`songs/setlist.py` holds the fifteen.
`songs/kit.py` holds the bass lines and drum beats the songs share, and
`python3 tools/song_check.py name` looks a song over: what would stop the
build, its length, how dense the notes come on each level, and whether the
title fits the setlist.

## Controls

| Button | What |
|---|---|
| Left | The left lane |
| Up or Down | The middle lane (either, so both thumb positions work) |
| Right | The right lane |
| A | In play: star power, once the meter is half full |
| B | In play: the pause menu (RESUME, RESTART, QUIT) |
| Up, Down | In the menus: move the pick |
| B | In the menus: take what the pick is on; on the title page: on to the menu; in the credits, held: the roll goes three times as fast |
| A | In the menus: back |
| Left, Right | On the setlist: the difficulty, on the spot |
| Up + Down held 2 seconds | Outside play: back to the console's game menu |

Settings: SOUND (the console's own setting, for every game), LEFTY FLIP
(Right is the left lane, Left the right), DEMO (the game plays the songs by
itself; the results show how it went, but nothing is saved as a best),
CLEAR SCORES (asks twice), CREDITS (the list scrolls by one to
reach it): the credits roll up from behind the jumping crowd, the logo, the
version (`PocketStage/version.h`, 1.0 since 2026-10-02), who made the game,
the fifteen songs with their bands, round and round; B held rolls them
three times as fast, A goes back (Joel, 2026-10-02).
Joel picked the roll and its words from samples 8 (`build/look/v8/`).

A note counts when its lane's button goes down within 90 ms of its time;
fire bursts out of the middle of the pad. A chord needs all its lanes down
within that window. A note with a tail is a hold (the tail two pixels
wide since 2026-10-02: one was hard to see): keep the button down
until the tail has run out, the fire burning all the while. No note ever comes while
another is still held (as on a real guitar, where you would have to strum
again). Every 10 notes in a row raise the multiplier, up to 4. A
missed note gives a clunk, and the tune drops out until the next hit, the
way Guitar Hero does it; the bass and the drums play on. Ring-shaped notes
are star notes (two bars in every eight): each one hit fills the star
meter down the right edge; from half full, A lets it go, and the score
counts double until it has run out (a full meter lasts 16 seconds). Stars
come from the score against the perfect score of the song: 5 from 75 % of
it, 4 from 55, 3 from 30, 2 from 15.

The rock meter is the bar up the left edge, under a heart (Joel's pick;
it was a figure with its arms up), the mirror of the star meter on the right: the crowd's mood, half way at the
start, up a point for every note hit (two with star power on), down for
every miss, one point on Easy, two on Medium, three on Hard, four on
Expert. The bottom third is the red zone, its frame dotted, and the bar
blinks in it; at the bottom the song stops and the crowd boos you off the
stage: BOOED OFF in big letters, how far you got, and the title page's
crowd booing, shaking fists and throwing cups at the stage, while the
spotlights go out one after the other; no score is kept. It was a dial in
the bottom left corner until the crowd took that corner (2026-10-01).

The stage is a photograph of the concert in the screen's two colours
(the look Joel picked on 2026-10-01: build/look/v4.html, blue noise): the
light behind the band, the band, the kit, the amps and the crowd black
against it. Seven cameras, cutting every four bars: the stage from the
crowd (with star power on, the same with a V of beams), the guitarist
from the front row, the bassist headbanging, the kit close, the stage
from behind the drummer, and the guitarist from the pit, looking up. The
band moves to the music: on every beat the players shift their weight,
the bassist's head goes down and up, the drummer's sticks come down; the
guitarist strums every note you hit; the crowd pumps its fists on the
beat as long as the rock meter is above the middle, and keeps them low
below it; with star power on the guitarist jumps on the beat and the
lights swing twice as fast. The highway shows a second and a half
ahead.

Songs are cut to about a minute, rather shorter (Joel, 2026-09-30).

The lanes were Left, Right and B at first; Joel found the three D-pad
buttons much better to play (2026-09-30).

## Layout

| Path | What |
|---|---|
| `PocketStage/` | The program: `PocketStage.ino` (USB, the frame), `title.h` (the title page), `menus.h` (the menus, the results, BOOED OFF), `ui.h` (the look: font, bars, items, stars), `save.h` (settings and best scores on the chip), `play.h` (notes, judgement, the highway, the fire on the pads, the rock meter), `stage.h` (pictures and sprites off the chip, the fast drawing, the stage's frames and the cuts), `audio.h` (two voices, drums, the song clock, the buttons), `flash.h` and `fxvector.S` (the flash chip), `fxdata.h`, `tables.h`, `logo.h` and `font.h` (generated) |
| `songs/` | The songs as text, one file each; `setlist.py` says which and in what order; `title_riff.py` the menus' loop |
| `songs/kit.py`, `songs/candidates.py` | The bass lines and drum beats the songs share; the fifty songs to pick the setlist from |
| `tools/song_check.py` | Look a song over before it goes on the chip |
| `tools/songs_page.py`, `tools/web/songs.html` | The page to pick the setlist from (`build/look/songs.html`) |
| `tools/build_data.py` | Songs to flash data: music stream with drums, four charts with their perfect scores, the stage's frames, the fire, the booed-off page's title, the crowd and the spotlights of the title page, the menu and the booed-off page, the tables. Its docstring is the file format and the notation |
| `tools/scene.py` | The stage: every camera's frames, rendered and dithered; `python3 tools/scene.py` writes `build/scene_<camera>.png` to look at them (`-f` renders afresh; otherwise they come from `build/scene_cache.json` until the tool changes) |
| `tools/bluenoise.py` | The blue-noise threshold matrix the stage is dithered with |
| `tools/crowd.py` | The crowd and the spotlights of the title page, the main menu and the booed-off page, in layers; `python3 tools/crowd.py` writes `build/crowd.png` (the frames come from `build/crowd_cache.json` until the tool changes) |
| `tools/flames.py` | The fire on the pads: each pad's frames for a hit and for a hold, as sprites; `python3 tools/flames.py` writes `build/flames.png` (the frames come from `build/flames_cache.json` until the tool changes) |
| `tools/stage.py`, `tools/render.py` | The band of before (sprites modelled and lit), no longer in the game; kept for the close-up band (`python3 tools/stage.py closeup`) |
| `tools/logo.py` | The logo (Impact, stretched, a thickness behind it) as a sprite in `logo.h`, the picture the console's menu shows (`assets/title.png`), and the booed-off page's title in the same style |
| `tools/font.py` | The menu font from Impact, eleven pixels tall, and the pick and the stars, in `font.h` |
| `tools/build.sh` | Build everything into `build/game` (or `build/$OUT`) |
| `tools/console.py` | Put a build on the console over USB (`load`), talk to it (`watch`) |
| `tools/preview_wav.py` | Hear a song without the console: what the timers would do, as a WAV file in `build/game` |
| `tools/web/ardens.html` | The emulator page; served by `../SmallTheftAuto/tools/serve.py` on port 8643 (launch config `pocket-stage-files`) |
| `tools/post_media.py`, `tools/web/record.js`, `tools/video.swift` | The clip, the pictures and the video for the post, recorded from the game in the emulator a frame at a time with its sound (launch config `pocket-stage-record`, port 8644); `post/README.md` says how |
| `notes/` | What the Java phone games were, and why their songs cannot be used |

The compiler is the one in `../SmallTheftAuto/.toolchain`; `.toolchain` here
is a link to it. The screen and the buttons are the Arduboy2 library's (in
that toolchain); the flash chip, the sound and the clock are our own.

## Build and run

```bash
tools/build.sh
```

Emulator: start the `pocket-stage-files` server and open
`http://localhost:8643/ardens.html?pkg=game/PocketStage.arduboy` (add
`&mute=1` to keep it silent).

Console, with the console on its game menu (a running Small Theft Auto has
no USB: hold Up and Down for two seconds):

```bash
python3 tools/console.py load game
```

The build keeps the USB connection, so it can be asked how it is doing:
`python3 tools/console.py watch 10 1:s` prints a line of status (the pages,
the score, the frame rate, how much of the stack was never touched); `b`
presses B for it (`u`, `d`, `l`, `r` Up, Down, Left and Right in the menus), `a` switches autoplay, `x` sends it back to the menu.
`OUT=dev EXTRA_FLAGS=-DPROFILE tools/build.sh` makes a build whose status
line adds the longest each part of the play page's frame took, in
microseconds: (nothing), the stage, the highway, the notes, the rest, the
whole frame. With `-DSTAGE_TEST` as well the build goes straight into a
song on Medium and plays it by itself (for looking at the stage).

That puts the build in the console's processor only: picking another game
in the console's menu replaces it. Pocket Stage is in the console's list of
games since 2026-10-01 (under "My games", after Small Theft Auto; put there
with `python3 ../CartManager/cartmanager.py add "Pocket Stage"`). To put a
new build in the list, in the place of the old one, its saves kept:

```bash
python3 ../CartManager/cartmanager.py update
```

then `python3 tools/console.py load game`, which finds the data in the list
and starts the same program the menu would.

## How it works

- **Sound.** The tune is a square wave timer 3 makes on one speaker pin by
  itself; the bass is timer 1 toggling the other pin from its interrupt. A
  drum takes the bass's pin for a few dozen milliseconds: a kick is a pitch
  sinking from 3 kHz to 200 Hz, a snare a burst of noise; then the bass
  comes back by itself. Timer 4 ticks a thousand times a second: it is the
  song's clock, plays the music events that are due and stamps every button
  press with its millisecond, so the judgement does not depend on the frame
  rate.
- **Data.** Everything comes off the flash chip in small streams: 2-byte
  music records (wait, then a note on or off, or a drum), 2 to 3-byte chart
  records (wait, lanes and flags, hold length). The main loop keeps a ring
  of music events and a queue of the notes in sight filled; nothing reads
  the chip from an interrupt. The title riff is a music stream that starts
  again at its end.
- **Saving.** The loader gives a game 4 KB of the chip to write to. The
  chip can only clear bits and wipes 4 KB at a time, so the place is a log:
  a key, then records appended one by one (a settings byte, or a level, a
  song's id and a score); only a better score is written, and when the log
  is full it is wiped and written afresh with what counts. The way Small
  Theft Auto does it. A song's id is made from its name and its charts, so
  when a chart changes the song starts again without a best (a best is for
  the chart it was played on; the old records are left behind and dropped
  when the log is next written afresh). The sound setting is the console's own (the Arduboy2
  library's EEPROM byte), so it holds for every game.
- **Perspective.** A note's row and scale come from a table by the time it
  still has to go (a second and a half is in sight); lanes converge on the
  far end at row 22, the edge of the drum riser, the hit line is row 56.
  (Two seconds to row 12 until the stage was built round the highway;
  the far rows held the last second and a quarter as a crawl of dots.)
- **Charts** are made by `tools/build_data.py` from the tune: Expert is
  every note; Easy keeps a note only when it is far enough from the notes
  already kept (700 ms), the notes on a beat first, so the beats stay and
  the notes between them go as a song gets faster. Medium and Hard lie at
  even steps between Easy and Expert (each about the same number of times
  more notes than the level below; at least 1.15), so every level adds
  notes to the one below whatever the tempo. Until 2026-10-02 they kept
  notes 380 and 200 ms apart, and in songs faster than about 150 beats a
  minute the eighths fell under both, so on 7 of the 15 songs two levels
  were the same. They add the notes on the strongest places first (bar,
  half bar, beat, eighths and triplets, sixteenths; the ones that start or
  end a run before the ones in the middle of it, spread through the song),
  take a whole place when that lands within 10 % of the step, and keep
  their notes 200 and 100 ms apart where they can. Lanes follow the shape of the tune, long notes on the first beat
  of a bar become chords, long notes become holds: the tail ends a little
  before the note does and 200 ms before the next note of the chart, and
  a note that would leave a tail under 250 ms stays a plain note. The tool
  also works out each chart's perfect score, for the stars.
- **The two voices must never play one pitch at the same time**: the
  speaker sits between the two pins and hears their difference, so two equal
  square waves cancel out and the note goes silent (heard on the console on
  2026-09-30, the tune's B against the bass's B). The tool refuses such an
  arrangement.
- **The look.** The logo and the menu font are Impact, rendered by
  `tools/logo.py` and `tools/font.py` into sprites; the menus are items in
  boxes with a guitar pick for a cursor. The title page draws the logo
  with a mask over a picture in the stage's look (`tools/crowd.py`, picked
  by Joel from the samples in `build/look/v6.html`): the crowd of the
  play page's wide shot across the whole width, black against the glow at
  the front of the stage, horns, fists and phones up, everyone jumping in
  their own time to the riff, and two spotlights swinging above as soft
  light. It is two layers on the chip, so the two move on their own: the
  light (rows 0 to 55, a picture for each of 32 places of the swing) and
  the crowd over it from the highest row the people reach (row 51) down
  (one for each of 16 steps of the jumping), so the spotlights come down
  into the glow behind the people. The crowd stands low, the heads cut by
  the bottom edge (three rows lower since 2026-10-02, Joel: "on all
  screens that have the crowd I think we can move it down a few pixels").
  PRESS B blinks halfway between the logo and the crowd; the main menu and
  the credits have the same crowd, so it stays put from one page to the
  other. The setlist shows two songs at a time (title and stars), and
  under a line who plays the one picked, its length, the best score and
  the number of notes. The booed-off page has its title in the logo's
  style (a sprite on the chip) over the same crowd booing (its own layers,
  laid like the title page's): the hands that were up shake a fist, a few thumbs go
  down, the phones keep filming, two people throw cups by turns (the game
  draws the cups, white, up into the dark and smaller as they fly at the
  stage); the spotlights are on as the page opens and go out one after
  the other; PRESS B sits halfway between the line that says how far the
  song got and the crowd, which stands as low as on the title page (Joel,
  2026-10-02), seven empty rows over and under it. Lines
  of small print are ten rows apart, a white box has a row of white above
  and below its letters: text that touches reads badly on this screen
  (Joel, 2026-09-30).
- **The stage** (`tools/scene.py`) is a photograph of the concert, made
  for the screen's two colours after Joel's mock-up and his sheet of
  dithered concert photographs (2026-10-01; the sample pages, v1 to v4,
  are in `build/look/`). The light comes first, in grey at four times the
  size: the haze, spots and beams from lamps above the frame, glows, the
  fog lit at the front of the stage. Then the stage and the people as
  black shapes cut out of it, built to a real body's proportions (seven
  and a half heads, slim arms and legs of the right length, boots seen
  from the front, the guitar low at the hips), the drummer behind a kit of
  real drum sizes, the amps and the mic stands to the players' scale. A
  shape gets a white rim only where it stands against dark (where it
  stands against the light its outline already reads), a backlit player a
  one-pixel glare round him, and seen close the guitar and the arms a thin
  line of light where they cross the body. The picture is brought down to
  the screen and dithered with blue noise: a pixel's threshold never
  changes, so between two frames only what moves flips. Every camera has
  a frame for each of four positions of the lights, five poses of the
  band (the two beats, the same two with a strum, a jump) and, where the
  crowd is in sight, two of the crowd: 240 frames, 240 KB on the chip.
  The game copies the frame it wants straight into the screen buffer
  (1.7 ms), then draws the highway and the hud over it. Earlier stages
  were drawn by hand (a cartoon), then modelled and lit as sprites over a
  drawn stage (`tools/stage.py`, kept); the photograph replaced them.
- **The fire on the pads** (`tools/flames.py`, picked by Joel from the
  samples in `build/look/v5.html`) is made of licks: a lick is born at the
  root of the fire in the middle of the pad, stretches upward, thins, lets
  go of the root, rises on its own and is gone. A hit lets three tall ones
  go at once; while a note is held a new one is born every three frames
  somewhere else along the root, to a schedule that repeats every 24
  frames, so the held fire goes round those frames without a seam; let go,
  nothing new is born and the fire dies down. A black line goes round the
  fire, but not down at the pad, so the fire comes out of it. Each pad's
  fire is its own; the frames (a hit's 16, a hold's 52) are sprites on the
  chip, 13 by 24 pixels, drawn after the pad and before the notes.
- **Drawing fast.** The play page has a lot in a frame, and the library
  draws boxes and lines a pixel at a time with the bounds checked for
  each; that cost the frame rate (54 a second with the band, where notes
  want 60). So the busy parts write straight into the screen buffer, a
  byte of eight rows at a time: the lights, the sprites, the highway's
  floor and lines, the box behind a note, the beat lines, the score's
  digits. The profile build (above) measures the parts. The chip is asked
  for the next byte before the last is put away, so a whole frame of the
  stage comes off it in 1.7 ms; the play page's whole frame takes 8.3 ms
  at the most of its 16 on the console, with fire on the pads (7.3 without
  it, 11 when the band was sprites). The title page takes 6.5 ms, the
  booed-off page 10.5, the main menu 15.7: the library's white bar and
  boxes, a pixel at a time, are most of it.
- **A note is judged before it leaves the queue.** A note not hit stays
  until its window has closed (90 ms after its time) and only then counts
  as a miss; dropped when it left the picture (64 ms), as it once was, it
  never counted on the console, whose frames were shorter than the gap.
  The emulator's frames come in bursts and hid that; judgement is checked
  on the console.

## Roadmap

1. The core loop, on the console, three songs. *Done 2026-09-30.*
2. The name, the title page, rock tracks with drums. *Done 2026-09-30.*
3. The menus: main, settings, difficulty, setlist with stars and best
   scores, results. *Done 2026-09-30.*
4. Star power, the pause menu. *Done 2026-09-30.*
5. The band behind the highway, the rock meter and failing. *Done
   2026-09-30; the band redone the same night, modelled and lit instead of
   drawn; the whole stage with the crowd after Joel's mock-up on
   2026-10-01; the stage as an animated photograph from seven cameras the
   same day.*
6. The difficulties tuned, more tracks.
7. Polish: messages, achievements. *The fire on the pads, the booed-off
   page, and the crowd of the title page, the menu and the booed-off page
   in the stage's look done 2026-10-01.*
