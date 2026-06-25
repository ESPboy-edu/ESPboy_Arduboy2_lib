#!/usr/bin/env python3
"""Generate compact Arduboy menu music from the Mutopia MIDI source."""

from __future__ import annotations

import argparse
import json
from pathlib import Path

import audio_data


PPQ_FALLBACK = 384
RIGHT_HAND_TRACK = 1
LEFT_HAND_TRACK = 2
TICKS_PER_EVENT_UNIT = 48
MEASURES_TO_EXPORT = 32
DEFAULT_THEME_START_MEASURES = (0, 32)
MIDI_TRANSPOSE = -12
REST_NOTE = 0
TEMPO_SCALE = 2 / 3


def read_vlq(data: bytes, offset: int) -> tuple[int, int]:
    value = 0
    while True:
        byte = data[offset]
        offset += 1
        value = (value << 7) | (byte & 0x7F)
        if (byte & 0x80) == 0:
            return value, offset


def read_tracks(path: Path) -> tuple[int, list[bytes]]:
    data = path.read_bytes()
    if data[:4] != b"MThd":
        raise ValueError(f"{path}: not a MIDI file")
    header_size = int.from_bytes(data[4:8], "big")
    header = data[8:8 + header_size]
    ppq = int.from_bytes(header[4:6], "big") if len(header) >= 6 else PPQ_FALLBACK

    tracks = []
    offset = 8 + header_size
    while offset < len(data):
        chunk_type = data[offset:offset + 4]
        chunk_size = int.from_bytes(data[offset + 4:offset + 8], "big")
        offset += 8
        if chunk_type == b"MTrk":
            tracks.append(data[offset:offset + chunk_size])
        offset += chunk_size
    return ppq, tracks


def read_track_notes(track: bytes) -> tuple[list[tuple[int, int, int]], list[tuple[int, int]]]:
    offset = 0
    tick = 0
    running_status: int | None = None
    active: dict[tuple[int, int], list[int]] = {}
    notes: list[tuple[int, int, int]] = []
    tempos: list[tuple[int, int]] = []

    while offset < len(track):
        delta, offset = read_vlq(track, offset)
        tick += delta
        status = track[offset]
        offset += 1

        if status == 0xFF:
            meta_type = track[offset]
            offset += 1
            size, offset = read_vlq(track, offset)
            payload = track[offset:offset + size]
            offset += size
            if meta_type == 0x51 and len(payload) == 3:
                tempos.append((tick, int.from_bytes(payload, "big")))
            if meta_type == 0x2F:
                break
            continue

        if status in (0xF0, 0xF7):
            size, offset = read_vlq(track, offset)
            offset += size
            continue

        if status < 0x80:
            offset -= 1
            if running_status is None:
                raise ValueError("MIDI running status without previous status")
            status = running_status
        else:
            running_status = status

        event_type = status & 0xF0
        channel = status & 0x0F
        if event_type in (0x80, 0x90):
            note = track[offset]
            velocity = track[offset + 1]
            offset += 2
            key = (channel, note)
            if event_type == 0x90 and velocity > 0:
                active.setdefault(key, []).append(tick)
            elif active.get(key):
                start = active[key].pop(0)
                notes.append((start, tick, note))
        elif event_type in (0xA0, 0xB0, 0xE0):
            offset += 2
        elif event_type in (0xC0, 0xD0):
            offset += 1
        else:
            raise ValueError(f"Unsupported MIDI event {status:#x}")

    return notes, tempos


def mono_voice_events(
    melody_notes: list[tuple[int, int, int]],
    bass_notes: list[tuple[int, int, int]],
    end_tick: int,
) -> list[tuple[int, int]]:
    changes = {0, end_tick}
    for source in (melody_notes, bass_notes):
        for start, end, _note in source:
            if start < end_tick:
                changes.add(start)
                changes.add(min(end, end_tick))

    events: list[tuple[int, int]] = []
    for start, end in zip(sorted(changes), sorted(changes)[1:]):
        if end <= start:
            continue
        melody_active = [note for note_start, note_end, note in melody_notes if note_start <= start and note_end > start]
        bass_active = [note for note_start, note_end, note in bass_notes if note_start <= start and note_end > start]
        if melody_active:
            note = max(melody_active) + MIDI_TRANSPOSE
        elif bass_active:
            note = min(bass_active)
        else:
            note = REST_NOTE
        duration = end - start
        units = max(1, round(duration / TICKS_PER_EVENT_UNIT))
        if events and events[-1][0] == note:
            events[-1] = (note, events[-1][1] + units)
        else:
            events.append((note, units))
    return events


def midi_frequency(note: int) -> int:
    return round(440.0 * (2.0 ** ((note - 69) / 12.0)))


def format_values(values: list[int], indent: str = "  ") -> str:
    lines = []
    for start in range(0, len(values), 16):
        chunk = values[start:start + 16]
        lines.append(indent + ", ".join(str(value) for value in chunk) + ",")
    return "\n".join(lines)


def generated_header(themes: list[dict], min_note: int, max_note: int, tick_ms: int) -> str:
    return audio_data.generated_header(themes, min_note, max_note, tick_ms)


def generated_source(themes: list[dict], min_note: int, max_note: int) -> str:
    return audio_data.generated_source(themes, min_note, max_note)


def slice_notes(
    notes: list[tuple[int, int, int]],
    start_tick: int,
    end_tick: int,
) -> list[tuple[int, int, int]]:
    return [
        (max(start, start_tick) - start_tick, min(end, end_tick) - start_tick, note)
        for start, end, note in notes
        if start < end_tick and end > start_tick
    ]


def theme_from_measure_range(
    name: str,
    melody_notes: list[tuple[int, int, int]],
    bass_notes: list[tuple[int, int, int]],
    ppq: int,
    start_measure: int,
    measures: int,
) -> dict:
    start_tick = start_measure * ppq * 2
    end_tick = (start_measure + measures) * ppq * 2
    events = mono_voice_events(
        slice_notes(melody_notes, start_tick, end_tick),
        slice_notes(bass_notes, start_tick, end_tick),
        end_tick - start_tick,
    )
    return {
        "id": name.lower().replace(" ", "-"),
        "name": name,
        "events": [{"note": note, "duration": duration} for note, duration in events],
    }


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--midi", type=Path, default=Path("tools/music/sources/entertainer.mid"))
    parser.add_argument("--out-dir", type=Path, default=Path("src"))
    parser.add_argument("--edited", type=Path, default=audio_data.EDITED_MENU_PATH)
    parser.add_argument("--force-midi", action="store_true")
    parser.add_argument("--measures", type=int, default=MEASURES_TO_EXPORT)
    return parser


def main() -> int:
    args = build_parser().parse_args()
    if args.edited.exists() and not args.force_midi:
        payload = audio_data.load_menu_music(Path("."))
        saved = audio_data.save_menu_music(Path("."), payload)
        counts = ", ".join(str(len(theme["events"])) for theme in saved["themes"])
        print(f"Generated {len(saved['themes'])} edited menu music theme(s), events {counts}.")
        return 0

    ppq, tracks = read_tracks(args.midi)
    if RIGHT_HAND_TRACK >= len(tracks):
        raise ValueError(f"{args.midi}: missing right-hand track {RIGHT_HAND_TRACK}")
    if LEFT_HAND_TRACK >= len(tracks):
        raise ValueError(f"{args.midi}: missing left-hand track {LEFT_HAND_TRACK}")
    melody_notes, _tempos = read_track_notes(tracks[RIGHT_HAND_TRACK])
    bass_notes, _tempos = read_track_notes(tracks[LEFT_HAND_TRACK])
    themes = [
        theme_from_measure_range(
            f"Theme {index + 1}",
            melody_notes,
            bass_notes,
            ppq,
            start_measure,
            args.measures,
        )
        for index, start_measure in enumerate(DEFAULT_THEME_START_MEASURES)
    ]
    used_notes = [
        event["note"]
        for theme in themes
        for event in theme["events"]
        if event["note"] != REST_NOTE
    ]
    min_note = min(used_notes)
    max_note = max(used_notes)

    # The MIDI tempo is 833333 microseconds per quarter note. The menu uses a
    # faster chiptune tempo so the rag reads better on the Arduboy speaker.
    tick_ms = round(833333 * TICKS_PER_EVENT_UNIT / ppq / 1000 * TEMPO_SCALE)

    args.out_dir.mkdir(parents=True, exist_ok=True)
    (args.out_dir / "MenuMusic.h").write_text(
        generated_header(themes, min_note, max_note, tick_ms),
        encoding="utf-8",
    )
    (args.out_dir / "MenuMusic.cpp").write_text(
        generated_source(themes, min_note, max_note),
        encoding="utf-8",
    )
    edited = {
        "id": "menu-music",
        "name": "Menu Music",
        "kind": "music",
        "tickMs": tick_ms,
        "themes": themes,
    }
    args.edited.parent.mkdir(parents=True, exist_ok=True)
    args.edited.write_text(json.dumps(edited, indent=2) + "\n", encoding="utf-8")
    event_counts = ", ".join(str(len(theme["events"])) for theme in themes)
    print(f"Generated {len(themes)} menu music theme(s), events {event_counts}, notes {min_note}-{max_note}.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
