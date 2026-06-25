"""Shared TableTop Studio audio data helpers."""

from __future__ import annotations

import json
import re
from pathlib import Path
from typing import Any


REST_NOTE = 0
EDITED_MENU_PATH = Path("tools/music/menu-music.json")
SOUND_EFFECTS_PATH = Path("tools/music/sound-effects.json")


def midi_frequency(note: int) -> int:
    return round(440.0 * (2.0 ** ((note - 69) / 12.0)))


def format_values(values: list[int], indent: str = "  ") -> str:
    lines = []
    for start in range(0, len(values), 16):
        chunk = values[start:start + 16]
        lines.append(indent + ", ".join(str(value) for value in chunk) + ",")
    return "\n".join(lines)


def generated_header(themes: list[dict[str, Any]], min_note: int, max_note: int, tick_ms: int) -> str:
    counts = ", ".join(str(len(theme["events"])) for theme in themes)
    externs = []
    for index in range(len(themes)):
        externs.append(f"extern const uint8_t MenuMusicTheme{index}Notes[] PROGMEM;")
        externs.append(f"extern const uint8_t MenuMusicTheme{index}Durations[] PROGMEM;")
    extern_text = "\n".join(externs)
    return f"""#pragma once

#include <Arduino.h>

constexpr uint8_t MENU_MUSIC_THEME_COUNT = {len(themes)};
constexpr uint8_t MENU_MUSIC_MIN_NOTE = {min_note};
constexpr uint8_t MENU_MUSIC_MAX_NOTE = {max_note};
constexpr uint8_t MENU_MUSIC_TICK_MS = {tick_ms};

extern const uint8_t MenuMusicThemeEventCounts[] PROGMEM;
{extern_text}
extern const uint8_t *const MenuMusicNotesByTheme[] PROGMEM;
extern const uint8_t *const MenuMusicDurationsByTheme[] PROGMEM;
extern const uint16_t MenuMusicFrequencies[] PROGMEM;
"""


def generated_source(themes: list[dict[str, Any]], min_note: int, max_note: int) -> str:
    frequencies = [midi_frequency(note) for note in range(min_note, max_note + 1)]
    chunks = []
    for index, theme in enumerate(themes):
        events = theme["events"]
        notes = [event["note"] for event in events]
        durations = [event["duration"] for event in events]
        chunks.append(f"""const uint8_t MenuMusicTheme{index}Notes[] PROGMEM = {{
{format_values(notes)}
}};

const uint8_t MenuMusicTheme{index}Durations[] PROGMEM = {{
{format_values(durations)}
}};
""")
    counts = ", ".join(str(len(theme["events"])) for theme in themes)
    notes_by_theme = ", ".join(f"MenuMusicTheme{index}Notes" for index in range(len(themes)))
    durations_by_theme = ", ".join(f"MenuMusicTheme{index}Durations" for index in range(len(themes)))
    return f"""#include "MenuMusic.h"

// Generated from TableTop Studio menu music data.
// Menu themes are compact monophonic Arduboy tone event streams.

{"".join(chunks)}
const uint8_t MenuMusicThemeEventCounts[] PROGMEM = {{ {counts} }};

const uint8_t *const MenuMusicNotesByTheme[] PROGMEM = {{ {notes_by_theme} }};

const uint8_t *const MenuMusicDurationsByTheme[] PROGMEM = {{ {durations_by_theme} }};

const uint16_t MenuMusicFrequencies[] PROGMEM = {{
{format_values(frequencies)}
}};
"""


def validate_events(events: Any) -> list[dict[str, int]]:
    if not isinstance(events, list) or not events:
        raise ValueError("Audio needs at least one event")
    clean = []
    for index, event in enumerate(events):
        if not isinstance(event, dict):
            raise ValueError(f"Audio event {index} must be an object")
        note = int(event.get("note", REST_NOTE))
        duration = int(event.get("duration", 1))
        if note < 0 or note > 127:
            raise ValueError(f"Audio event {index} note is outside MIDI range")
        if duration < 1 or duration > 255:
            raise ValueError(f"Audio event {index} duration must fit uint8_t")
        clean.append({"note": note, "duration": duration})
    return clean


def validate_themes(payload: dict[str, Any]) -> list[dict[str, Any]]:
    raw_themes = payload.get("themes")
    if raw_themes is None:
        return [
            {
                "id": "theme-a",
                "name": str(payload.get("name") or "Menu Music"),
                "events": validate_events(payload.get("events")),
            }
        ]
    if not isinstance(raw_themes, list) or not raw_themes:
        raise ValueError("Menu music needs at least one theme")
    themes = []
    for index, theme in enumerate(raw_themes):
        if not isinstance(theme, dict):
            raise ValueError(f"Menu music theme {index} must be an object")
        themes.append(
            {
                "id": str(theme.get("id") or f"theme-{index + 1}"),
                "name": str(theme.get("name") or f"Theme {index + 1}"),
                "events": validate_events(theme.get("events")),
            }
        )
    return themes


def parse_int_array(source: str, name: str) -> list[int]:
    match = re.search(
        rf"const\s+uint(?:8|16)_t\s+{name}\[\]\s+PROGMEM\s*=\s*\{{(.*?)\}};",
        source,
        flags=re.DOTALL,
    )
    if not match:
        raise ValueError(f"Could not find {name}")
    return [int(token) for token in re.findall(r"\d+", match.group(1))]


def load_menu_music(root: Path) -> dict[str, Any]:
    edited_path = root / EDITED_MENU_PATH
    if edited_path.exists():
        payload = json.loads(edited_path.read_text(encoding="utf-8"))
        themes = validate_themes(payload)
        payload["themes"] = themes
        payload["events"] = themes[0]["events"]
        payload["path"] = str(EDITED_MENU_PATH)
        return payload

    header = (root / "src/MenuMusic.h").read_text(encoding="utf-8")
    source = (root / "src/MenuMusic.cpp").read_text(encoding="utf-8")
    tick_ms = int(re.search(r"MENU_MUSIC_TICK_MS = (\d+)", header).group(1))
    notes = parse_int_array(source, "MenuMusicTheme0Notes")
    durations = parse_int_array(source, "MenuMusicTheme0Durations")
    events = [{"note": note, "duration": duration} for note, duration in zip(notes, durations)]
    return {
        "id": "menu-music",
        "name": "Menu Music",
        "kind": "music",
        "tickMs": tick_ms,
        "events": validate_events(events),
        "themes": [{"id": "theme-a", "name": "Theme A", "events": validate_events(events)}],
        "path": "src/MenuMusic.*",
    }


def save_menu_music(root: Path, payload: dict[str, Any]) -> dict[str, Any]:
    edited_path = root / EDITED_MENU_PATH
    if payload.get("themes") is None and edited_path.exists():
        existing = json.loads(edited_path.read_text(encoding="utf-8"))
        themes = validate_themes(existing)
        replacement_events = validate_events(payload.get("events"))
        themes[0] = themes[0] | {"events": replacement_events}
    else:
        themes = validate_themes(payload)
    tick_ms = int(payload.get("tickMs", 69))
    if tick_ms < 20 or tick_ms > 250:
        raise ValueError("tickMs must be between 20 and 250")
    used_notes = [
        event["note"]
        for theme in themes
        for event in theme["events"]
        if event["note"] != REST_NOTE
    ]
    if not used_notes:
        raise ValueError("Menu music needs at least one note")
    min_note = min(used_notes)
    max_note = max(used_notes)

    edited = {
        "id": "menu-music",
        "name": "Menu Music",
        "kind": "music",
        "tickMs": tick_ms,
        "themes": themes,
    }
    edited_path.parent.mkdir(parents=True, exist_ok=True)
    edited_path.write_text(json.dumps(edited, indent=2) + "\n", encoding="utf-8")
    (root / "src/MenuMusic.h").write_text(
        generated_header(themes, min_note, max_note, tick_ms),
        encoding="utf-8",
    )
    (root / "src/MenuMusic.cpp").write_text(
        generated_source(themes, min_note, max_note),
        encoding="utf-8",
    )
    return edited | {"events": themes[0]["events"], "path": str(EDITED_MENU_PATH)}


def load_effects(root: Path) -> list[dict[str, Any]]:
    path = root / SOUND_EFFECTS_PATH
    if not path.exists():
        return [
            {
                "id": "new-effect",
                "name": "New Effect",
                "kind": "effect",
                "tickMs": 50,
                "events": [{"note": 72, "duration": 2}],
            }
        ]
    payload = json.loads(path.read_text(encoding="utf-8"))
    effects = payload.get("effects", [])
    for effect in effects:
        effect["events"] = validate_events(effect.get("events"))
    return effects


def save_effect(root: Path, payload: dict[str, Any]) -> dict[str, Any]:
    effect_id = re.sub(r"[^a-z0-9_-]+", "-", str(payload.get("id") or payload.get("name") or "effect").lower()).strip("-")
    effect = {
        "id": effect_id or "effect",
        "name": str(payload.get("name") or "Effect"),
        "kind": "effect",
        "tickMs": int(payload.get("tickMs", 50)),
        "events": validate_events(payload.get("events")),
    }
    effects = [item for item in load_effects(root) if item.get("id") != effect["id"]]
    effects.append(effect)
    effects.sort(key=lambda item: item.get("id", ""))
    path = root / SOUND_EFFECTS_PATH
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps({"effects": effects}, indent=2) + "\n", encoding="utf-8")
    return effect | {"path": str(SOUND_EFFECTS_PATH)}
